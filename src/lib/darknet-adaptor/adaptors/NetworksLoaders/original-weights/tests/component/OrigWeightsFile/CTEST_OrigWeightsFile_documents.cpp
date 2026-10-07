#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "CTEST_OrigWeightsFile_provider.h"
#include "WeightsChecks.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsValidator.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h"

using namespace testing;
using namespace weights_test;

using darknet_adaptor::loaders::OrigWeightsInspection;
using darknet_adaptor::loaders::OrigWeightsReader;
using darknet_adaptor::loaders::OrigWeightsValidator;
using darknet_adaptor::loaders::OrigWeightsWriter;
using darknet_adaptor::loaders::WeightsArray;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsLayer;
using darknet_adaptor::loaders::WeightsReport;
using darknet_adaptor::loaders::WeightsSchema;

namespace
{

constexpr const uint64_t SEED = 0xD0C5EEDU;

NetSpec net(const std::string& name, int width, int height, int channels,
            const std::vector<LayerSpec>& layers)
{
  NetSpec spec;
  spec.name = name;
  spec.width = width;
  spec.height = height;
  spec.channels = channels;
  spec.layers = layers;
  return spec;
}

NetSpec every_kind_net()
{
  return net("every_kind", 16, 12, 3,
             {conv(6, 3).with_bn(), batchnorm(), conv(6, 1),
              shortcut({-3}, "per_channel", "softmax"), maxpool(2, 2),
              local(3, 3, 1, true), crnn(4, 3, 3).with_bn(),
              conv_lstm(3, 3).with_peephole().with_bn(), implicit(5, 2),
              conv(4, 1).with_bn(), rnn(6, 5).with_bn(), lstm(4).with_bn(),
              gru(3), connected(7).with_bn(), softmax()});
}

NetSpec skipping_net()
{
  return net("skipping", 6, 6, 2,
             {conv(4, 3).with_bn().with_dontloadscales(), conv(5, 3),
              conv(3, 3).with_dontload(), connected(4).with_bn()});
}

LayoutOptions loading(bool transposed = false)
{
  LayoutOptions options;
  options.loading = true;
  options.transposed = transposed;
  return options;
}

std::string variant_name(const TestParamInfo<WeightsVariant>& info)
{
  return info.param.name;
}

/**
 * @brief Checks the document holds the arrays of the layout with the pattern
 * values, the first floats of them alone for the limit given, and lists
 * every layer of the network.
 */
::testing::AssertionResult document_holds(const WeightsDocument& document,
                                          const NetSpec& spec,
                                          const Layout& layout,
                                          const Pattern& pattern,
                                          uint64_t floats)
{
  if (document.layers.size() != spec.layers.size()) {
    return ::testing::AssertionFailure()
           << document.layers.size() << " layers instead of "
           << spec.layers.size();
  }

  // the document arrays in the file order
  std::vector<std::tuple<int, const WeightsArray*>> arrays;

  for (size_t index = 0U; index < document.layers.size(); ++index) {
    const WeightsLayer& l = document.layers[index];

    if (l.index != static_cast<int>(index) ||
        l.type != layer_type(spec.layers[index].kind)) {
      return ::testing::AssertionFailure()
             << "the layer " << index << " is " << l.index << " of the type "
             << WeightsSchema::name(l.type);
    }

    for (const WeightsArray& a : l.arrays) {
      arrays.emplace_back(l.index, &a);
    }
  }

  size_t position = 0U;
  uint64_t remaining = floats;

  for (const Block& block : layout) {
    if (remaining == 0U) {
      break;
    }

    if (position >= arrays.size()) {
      return ::testing::AssertionFailure()
             << "the document has no " << describe(block);
    }

    const auto& [layer, array] = arrays[position++];
    const uint64_t count = std::min(remaining, block.count);

    if (layer != block.layer || array->values.size() != count ||
        WeightsSchema::name(array->kind).empty()) {
      return ::testing::AssertionFailure()
             << "the " << describe(block) << " is the layer " << layer << " "
             << WeightsSchema::name(array->sub) << " "
             << WeightsSchema::name(array->kind) << " of "
             << array->values.size() << " floats";
    }

    const PatternValues values = pattern.of(block);

    for (uint64_t index = 0U; index < count; ++index) {
      if (bits(array->values[index]) != bits(values(index))) {
        return ::testing::AssertionFailure()
               << describe(block) << " at the index " << index;
      }
    }

    remaining -= count;
  }

  if (position != arrays.size()) {
    return ::testing::AssertionFailure()
           << arrays.size() - position << " arrays of the document are extra";
  }

  return ::testing::AssertionSuccess();
}

/**
 * @brief The read and the write of the file make the same bytes, the arrays
 * of the document follow the network ones.
 */
void check_rewrite(const std::string& path, network& net)
{
  WeightsDocument document;

  ASSERT_TRUE(OrigWeightsReader{}.read(path, &net, document));

  const WeightsReport report = OrigWeightsValidator::validate(document, net);

  EXPECT_TRUE(report.ok()) << report.first_error();

  ASSERT_TRUE(OrigWeightsWriter{}.write(document, path + ".rewritten"));
  EXPECT_EQ(document.file_size(), file_size(path));
  EXPECT_TRUE(files_equal(path, path + ".rewritten"));
}

}  // namespace

class CTEST_OrigWeightsFile_documents : public Test
{
 public:
  WeightsWorkspace workspace{CTEST_OrigWeightsFile_provider::GENERATED_DIR};
};

TEST_F(CTEST_OrigWeightsFile_documents,
       read_splits_the_saved_file_by_the_network_arrays)
{
  const NetSpec spec = every_kind_net();
  const std::string weights = workspace.file("every_kind.weights");
  const Pattern pattern{SEED};

  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Train};

  ASSERT_TRUE(darknet.fill(spec.layout(), pattern));

  *darknet.net().cur_iteration = 9;

  save(darknet.net(), weights);

  WeightsDocument document;

  ASSERT_TRUE(OrigWeightsReader{}.read(weights, &darknet.net(), document));

  EXPECT_EQ(document.header.major, 0);
  EXPECT_EQ(document.header.minor, 2);
  EXPECT_EQ(document.header.revision, 5);
  EXPECT_EQ(document.header.seen, saved_seen(spec, ParseMode::Train, 9));
  EXPECT_TRUE(document.trailing.empty());
  EXPECT_TRUE(document_holds(document, spec, spec.layout(), pattern,
                             std::numeric_limits<uint64_t>::max()));
}

TEST_F(CTEST_OrigWeightsFile_documents,
       read_takes_the_arrays_the_file_made_for_the_load_holds)
{
  const NetSpec spec = skipping_net();
  const std::string cfg = workspace.write_cfg(spec);
  const Pattern pattern{SEED};
  const WeightsHeader header{0, 2, 5, 128U};

  DarknetNetwork darknet{cfg, ParseMode::Train};

  // the file of the arrays the network loads: no arrays of the dontload
  // layer, no batch normalization arrays of the dontloadscales one
  const std::string made = workspace.file("made.weights");

  ASSERT_TRUE(
      write_weights_file(made, header, spec.layout(loading()), pattern));

  OrigWeightsInspection inspection;

  ASSERT_TRUE(OrigWeightsValidator::inspect(made, darknet.net(), inspection));
  EXPECT_TRUE(inspection.loading_layout);
  EXPECT_EQ(inspection.expected_floats(), count_floats(spec.layout(loading())));
  EXPECT_EQ(inspection.saved_floats, count_floats(spec.layout()));

  WeightsDocument document;

  ASSERT_TRUE(OrigWeightsReader{}.read(made, &darknet.net(), document));
  EXPECT_TRUE(document_holds(document, spec, spec.layout(loading()), pattern,
                             std::numeric_limits<uint64_t>::max()));
  EXPECT_TRUE(document.layers[2].arrays.empty());

  check_rewrite(made, darknet.net());

  // the file darknet saves holds every array
  const std::string saved = workspace.file("saved.weights");

  ASSERT_TRUE(darknet.fill(spec.layout(), pattern));

  save(darknet.net(), saved);

  ASSERT_TRUE(OrigWeightsValidator::inspect(saved, darknet.net(), inspection));
  EXPECT_FALSE(inspection.loading_layout);

  ASSERT_TRUE(OrigWeightsReader{}.read(saved, &darknet.net(), document));
  EXPECT_TRUE(document_holds(document, spec, spec.layout(), pattern,
                             std::numeric_limits<uint64_t>::max()));

  check_rewrite(saved, darknet.net());
}

TEST_F(CTEST_OrigWeightsFile_documents,
       read_keeps_the_truncated_arrays_and_the_trailing_bytes)
{
  const NetSpec spec = every_kind_net();
  const Layout layout = spec.layout();
  const Pattern pattern{SEED};
  const WeightsHeader header{0, 1, 3, 77U};
  const uint64_t floats = count_floats(layout);
  const uint64_t full = header.size() + floats * sizeof(float);

  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Inference};

  for (const uint64_t size :
       {header.size(), header.size() + 1U, header.size() + 4U,
        header.size() + 4U * layout[0].count + 2U, full / 2U, full - 1U, full,
        full + 1U, full + 1000U}) {
    SCOPED_TRACE("the file of " + std::to_string(size) + " of " +
                 std::to_string(full) + " bytes");

    const std::string path = workspace.file("cut.weights");

    ASSERT_TRUE(write_weights_file(path, header, layout, pattern));

    if (size > full) {
      ASSERT_TRUE(append_bytes(path, size - full));
    } else {
      ASSERT_TRUE(truncate_file(path, size));
    }

    WeightsDocument document;

    ASSERT_TRUE(OrigWeightsReader{}.read(path, &darknet.net(), document));

    const uint64_t payload = size - header.size();

    EXPECT_EQ(document.header.seen, 77U);
    EXPECT_EQ(document.trailing.size(),
              size > full ? size - full : payload % sizeof(float));
    EXPECT_TRUE(document_holds(document, spec, layout, pattern,
                               std::min(floats, payload / sizeof(float))));

    check_rewrite(path, darknet.net());
  }
}

TEST_F(CTEST_OrigWeightsFile_documents, read_fails_without_the_network)
{
  const NetSpec spec = net("plain", 4, 4, 1, {conv(2, 3)});
  const std::string weights = workspace.file("plain.weights");

  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Inference};

  save(darknet.net(), weights);

  WeightsDocument document;

  EXPECT_FALSE(OrigWeightsReader{}.read(weights, nullptr, document));
  EXPECT_FALSE(OrigWeightsReader{}.read(workspace.file("missing.weights"),
                                        &darknet.net(), document));

  // the files shorter than the header, every cut shortens the same file
  for (uint64_t size = 20U; size-- > 0U;) {
    ASSERT_TRUE(truncate_file(weights, size));
    EXPECT_FALSE(OrigWeightsReader{}.read(weights, &darknet.net(), document))
        << size;
    EXPECT_FALSE(OrigWeightsValidator::validate(weights, darknet.net()).ok())
        << size;
  }
}

TEST_F(CTEST_OrigWeightsFile_documents, write_rejects_the_document_of_errors)
{
  WeightsDocument document;
  document.header =
      darknet_adaptor::loaders::WeightsHeader{0, 1, 0, 0x100000000ULL};

  const std::string path = workspace.file("errors.weights");

  EXPECT_FALSE(OrigWeightsWriter{}.write(document, path));
  EXPECT_EQ(file_size(path), 0U);

  document.header.seen = 5U;

  EXPECT_TRUE(OrigWeightsWriter{}.write(document, path));
  EXPECT_TRUE(
      file_matches(path, WeightsHeader{0, 1, 0, 5U}, Layout{}, Pattern{SEED}));
  EXPECT_FALSE(
      OrigWeightsWriter{}.write(document, workspace.file("no/such/x.weights")));
}

TEST_F(CTEST_OrigWeightsFile_documents, validator_reports_the_file_fit)
{
  const NetSpec spec = skipping_net();
  const Layout layout = spec.layout();
  const Pattern pattern{SEED};
  const WeightsHeader header{0, 2, 5, 0U};
  const uint64_t full = header.size() + count_floats(layout) * sizeof(float);

  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Train};

  const auto validate = [&](const Layout& file_layout, const int64_t change) {
    const std::string path = workspace.file("validated.weights");

    EXPECT_TRUE(write_weights_file(path, header, file_layout, pattern));

    const uint64_t size = file_size(path);

    if (change > 0) {
      EXPECT_TRUE(append_bytes(path, static_cast<uint64_t>(change)));
    } else if (change < 0) {
      EXPECT_TRUE(truncate_file(path, size - static_cast<uint64_t>(-change)));
    }

    return OrigWeightsValidator::validate(path, darknet.net());
  };

  // the file darknet saves
  WeightsReport report = validate(layout, 0);

  EXPECT_TRUE(report.ok());
  EXPECT_TRUE(report.warnings().empty());

  // the file made for the load
  report = validate(spec.layout(loading()), 0);

  EXPECT_TRUE(report.ok());
  ASSERT_EQ(report.warnings().size(), 1U);
  EXPECT_NE(report.warnings()[0].find("dontload"), std::string::npos);

  // the truncated file
  report = validate(layout, -8);

  EXPECT_TRUE(report.ok());
  ASSERT_EQ(report.warnings().size(), 1U);
  EXPECT_NE(
      report.warnings()[0].find("holds " + std::to_string(full - 8U - 20U) +
                                " of the " + std::to_string(full - 20U)),
      std::string::npos)
      << report.warnings()[0];

  // the file of the part of a float
  report = validate(layout, -3);

  EXPECT_TRUE(report.ok());
  ASSERT_EQ(report.warnings().size(), 2U);
  EXPECT_NE(report.warnings()[1].find("the 1 bytes of a float"),
            std::string::npos)
      << report.warnings()[1];

  // the extra bytes
  report = validate(layout, 13);

  EXPECT_TRUE(report.ok());
  ASSERT_EQ(report.warnings().size(), 1U);
  EXPECT_NE(report.warnings()[0].find("13 bytes after the network arrays"),
            std::string::npos)
      << report.warnings()[0];

  // the missing file
  report = OrigWeightsValidator::validate(workspace.file("missing.weights"),
                                          darknet.net());

  EXPECT_FALSE(report.ok());
}

TEST_F(CTEST_OrigWeightsFile_documents,
       validator_rejects_the_documents_of_the_gaps_between_the_arrays)
{
  const NetSpec spec = skipping_net();
  const std::string cfg = workspace.write_cfg(spec);
  const Pattern pattern{SEED};

  DarknetNetwork darknet{cfg, ParseMode::Train};

  ASSERT_TRUE(darknet.fill(spec.layout(), pattern));

  const std::string saved = workspace.file("saved.weights");

  save(darknet.net(), saved);

  // the document of every array the save writes
  WeightsDocument complete;

  ASSERT_TRUE(OrigWeightsReader{}.read(saved, &darknet.net(), complete));
  ASSERT_EQ(complete.layers[1].arrays.size(), 2U);

  const auto validated = [&darknet](const WeightsDocument& document) {
    return OrigWeightsValidator::validate(document, darknet.net());
  };

  EXPECT_TRUE(validated(complete).ok());

  // the documents of the arrays of the network up to a point: the truncated
  // file, the file saved up to a layer, the empty one
  WeightsDocument truncated = complete;

  truncated.layers[1].arrays.pop_back();
  truncated.layers[1].arrays.back().values.resize(3U);
  truncated.layers[2].arrays.clear();
  truncated.layers[3].arrays.clear();

  EXPECT_TRUE(validated(truncated).ok());

  WeightsDocument empty = complete;

  for (WeightsLayer& l : empty.layers) {
    l.arrays.clear();
  }

  EXPECT_TRUE(validated(empty).ok());

  // the array missing in the middle
  WeightsDocument missing = complete;

  missing.layers[1].arrays.erase(missing.layers[1].arrays.begin());

  WeightsReport report = validated(missing);

  EXPECT_FALSE(report.ok());
  EXPECT_NE(report.first_error().find("the document misses the layer 1 biases "
                                      "and holds the layer 1 weights after it"),
            std::string::npos)
      << report.first_error();

  // the shorter array followed by others
  WeightsDocument shorter = complete;

  shorter.layers[0].arrays[0].values.resize(2U);

  report = validated(shorter);

  EXPECT_FALSE(report.ok());
  EXPECT_NE(report.first_error().find("the layer 0 biases holds 2 of the 4 "
                                      "values of the network one and the "
                                      "layer 0 scales follows it"),
            std::string::npos)
      << report.first_error();

  // the array of no network layer
  WeightsDocument extra = complete;
  WeightsArray scales;

  scales.kind = darknet_adaptor::loaders::WeightsArrayKind::Scales;
  scales.values.assign(5U, 1.f);

  extra.layers[1].arrays.insert(extra.layers[1].arrays.begin() + 1, scales);

  report = validated(extra);

  EXPECT_FALSE(report.ok());
  EXPECT_NE(report.first_error().find(
                "the layer 1 scales isn't an array of the network"),
            std::string::npos)
      << report.first_error();

  // the document of the file made for the load and the array missing from
  // it: the gap of the arrays the load reads is the one reported
  const std::string made = workspace.file("made.weights");

  ASSERT_TRUE(write_weights_file(made, WeightsHeader{0, 2, 5, 0U},
                                 spec.layout(loading()), pattern));

  WeightsDocument loaded;

  ASSERT_TRUE(OrigWeightsReader{}.read(made, &darknet.net(), loaded));
  EXPECT_TRUE(validated(loaded).ok());

  loaded.layers[1].arrays.pop_back();

  report = validated(loaded);

  EXPECT_FALSE(report.ok());
  EXPECT_NE(report.first_error().find("the document misses the layer 1 weights "
                                      "and holds the layer 3 biases after it"),
            std::string::npos)
      << report.first_error();
}

/**
 * @brief The read of the files of the generated networks and the write of
 * their documents make the same bytes: the files darknet saves, the files of
 * every header version made for the load, the truncated files.
 */
class CTEST_OrigWeightsFile_rewrites : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace{CTEST_OrigWeightsFile_provider::GENERATED_DIR};
};

TEST_P(CTEST_OrigWeightsFile_rewrites, documents_write_the_files_they_read)
{
  const WeightsVariant& variant = GetParam();
  const Layout layout = variant.spec.layout();
  const Pattern pattern{variant.seed};

  DarknetNetwork darknet{workspace.write_cfg(variant.spec), variant.mode};

  ASSERT_TRUE(darknet.fill(layout, pattern));

  *darknet.net().cur_iteration = variant.iteration;

  const std::string saved = workspace.file(variant.name + ".weights");

  save(darknet.net(), saved);

  WeightsDocument document;

  ASSERT_TRUE(OrigWeightsReader{}.read(saved, &darknet.net(), document));
  EXPECT_TRUE(document_holds(document, variant.spec, layout, pattern,
                             std::numeric_limits<uint64_t>::max()));

  check_rewrite(saved, darknet.net());

  // the file of the variant header made for the load
  WeightsHeader header = variant.header;
  header.seen = header.stored_seen();

  const Layout loaded = variant.spec.layout(loading(header.transposed()));
  const std::string made = workspace.file(variant.name + ".made.weights");

  ASSERT_TRUE(write_weights_file(made, header, loaded, pattern));

  check_rewrite(made, darknet.net());

  // the truncated files
  const uint64_t size = file_size(made);

  for (const uint64_t cut :
       std::set<uint64_t>{header.size(), size / 2U, size - 1U}) {
    if (cut < header.size() || cut >= size) {
      continue;
    }

    SCOPED_TRACE("the file cut at the " + std::to_string(cut) + " bytes");

    ASSERT_TRUE(truncate_file(made, cut));

    check_rewrite(made, darknet.net());
  }
}

INSTANTIATE_TEST_SUITE_P(Layers, CTEST_OrigWeightsFile_rewrites,
                         ValuesIn(variants::layers()), variant_name);

INSTANTIATE_TEST_SUITE_P(
    Random, CTEST_OrigWeightsFile_rewrites,
    ValuesIn(variants::random(variants::DEFAULT_RANDOM_SEED,
                              variants::DEFAULT_RANDOM_COUNT)),
    variant_name);
