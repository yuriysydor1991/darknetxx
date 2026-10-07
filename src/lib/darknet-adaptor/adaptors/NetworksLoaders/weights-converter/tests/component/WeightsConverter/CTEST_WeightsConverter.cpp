#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "DarknetNetwork.h"
#include "NetworkChecks.h"
#include "WeightsFile.h"
#include "WeightsNetSpec.h"
#include "WeightsVariants.h"
#include "WeightsWorkspace.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Reader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Writer.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Reader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Writer.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkFiles.h"
#include "src/lib/zlib/ZlibController.h"
#include "src/log/default-logger/DefaultLogger.h"

using namespace testing;
using namespace weights_factory;

using darknet_adaptor::loaders::IWeightsFileFormatPtr;
using darknet_adaptor::loaders::NetworkFiles;
using darknet_adaptor::loaders::OrigWeightsReader;
using darknet_adaptor::loaders::OrigWeightsWriter;
using darknet_adaptor::loaders::WeightsConversion;
using darknet_adaptor::loaders::WeightsConverter;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsFileFormats;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Reader;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Writer;
using darknet_adaptor::loaders::dxxwjz2::v2::Dxxwjz2Reader;
using darknet_adaptor::loaders::dxxwjz2::v2::Dxxwjz2Writer;

namespace
{

/// @brief Keeps the info messages of the darknet parser out of the output.
class QuietLogEnvironment : public Environment
{
 public:
  void SetUp() override
  {
    default_logger::DefaultLogger::level(
        default_logger::DefaultLogger::LVL_WARNING);
  }
};

[[maybe_unused]] Environment* const quiet_log_environment =
    AddGlobalTestEnvironment(new QuietLogEnvironment);

constexpr const uint64_t SEED = 0xC0417E27U;

const IWeightsFileFormatPtr ORIGINAL = WeightsFileFormats::named("weights");
const IWeightsFileFormatPtr DXXWJZ1 = WeightsFileFormats::named("dxxwjz1");
const IWeightsFileFormatPtr DXXWJZ2 = WeightsFileFormats::named("dxxwjz2");

std::string variant_name(const TestParamInfo<WeightsVariant>& info)
{
  return info.param.name;
}

NetSpec small_net()
{
  NetSpec spec;
  spec.name = "small";
  spec.width = 6;
  spec.height = 6;
  spec.channels = 2;
  spec.layers = {conv(4, 3).with_bn(), maxpool(2, 2), connected(3).with_bn(),
                 rnn(3, 2)};
  return spec;
}

WeightsConversion conversion(const IWeightsFileFormatPtr& from,
                             const std::string& src,
                             const IWeightsFileFormatPtr& to,
                             const std::string& dst, const std::string& cfg)
{
  WeightsConversion result;
  result.from = from;
  result.src = src;
  result.to = to;
  result.dst = dst;
  result.cfg = cfg;
  return result;
}

/// @brief The files of the directory, the ones the conversion left behind.
std::vector<std::string> files_of(const std::string& dir)
{
  std::vector<std::string> files;

  for (const auto& entry : std::filesystem::directory_iterator{dir}) {
    files.push_back(entry.path().filename().string());
  }

  std::sort(files.begin(), files.end());

  return files;
}

/**
 * @brief Converts the original weights file to the dxxwjz1 and the dxxwjz2
 * ones and back, both ways and each format into itself, expecting the same
 * bytes.
 */
void check_back_and_forth(const std::string& weights, const std::string& cfg,
                          const std::string& dir)
{
  WeightsConverter converter;

  const std::string dxxwjz1 = dir + "/converted.dxxwjz1";
  const std::string again = dir + "/again.dxxwjz1";
  const std::string back = dir + "/back.weights";
  const std::string validated = dir + "/validated.weights";
  const std::string copied = dir + "/copied.weights";

  ASSERT_TRUE(
      converter.convert(conversion(ORIGINAL, weights, DXXWJZ1, dxxwjz1, cfg)));
  EXPECT_TRUE(zlibi::ZlibController::is_gzip_file(dxxwjz1));

  // the dxxwjz1 file needs no network to convert back
  ASSERT_TRUE(
      converter.convert(conversion(DXXWJZ1, dxxwjz1, ORIGINAL, back, "")));
  EXPECT_TRUE(files_equal(weights, back));

  // and converts back validated against the network the same way
  ASSERT_TRUE(converter.convert(
      conversion(DXXWJZ1, dxxwjz1, ORIGINAL, validated, cfg)));
  EXPECT_TRUE(files_equal(weights, validated));

  // each format converts into itself
  ASSERT_TRUE(
      converter.convert(conversion(DXXWJZ1, dxxwjz1, DXXWJZ1, again, cfg)));
  EXPECT_TRUE(files_equal(dxxwjz1, again));

  ASSERT_TRUE(
      converter.convert(conversion(ORIGINAL, weights, ORIGINAL, copied, cfg)));
  EXPECT_TRUE(files_equal(weights, copied));

  // the dxxwjz2 file keeps the weights of the others the same way
  const std::string dxxwjz2 = dir + "/converted.dxxwjz2";
  const std::string from_dxxwjz2 = dir + "/from_dxxwjz2.weights";
  const std::string twice = dir + "/twice.dxxwjz2";
  const std::string through_dxxwjz1 = dir + "/through.dxxwjz1";
  const std::string through_back = dir + "/through.weights";

  ASSERT_TRUE(
      converter.convert(conversion(ORIGINAL, weights, DXXWJZ2, dxxwjz2, cfg)));
  ASSERT_TRUE(converter.convert(
      conversion(DXXWJZ2, dxxwjz2, ORIGINAL, from_dxxwjz2, "")));
  EXPECT_TRUE(files_equal(weights, from_dxxwjz2));

  ASSERT_TRUE(
      converter.convert(conversion(DXXWJZ2, dxxwjz2, DXXWJZ2, twice, "")));
  EXPECT_TRUE(files_equal(dxxwjz2, twice));

  ASSERT_TRUE(converter.convert(
      conversion(DXXWJZ2, dxxwjz2, DXXWJZ1, through_dxxwjz1, cfg)));
  EXPECT_TRUE(files_equal(dxxwjz1, through_dxxwjz1));
  ASSERT_TRUE(converter.convert(
      conversion(DXXWJZ1, through_dxxwjz1, ORIGINAL, through_back, "")));
  EXPECT_TRUE(files_equal(weights, through_back));
}

}  // namespace

class CTEST_WeightsConverter : public Test
{
 public:
  WeightsWorkspace workspace{CTEST_WeightsConverter_DATA_DIR};

  /// @brief The weights file of the pattern filled network and its cfg.
  std::string saved(const NetSpec& spec, std::string& cfg)
  {
    cfg = workspace.write_cfg(spec);

    DarknetNetwork darknet{cfg, ParseMode::Train};

    EXPECT_TRUE(darknet.fill(spec.layout(), Pattern{SEED}));

    *darknet.net().cur_iteration = 5;

    const std::string weights = workspace.file(spec.name + ".weights");

    EXPECT_TRUE(OrigWeightsWriter{}.save(darknet.net(), weights));

    return weights;
  }
};

TEST_F(CTEST_WeightsConverter, dxxwjz2_file_keeps_the_network_description)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const std::string names = workspace.file("small.names");
  const std::string dst = workspace.file("small.dxxwjz2");
  std::string text;

  {
    std::ofstream file{names};
    file << "cat\r\ndog\r\nbird\r\n";
  }

  WeightsConversion described =
      conversion(ORIGINAL, weights, DXXWJZ2, dst, cfg);

  described.names = names;

  ASSERT_TRUE(WeightsConverter{}.convert(described));
  ASSERT_TRUE(NetworkFiles::read_text(cfg, text));

  WeightsDocument document;

  ASSERT_TRUE(Dxxwjz2Reader{}.read(dst, nullptr, document));
  EXPECT_EQ(document.network.cfg(), text);
  EXPECT_EQ(document.network.classes(),
            (std::vector<std::string>{"cat", "dog", "bird"}));
  EXPECT_EQ(document.training.size(), 0U);

  // the network description and the training record go on, while the
  // formats of the weights alone keep none of them
  document.training.record(1, 2.f, 2.f);
  document.training.seconds(60.0);

  const std::string trained = workspace.file("trained.dxxwjz2");
  const std::string again = workspace.file("again.dxxwjz2");

  ASSERT_TRUE(Dxxwjz2Writer{}.write(document, trained));
  ASSERT_TRUE(WeightsConverter{}.convert(
      conversion(DXXWJZ2, trained, DXXWJZ2, again, "")));
  EXPECT_TRUE(files_equal(trained, again));
}

TEST_F(CTEST_WeightsConverter, dxxwjz2_destination_needs_the_network_cfg)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const std::string dxxwjz1 = workspace.file("small.dxxwjz1");
  const std::string dst = workspace.file("small.dxxwjz2");

  ASSERT_TRUE(WeightsConverter{}.convert(
      conversion(ORIGINAL, weights, DXXWJZ1, dxxwjz1, cfg)));

  EXPECT_FALSE(WeightsConverter{}.convert(
      conversion(DXXWJZ1, dxxwjz1, DXXWJZ2, dst, "")));
  EXPECT_FALSE(std::filesystem::exists(dst));

  EXPECT_TRUE(WeightsConverter{}.convert(
      conversion(DXXWJZ1, dxxwjz1, DXXWJZ2, dst, cfg)));
}

TEST_F(CTEST_WeightsConverter, network_to_train_is_made_of_the_cfg)
{
  const std::string cfg = workspace.write_cfg(small_net());
  const std::string dst = workspace.file("to_train.dxxwjz2");
  WeightsDocument document;
  std::string text;

  ASSERT_TRUE(
      WeightsConverter{}.convert(conversion(nullptr, "", DXXWJZ2, dst, cfg)));
  ASSERT_TRUE(Dxxwjz2Reader{}.read(dst, nullptr, document));
  ASSERT_TRUE(NetworkFiles::read_text(cfg, text));

  EXPECT_EQ(document.network.cfg(), text);
  EXPECT_TRUE(document.layers.empty());
  EXPECT_EQ(document.floats(), 0U);

  // the network to train goes on as itself, the formats of the weights alone
  // would keep nothing of it
  const std::string again = workspace.file("again.dxxwjz2");

  ASSERT_TRUE(
      WeightsConverter{}.convert(conversion(DXXWJZ2, dst, DXXWJZ2, again, "")));
  EXPECT_TRUE(files_equal(dst, again));

  for (const auto& format : {ORIGINAL, DXXWJZ1}) {
    const std::string nothing = workspace.file("nothing");

    EXPECT_FALSE(WeightsConverter{}.convert(
        conversion(DXXWJZ2, dst, format, nothing, cfg)));
    EXPECT_FALSE(WeightsConverter{}.convert(
        conversion(nullptr, "", format, nothing, cfg)));
    EXPECT_FALSE(std::filesystem::exists(nothing));
  }

  EXPECT_FALSE(
      WeightsConverter{}.convert(conversion(nullptr, "", DXXWJZ2, dst, "")));
}

TEST_F(CTEST_WeightsConverter, class_names_of_the_conversion_go_first)
{
  // the names= key of the cfg names the classes of none given
  NetSpec spec = small_net();
  std::string cfg;
  const std::string weights = saved(spec, cfg);
  const std::string keyed = workspace.file("keyed.names");
  const std::string given = workspace.file("given.names");
  std::string text;

  std::ofstream{keyed} << "keyed\n";
  std::ofstream{given} << "given\n";

  ASSERT_TRUE(NetworkFiles::read_text(cfg, text));

  const std::string named_cfg = workspace.file("named.cfg");

  const std::string net_section{"[net]\n"};

  std::ofstream{named_cfg} << text.insert(
      text.find(net_section) + net_section.size(), "names=" + keyed + "\n");

  const std::string from_key = workspace.file("from_key.dxxwjz2");
  const std::string from_source = workspace.file("from_source.dxxwjz2");
  const std::string from_given = workspace.file("from_given.dxxwjz2");
  WeightsDocument document;

  ASSERT_TRUE(WeightsConverter{}.convert(
      conversion(ORIGINAL, weights, DXXWJZ2, from_key, named_cfg)));
  ASSERT_TRUE(Dxxwjz2Reader{}.read(from_key, nullptr, document));
  EXPECT_EQ(document.network.classes(), (std::vector<std::string>{"keyed"}));

  // the classes of the source are over the names= key ones
  document.network.classes({"source"});
  ASSERT_TRUE(Dxxwjz2Writer{}.write(document, from_source));
  ASSERT_TRUE(WeightsConverter{}.convert(
      conversion(DXXWJZ2, from_source, DXXWJZ2, from_given, named_cfg)));
  ASSERT_TRUE(Dxxwjz2Reader{}.read(from_given, nullptr, document));
  EXPECT_EQ(document.network.classes(), (std::vector<std::string>{"source"}));

  // and the names file of the conversion is over both
  WeightsConversion named =
      conversion(DXXWJZ2, from_source, DXXWJZ2, from_given, named_cfg);

  named.names = given;

  ASSERT_TRUE(WeightsConverter{}.convert(named));
  ASSERT_TRUE(Dxxwjz2Reader{}.read(from_given, nullptr, document));
  EXPECT_EQ(document.network.classes(), (std::vector<std::string>{"given"}));
}

TEST_F(CTEST_WeightsConverter, reference_weights_convert_back_and_forth)
{
  for (const std::string& name : {std::string{"1"}, std::string{"2"}}) {
    SCOPED_TRACE(name);

    check_back_and_forth(
        std::string{CTEST_WeightsConverter_REFERENCE_DIR} + "/" + name +
            ".weights",
        std::string{CTEST_WeightsConverter_REFERENCE_DIR} + "/" + name + ".cfg",
        workspace.dir());
  }
}

TEST_F(CTEST_WeightsConverter, destination_directories_are_made)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const std::string dst = workspace.file("made/deep/dirs/small.dxxwjz1");

  ASSERT_TRUE(WeightsConverter{}.convert(
      conversion(ORIGINAL, weights, DXXWJZ1, dst, cfg)));
  EXPECT_TRUE(std::filesystem::is_regular_file(dst));
}

TEST_F(CTEST_WeightsConverter, destinations_are_replaced_and_sources_kept)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const std::string dst = workspace.file("replaced.dxxwjz1");
  const std::string copy = workspace.file("copy.weights");

  {
    std::ofstream old{dst};
    old << "the previous file";
  }

  ASSERT_TRUE(WeightsConverter{}.convert(
      conversion(ORIGINAL, weights, DXXWJZ1, dst, cfg)));

  // the conversion of the file into itself
  std::filesystem::copy_file(weights, copy);

  ASSERT_TRUE(WeightsConverter{}.convert(
      conversion(ORIGINAL, copy, ORIGINAL, copy, cfg)));
  EXPECT_TRUE(files_equal(weights, copy));

  ASSERT_TRUE(
      WeightsConverter{}.convert(conversion(DXXWJZ1, dst, DXXWJZ1, dst, cfg)));

  WeightsConversion back = conversion(DXXWJZ1, dst, ORIGINAL, copy, "");

  ASSERT_TRUE(WeightsConverter{}.convert(back));
  EXPECT_TRUE(files_equal(weights, copy));

  // no temporary files are left behind
  EXPECT_EQ(files_of(workspace.dir()),
            (std::vector<std::string>{"copy.weights", "replaced.dxxwjz1",
                                      "small.cfg", "small.weights"}));
}

TEST_F(CTEST_WeightsConverter, failed_conversions_leave_no_destination)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const std::string dxxwjz1 = workspace.file("small.dxxwjz1");

  ASSERT_TRUE(WeightsConverter{}.convert(
      conversion(ORIGINAL, weights, DXXWJZ1, dxxwjz1, cfg)));

  // the network of another cfg and a broken cfg
  NetSpec other = small_net();
  other.name = "other";
  other.layers[0] = connected(4);

  const std::string other_cfg = workspace.write_cfg(other);
  const std::string broken_cfg = workspace.file("broken.cfg");

  {
    std::ofstream broken{broken_cfg};
    broken << "[net]\nwidth=6\nheight=6\nchannels=2\n[convolutional]\n"
              "filters=2\nsize=3\n[route]\nlayers=-7\n";
  }

  // the corrupted dxxwjz1 file
  const std::string corrupted = workspace.file("corrupted.dxxwjz1");

  std::filesystem::copy_file(dxxwjz1, corrupted);
  std::filesystem::resize_file(corrupted,
                               std::filesystem::file_size(corrupted) / 2U);

  const std::string dst = workspace.file("out/never.file");

  const std::vector<std::pair<std::string, WeightsConversion>> failures{
      {"the original weights without the cfg",
       conversion(ORIGINAL, weights, DXXWJZ1, dst, "")},
      {"the missing source",
       conversion(ORIGINAL, workspace.file("missing.weights"), DXXWJZ1, dst,
                  cfg)},
      {"the directory source",
       conversion(DXXWJZ1, workspace.dir(), ORIGINAL, dst, "")},
      {"the missing cfg", conversion(ORIGINAL, weights, DXXWJZ1, dst,
                                     workspace.file("missing.cfg"))},
      {"the broken cfg",
       conversion(ORIGINAL, weights, DXXWJZ1, dst, broken_cfg)},
      {"the dxxwjz1 file of another network",
       conversion(DXXWJZ1, dxxwjz1, ORIGINAL, dst, other_cfg)},
      {"the corrupted dxxwjz1 file",
       conversion(DXXWJZ1, corrupted, ORIGINAL, dst, "")},
      {"the original weights file read as the dxxwjz1 one",
       conversion(DXXWJZ1, weights, ORIGINAL, dst, "")},
      {"the dxxwjz1 file read as the original weights one",
       conversion(ORIGINAL, dxxwjz1, DXXWJZ1, dst, cfg)},
      {"the empty destination",
       conversion(ORIGINAL, weights, DXXWJZ1, "", cfg)},
      {"the destination of the file parent",
       conversion(ORIGINAL, weights, DXXWJZ1, weights + "/x.dxxwjz1", cfg)},
      {"the destination directory",
       conversion(ORIGINAL, weights, DXXWJZ1, workspace.dir(), cfg)}};

  for (const auto& [name, failure] : failures) {
    SCOPED_TRACE(name);

    EXPECT_FALSE(WeightsConverter{}.convert(failure));
    EXPECT_FALSE(std::filesystem::exists(dst));
  }

  // no temporary files are left behind
  EXPECT_EQ(files_of(workspace.dir()),
            (std::vector<std::string>{"broken.cfg", "corrupted.dxxwjz1",
                                      "other.cfg", "small.cfg", "small.dxxwjz1",
                                      "small.weights"}));
}

TEST_F(CTEST_WeightsConverter, array_gaps_fail_the_original_weights_of_the_cfg)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const std::string dxxwjz1 = workspace.file("small.dxxwjz1");

  ASSERT_TRUE(WeightsConverter{}.convert(
      conversion(ORIGINAL, weights, DXXWJZ1, dxxwjz1, cfg)));

  // the document edited by hand: the scales of the first layer removed, the
  // arrays after them are there
  darknet_adaptor::loaders::WeightsDocument document;

  ASSERT_TRUE(Dxxwjz1Reader{}.read(dxxwjz1, nullptr, document));
  ASSERT_GT(document.layers[0].arrays.size(), 2U);

  document.layers[0].arrays.erase(document.layers[0].arrays.begin() + 1);

  const std::string gapped = workspace.file("gapped.dxxwjz1");

  ASSERT_TRUE(Dxxwjz1Writer{}.write(document, gapped));

  // the original weights file keeps no names of its arrays, so the arrays
  // after the gap would load into the wrong ones of the network
  const std::string dst = workspace.file("gapped.weights");

  EXPECT_FALSE(WeightsConverter{}.convert(
      conversion(DXXWJZ1, gapped, ORIGINAL, dst, cfg)));
  EXPECT_FALSE(std::filesystem::exists(dst));

  // the dxxwjz1 file keeps the names, the network keeps the values of the
  // missing array
  const std::string again = workspace.file("again.dxxwjz1");

  EXPECT_TRUE(WeightsConverter{}.convert(
      conversion(DXXWJZ1, gapped, DXXWJZ1, again, cfg)));

  // with no network to check against, the document is written as it is
  EXPECT_TRUE(WeightsConverter{}.convert(
      conversion(DXXWJZ1, gapped, ORIGINAL, dst, "")));
  EXPECT_EQ(file_size(dst), document.file_size());
}

TEST_F(CTEST_WeightsConverter, stop_requests_stop_the_conversion)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const std::string dst = workspace.file("stopped.dxxwjz1");

  // the stop of the first, the second and the third step
  for (int steps = 0; steps < 3; ++steps) {
    SCOPED_TRACE("the stop after " + std::to_string(steps) + " steps");

    int asked = 0;
    WeightsConversion stopped =
        conversion(ORIGINAL, weights, DXXWJZ1, dst, cfg);

    stopped.stop_requested = [&asked, steps]() { return asked++ >= steps; };

    EXPECT_FALSE(WeightsConverter{}.convert(stopped));
    EXPECT_FALSE(std::filesystem::exists(dst));
  }

  EXPECT_EQ(files_of(workspace.dir()),
            (std::vector<std::string>{"small.cfg", "small.weights"}));

  // the stop never requested
  WeightsConversion completed =
      conversion(ORIGINAL, weights, DXXWJZ1, dst, cfg);

  completed.stop_requested = []() { return false; };

  EXPECT_TRUE(WeightsConverter{}.convert(completed));
  EXPECT_TRUE(std::filesystem::exists(dst));
}

TEST_F(CTEST_WeightsConverter, truncated_sources_convert_with_the_warnings)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const uint64_t size = file_size(weights);

  // the file of the part of its floats and the extra bytes
  for (const int64_t change : {-6, -4, 7}) {
    SCOPED_TRACE("the change of " + std::to_string(change) + " bytes");

    const std::string changed = workspace.file("changed.weights");

    std::filesystem::copy_file(
        weights, changed, std::filesystem::copy_options::overwrite_existing);

    if (change > 0) {
      ASSERT_TRUE(append_bytes(changed, static_cast<uint64_t>(change)));
    } else {
      ASSERT_TRUE(
          truncate_file(changed, size - static_cast<uint64_t>(-change)));
    }

    check_back_and_forth(changed, cfg, workspace.dir());
  }
}

/**
 * @brief The files of the generated networks convert back and forth: the
 * files darknet saves and the files of every header version made for the
 * load.
 */
class CTEST_WeightsConverter_variants : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace{CTEST_WeightsConverter_DATA_DIR};
};

TEST_P(CTEST_WeightsConverter_variants, converts_back_and_forth)
{
  const WeightsVariant& variant = GetParam();
  const std::string cfg = workspace.write_cfg(variant.spec);
  const std::string saved = workspace.file(variant.name + ".weights");
  const std::string made = workspace.file(variant.name + ".made.weights");

  {
    DarknetNetwork darknet{cfg, variant.mode};

    ASSERT_TRUE(darknet.fill(variant.spec.layout(), Pattern{variant.seed}));

    *darknet.net().cur_iteration = variant.iteration;

    ASSERT_TRUE(OrigWeightsWriter{}.save(darknet.net(), saved));
  }

  WeightsHeader header = variant.header;
  header.seen = header.stored_seen();

  LayoutOptions loading;
  loading.loading = true;
  loading.transposed = header.transposed();

  ASSERT_TRUE(write_weights_file(made, header, variant.spec.layout(loading),
                                 Pattern{variant.seed}));

  for (const std::string& weights : {saved, made}) {
    SCOPED_TRACE(weights);

    check_back_and_forth(weights, cfg, workspace.dir());
  }
}

INSTANTIATE_TEST_SUITE_P(Layers, CTEST_WeightsConverter_variants,
                         ValuesIn(variants::layers()), variant_name);

INSTANTIATE_TEST_SUITE_P(
    Random, CTEST_WeightsConverter_variants,
    ValuesIn(variants::random(variants::DEFAULT_RANDOM_SEED,
                              variants::DEFAULT_RANDOM_COUNT)),
    variant_name);
