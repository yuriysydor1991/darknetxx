#include <gtest/gtest.h>

#include <algorithm>
#include <climits>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <limits>
#include <string>
#include <vector>

#include "CTEST_Dxxwjz1_provider.h"
#include "Dxxwjz1Checks.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Reader.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Validator.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Writer.h"
#include "src/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/NetworksLoaders/original-weights/OrigWeightsWriter.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/log/default-logger/DefaultLogger.h"
#include "src/zlib/ZlibController.h"

using namespace testing;
using namespace dxxwjz1_test;

using darknet_adaptor::loaders::OrigWeightsReader;
using darknet_adaptor::loaders::OrigWeightsWriter;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsReport;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Reader;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Validator;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Writer;

namespace
{

/**
 * @brief Keeps the info messages of the darknet parser (every layer of the
 * thousands of the generated networks) out of the tests output, while the
 * warnings and the errors stay.
 */
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

constexpr const uint64_t SEED = 0xD1C3A5E1U;

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

/// @brief The small network of every weights layer kind.
NetSpec every_kind_net()
{
  return net("every_kind", 16, 12, 3,
             {conv(6, 3).with_bn(),                       // 0
              batchnorm(),                                // 1
              conv(6, 1),                                 // 2
              shortcut({-3}, "per_channel", "softmax"),   // 3
              maxpool(2, 2),                              // 4
              local(3, 3, 1, true),                       // 5
              crnn(4, 3, 3).with_bn(),                    // 6
              conv_lstm(3, 3).with_peephole().with_bn(),  // 7
              implicit(5, 2),                             // 8
              conv(4, 1).with_bn(),                       // 9
              rnn(6, 5).with_bn(),                        // 10
              lstm(4).with_bn(),                          // 11
              gru(3),                                     // 12
              connected(7).with_bn(),                     // 13
              softmax()});                                // 14
}

LayoutOptions loading(bool transposed = false)
{
  LayoutOptions options;
  options.loading = true;
  options.transposed = transposed;
  return options;
}

std::vector<float> copy_of(const float* values, int count)
{
  return std::vector<float>(values, values + count);
}

::testing::AssertionResult same_bits(const std::vector<float>& actual,
                                     const std::vector<float>& expected)
{
  if (actual.size() != expected.size()) {
    return ::testing::AssertionFailure()
           << actual.size() << " floats instead of " << expected.size();
  }

  for (size_t index = 0U; index < actual.size(); ++index) {
    if (bits(actual[index]) != bits(expected[index])) {
      return ::testing::AssertionFailure()
             << actual[index] << " at the index " << index << " instead of "
             << expected[index];
    }
  }

  return ::testing::AssertionSuccess();
}

/// @brief The text of the file, the gzip compressed one uncompressed.
std::string text_of(const std::string& path)
{
  std::string text;
  EXPECT_TRUE(read_text(path, text)) << path;
  return text;
}

std::vector<char> bytes_of(const std::string& path)
{
  std::ifstream file{path, std::ios::binary};

  return std::vector<char>{std::istreambuf_iterator<char>{file},
                           std::istreambuf_iterator<char>{}};
}

void write_bytes(const std::string& path, const std::vector<char>& bytes)
{
  std::ofstream file{path, std::ios::binary | std::ios::trunc};
  file.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
}

}  // namespace

class CTEST_Dxxwjz1 : public Test
{
 public:
  WeightsWorkspace workspace{CTEST_Dxxwjz1_provider::GENERATED_DIR};

  /// @brief Saves the pattern filled network of the spec, returns the path.
  std::string saved(const NetSpec& spec, const std::string& name,
                    int iteration = 3)
  {
    const std::string path = workspace.file(name);

    DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Train};

    EXPECT_TRUE(darknet.fill(spec.layout(), Pattern{SEED}));

    *darknet.net().cur_iteration = iteration;

    save(darknet.net(), path);

    return path;
  }
};

TEST_F(CTEST_Dxxwjz1, the_file_is_the_gzip_compressed_json_document)
{
  const NetSpec spec = every_kind_net();
  const std::string path = saved(spec, "every_kind.dxxwjz1");

  EXPECT_TRUE(zlibi::ZlibController::is_gzip_file(path));

  const std::string text = text_of(path);

  // the format, the version and the generator come first, the header of the
  // current darknet version and the seen of the iteration follow
  EXPECT_EQ(text.rfind("{\n  \"format\": \"dxxwjz1\",\n  \"version\": 1,\n  "
                       "\"generator\": \"darknetxx ",
                       0U),
            0U)
      << text.substr(0U, 200U);
  EXPECT_NE(text.find("\"header\": {\"major\": 0, \"minor\": 2, \"revision\": "
                      "5, \"seen\": 3},"),
            std::string::npos);

  fjson document;

  ASSERT_TRUE(read_document(path, document));
  EXPECT_TRUE(document_matches(document, spec, saved_header(3U), spec.layout(),
                               Pattern{SEED}));
}

TEST_F(CTEST_Dxxwjz1, the_text_keeps_a_layer_on_a_line_in_the_file_order)
{
  const NetSpec spec = net(
      "lines", 4, 4, 2,
      {conv(2, 1).with_bn(), maxpool(2, 2), connected(3).with_bn(), rnn(2, 2)});
  const std::string text = text_of(saved(spec, "lines.dxxwjz1"));

  // the layers in the index order, the arrays in the order of the original
  // weights file
  const std::vector<std::string> order{
      "{\"index\": 0, \"type\": \"convolutional\", \"arrays\": {",
      "\"biases\": [",
      "\"scales\": [",
      "\"rolling_mean\": [",
      "\"rolling_variance\": [",
      "\"weights\": [",
      "{\"index\": 1, \"type\": \"maxpool\"}",
      "{\"index\": 2, \"type\": \"connected\", \"arrays\": {",
      "\"biases\": [",
      "\"weights\": [",
      "\"scales\": [",
      "{\"index\": 3, \"type\": \"rnn\", \"sublayers\": {",
      "\"input_layer\": {\"arrays\": {",
      "\"self_layer\": {\"arrays\": {",
      "\"output_layer\": {\"arrays\": {"};

  size_t position = 0U;

  for (const std::string& part : order) {
    const size_t found = text.find(part, position);

    ASSERT_NE(found, std::string::npos)
        << part << " after " << position << " of\n"
        << text.substr(0U, 3000U);

    position = found + part.size();
  }

  // a layer a line, an array a line: the 5 lines of the document start, the
  // 7 lines of the convolutional layer, the one of the maxpool, the 7 of the
  // connected, the 14 of the rnn and the 3 of the document end
  EXPECT_EQ(std::count(text.begin(), text.end(), '\n'), 37);
}

TEST_F(CTEST_Dxxwjz1, the_saves_are_byte_identical)
{
  const NetSpec spec = every_kind_net();

  EXPECT_TRUE(
      files_equal(saved(spec, "first.dxxwjz1"), saved(spec, "second.dxxwjz1")));
}

TEST_F(CTEST_Dxxwjz1, special_float_values_round_trip_bit_exact)
{
  static const uint32_t specials[] = {
      0x00000000U,  // +0
      0x80000000U,  // -0
      0x00000001U,  // the smallest denormal
      0x807FFFFFU,  // the biggest negative denormal
      0x00800000U,  // FLT_MIN
      0x7F7FFFFFU,  // FLT_MAX
      0xFF7FFFFFU,  // -FLT_MAX
      0x7F800000U,  // +inf
      0xFF800000U,  // -inf
      0x7FC00000U,  // the quiet NaN
      0xFFC12345U,  // the negative quiet NaN with the payload
      0x7FA00001U,  // the signaling NaN
      0x3F800000U,  // 1
      0x34000000U,  // FLT_EPSILON
  };
  constexpr const size_t specials_count =
      sizeof(specials) / sizeof(specials[0]);

  const NetSpec spec = every_kind_net();
  const std::string cfg = workspace.write_cfg(spec);
  const std::string path = workspace.file("specials.dxxwjz1");
  const std::string weights = workspace.file("specials.weights");
  const Layout layout = spec.layout();

  {
    DarknetNetwork darknet{cfg, ParseMode::Train};

    for (size_t block = 0U; block < layout.size(); ++block) {
      float* values = darknet.array(layout[block]);

      ASSERT_NE(values, nullptr) << describe(layout[block]);

      for (uint64_t index = 0U; index < layout[block].count; ++index) {
        std::memcpy(&values[index], &specials[(block + index) % specials_count],
                    sizeof(float));
      }
    }

    save(darknet.net(), path);
    ASSERT_TRUE(OrigWeightsWriter{}.save(darknet.net(), weights));
  }

  // the JSON of the non finite floats is the strings of their bit patterns
  const std::string text = text_of(path);

  EXPECT_NE(text.find("\"0xffc12345\""), std::string::npos);
  EXPECT_NE(text.find("\"0x7fa00001\""), std::string::npos);
  EXPECT_NE(text.find("-0.0"), std::string::npos);
  EXPECT_EQ(text.find("nan"), std::string::npos);
  EXPECT_EQ(text.find("inf"), std::string::npos);

  DarknetNetwork darknet{cfg, ParseMode::Inference};

  ASSERT_TRUE(darknet.fill(layout, Pattern{SEED}));

  load(darknet.net(), path);

  for (size_t block = 0U; block < layout.size(); ++block) {
    const float* values = darknet.array(layout[block]);

    for (uint64_t index = 0U; index < layout[block].count; ++index) {
      uint32_t actual = 0U;
      std::memcpy(&actual, &values[index], sizeof(actual));

      ASSERT_EQ(actual, specials[(block + index) % specials_count])
          << describe(layout[block]) << " at the index " << index;
    }
  }

  // and the conversion to the original weights file keeps them all
  WeightsDocument document;
  const std::string converted = workspace.file("specials.converted.weights");

  ASSERT_TRUE(Dxxwjz1Reader{}.read(path, &darknet.net(), document));
  ASSERT_TRUE(OrigWeightsWriter{}.write(document, converted));
  EXPECT_TRUE(files_equal(weights, converted));
}

TEST_F(CTEST_Dxxwjz1, dontload_layers_are_skipped_by_their_names)
{
  // the original loader reads the arrays of the layer after the dontload one
  // in place of its ones, the dxxwjz1 loader finds the arrays by the layers
  const NetSpec spec =
      net("dontload", 8, 8, 3,
          {conv(4, 3), conv(5, 3).with_dontload(), conv(6, 3)});
  const std::string cfg = workspace.write_cfg(spec);
  const Pattern pattern{SEED};
  const Pattern before{~SEED};
  const Layout memory = spec.layout();
  const std::string path = workspace.file("dontload.dxxwjz1");

  {
    DarknetNetwork darknet{cfg, ParseMode::Inference};

    ASSERT_TRUE(darknet.fill(memory, pattern));

    save(darknet.net(), path);
  }

  DarknetNetwork darknet{cfg, ParseMode::Inference};

  ASSERT_TRUE(darknet.fill(memory, before));

  load(darknet.net(), path);

  EXPECT_TRUE(holds_document(darknet, memory, before, spec.layout(loading()),
                             pattern, counts_of(memory)));

  // the layer after the dontload one got its own arrays
  std::vector<float> biases(6U);

  for (uint64_t index = 0U; index < biases.size(); ++index) {
    biases[index] =
        pattern.value(Block{2, SubLayer::None, ArrayKind::Biases, 6U}, index);
  }

  EXPECT_TRUE(same_bits(copy_of(darknet.net().layers[2].biases, 6), biases));
}

TEST_F(CTEST_Dxxwjz1, dontloadscales_layers_skip_the_batch_normalization_arrays)
{
  const NetSpec spec =
      net("dontloadscales", 6, 6, 2,
          {conv(4, 3).with_bn().with_dontloadscales(),
           connected(5).with_bn().with_dontloadscales(),
           batchnorm().with_dontloadscales(), conv(3, 1).with_bn()});
  const std::string cfg = workspace.write_cfg(spec);
  const Pattern pattern{SEED};
  const Pattern before{~SEED};
  const Layout memory = spec.layout();
  const std::string path = workspace.file("dontloadscales.dxxwjz1");

  ASSERT_TRUE(write_document(
      path, document_of(spec, WeightsHeader{}, memory, pattern)));

  DarknetNetwork darknet{cfg, ParseMode::Train};

  ASSERT_TRUE(darknet.fill(memory, before));

  load(darknet.net(), path);

  EXPECT_TRUE(holds_document(darknet, memory, before, spec.layout(loading()),
                             pattern, counts_of(memory)));
  EXPECT_EQ(darknet.net().layers[0].scales[0],
            before.value(Block{0, SubLayer::None, ArrayKind::Scales, 4U}, 0U));
}

TEST_F(CTEST_Dxxwjz1, header_versions_transpose_the_connected_weights)
{
  const NetSpec spec =
      net("transposed", 5, 4, 2,
          {conv(3, 3).with_bn(), connected(4).with_bn(), rnn(5, 3), gru(4),
           lstm(3).with_bn(), connected(2)});
  const std::string cfg = workspace.write_cfg(spec);
  const Pattern pattern{SEED};
  const Pattern before{~SEED};
  const Layout memory = spec.layout();

  for (const WeightsHeader& header : variants::headers()) {
    SCOPED_TRACE("the version " + header.describe());

    const std::string path = workspace.file("transposed.dxxwjz1");

    ASSERT_TRUE(
        write_document(path, document_of(spec, header, memory, pattern)));

    DarknetNetwork darknet{cfg, ParseMode::Train};

    ASSERT_TRUE(darknet.fill(memory, before));

    load(darknet.net(), path);

    EXPECT_EQ(*darknet.net().seen, header.seen);
    EXPECT_TRUE(holds_document(darknet, memory, before,
                               spec.layout(loading(header.transposed())),
                               pattern, counts_of(memory)));
  }
}

TEST_F(CTEST_Dxxwjz1, flipped_conv_weights_are_transposed_on_the_load)
{
  // 3 filters of the 2 channels 1x1 kernels: the document holds the weights
  // as the kernel x filters matrix
  const NetSpec spec =
      net("flipped", 2, 2, 2,
          {avgpool(), conv(3, 1).with_flipped().with_pad(false)});
  const std::string path = workspace.file("flipped.dxxwjz1");

  ASSERT_TRUE(write_text(
      path,
      R"({"format": "dxxwjz1", "version": 1, "header": {"major": 0, "minor": 2, "revision": 5, "seen": 0}, "layers": [)"
      R"({"index": 0, "type": "avgpool"}, {"index": 1, "type": "convolutional", "arrays": )"
      R"({"biases": [10, 11, 12], "weights": [0, 1, 2, 3, 4, 5]}}]})"));

  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Inference};
  const layer& l = darknet.net().layers[1];

  load(darknet.net(), path);

  EXPECT_TRUE(same_bits(copy_of(l.biases, 3), {10.f, 11.f, 12.f}));
  EXPECT_TRUE(same_bits(copy_of(l.weights, 6), {0.f, 3.f, 1.f, 4.f, 2.f, 5.f}));
}

TEST_F(CTEST_Dxxwjz1, documents_of_another_network_leave_the_network_intact)
{
  const NetSpec spec = net("target", 6, 6, 2,
                           {conv(4, 3).with_bn(), maxpool(2, 2), connected(3)});
  const std::string cfg = workspace.write_cfg(spec);
  const Layout memory = spec.layout();
  const Pattern before{~SEED};
  const std::string header =
      R"({"format": "dxxwjz1", "version": 1, "header": {"major": 0, "minor": 2, "revision": 5, "seen": 7}, "layers": [)";

  const std::vector<std::pair<std::string, std::string>> documents{
      // the layer of another type
      {R"({"index": 0, "type": "connected", "arrays": {"biases": [1]}}]})",
       "is the convolutional layer in the network"},
      // the layer beyond the network ones
      {R"({"index": 3, "type": "maxpool"}]})", "beyond the 3 layers"},
      // the array longer than the network one
      {R"({"index": 0, "type": "convolutional", "arrays": {"biases": [1, 2, 3, 4, 5]}}]})",
       "the network one takes 4"},
      // the array the network doesn't have
      {R"({"index": 2, "type": "connected", "arrays": {"scales": [1, 2, 3]}}]})",
       "the layer 2 scales isn't an array of the network"},
      // the array the layer type doesn't have
      {R"({"index": 1, "type": "maxpool", "arrays": {"weights": [1]}}]})",
       "keeps no weights"},
      // the layer given twice
      {R"({"index": 1, "type": "maxpool"}, {"index": 1, "type": "maxpool"}]})",
       "given twice"}};

  for (const auto& [layers, error] : documents) {
    SCOPED_TRACE(layers);

    const std::string path = workspace.file("other.dxxwjz1");

    ASSERT_TRUE(write_text(path, header + layers));

    DarknetNetwork darknet{cfg, ParseMode::Train};
    network& target = darknet.net();

    ASSERT_TRUE(darknet.fill(memory, before));

    *target.seen = 11U;

    EXPECT_FALSE(Dxxwjz1Reader{}.load(target, path));
    EXPECT_EQ(*target.seen, 11U);
    EXPECT_TRUE(darknet.holds(memory, before));

    const WeightsReport report = Dxxwjz1Validator::validate(path, &target);

    ASSERT_FALSE(report.ok());
    EXPECT_NE(report.first_error().find(error), std::string::npos)
        << report.first_error();
  }
}

TEST_F(CTEST_Dxxwjz1, corrupted_and_truncated_files_are_rejected)
{
  const NetSpec spec = every_kind_net();
  const std::string path = saved(spec, "corrupted.dxxwjz1");
  const std::vector<char> whole = bytes_of(path);
  const std::string cfg = workspace.write_cfg(spec);
  const Layout memory = spec.layout();
  const Pattern before{~SEED};

  std::vector<std::vector<char>> broken;

  // the cuts of the gzip header, of the deflate data and of the trailer
  for (const size_t cut : {size_t{0}, size_t{5}, whole.size() / 2U,
                           whole.size() - 8U, whole.size() - 1U}) {
    broken.emplace_back(whole.begin(),
                        whole.begin() + static_cast<std::ptrdiff_t>(cut));
  }

  // the flipped bytes of the deflate data and of the CRC
  for (const size_t position : {whole.size() / 3U, whole.size() - 7U}) {
    std::vector<char> corrupted = whole;
    corrupted[position] = static_cast<char>(corrupted[position] ^ 0x21);
    broken.push_back(corrupted);
  }

  // the data after the gzip stream
  broken.push_back(whole);
  broken.back().push_back('x');

  for (size_t index = 0U; index < broken.size(); ++index) {
    SCOPED_TRACE("the broken file " + std::to_string(index));

    write_bytes(path, broken[index]);

    DarknetNetwork darknet{cfg, ParseMode::Train};

    ASSERT_TRUE(darknet.fill(memory, before));

    EXPECT_FALSE(Dxxwjz1Reader{}.load(darknet.net(), path));
    EXPECT_TRUE(darknet.holds(memory, before));
    EXPECT_FALSE(Dxxwjz1Validator::validate(path, nullptr).ok());
  }
}

TEST_F(CTEST_Dxxwjz1, plain_json_files_load_with_the_warning)
{
  const NetSpec spec = every_kind_net();
  const std::string compressed = saved(spec, "compressed.dxxwjz1");
  const std::string plain = workspace.file("plain.json");

  ASSERT_TRUE(write_text(plain, text_of(compressed), false));
  EXPECT_FALSE(zlibi::ZlibController::is_gzip_file(plain));

  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Train};

  ASSERT_TRUE(darknet.fill(spec.layout(), Pattern{~SEED}));

  load(darknet.net(), plain);

  EXPECT_TRUE(darknet.holds(spec.layout(), Pattern{SEED}));

  const WeightsReport report = Dxxwjz1Validator::validate(plain, nullptr);

  EXPECT_TRUE(report.ok());
  ASSERT_EQ(report.warnings().size(), 1U);
  EXPECT_NE(report.warnings()[0].find("isn't gzip compressed"),
            std::string::npos);
}

TEST_F(CTEST_Dxxwjz1, validator_warns_about_the_partial_documents)
{
  const NetSpec spec = net("partial", 6, 6, 2,
                           {conv(4, 3).with_bn(), maxpool(2, 2), connected(3)});
  const std::string cfg = workspace.write_cfg(spec);
  const Layout memory = spec.layout();
  const std::string path = workspace.file("partial.dxxwjz1");
  const uint64_t floats = count_floats(memory);

  DarknetNetwork darknet{cfg, ParseMode::Train};

  // the whole document
  ASSERT_TRUE(write_document(
      path, document_of(spec, WeightsHeader{}, memory, Pattern{SEED})));

  WeightsReport report = Dxxwjz1Validator::validate(path, &darknet.net());

  EXPECT_TRUE(report.ok());
  EXPECT_TRUE(report.warnings().empty());

  // the document of the part of the first array: the truncated array and
  // the arrays the document doesn't have
  ASSERT_TRUE(write_document(
      path, document_of(spec, WeightsHeader{}, memory, Pattern{SEED}, 2U)));

  report = Dxxwjz1Validator::validate(path, &darknet.net());

  EXPECT_TRUE(report.ok());
  ASSERT_EQ(report.warnings().size(), 2U);
  EXPECT_NE(report.warnings()[0].find("holds 2 of the 4 values"),
            std::string::npos)
      << report.warnings()[0];
  EXPECT_NE(report.warnings()[1].find("aren't in the document"),
            std::string::npos)
      << report.warnings()[1];

  // the document of the network alone is complete and the document alone
  // has nothing to compare with
  report = Dxxwjz1Validator::validate(path, nullptr);

  EXPECT_TRUE(report.ok());
  EXPECT_TRUE(report.warnings().empty());

  EXPECT_GT(floats, 2U);
}

TEST_F(CTEST_Dxxwjz1, header_of_the_32_bits_seen_keeps_the_32_bits_seen)
{
  const NetSpec spec = net("seen32", 4, 4, 1, {conv(2, 3)});
  const std::string cfg = workspace.write_cfg(spec);
  const std::string path = workspace.file("seen32.dxxwjz1");
  const std::string weights = workspace.file("seen32.weights");

  DarknetNetwork darknet{cfg, ParseMode::Train};

  // the seen of the 0.1 version beyond the 32 bits
  ASSERT_TRUE(write_document(
      path, document_of(spec, WeightsHeader{0, 1, 0, 0x100000000ULL},
                        spec.layout(), Pattern{SEED})));

  const WeightsReport report = Dxxwjz1Validator::validate(path, nullptr);

  ASSERT_FALSE(report.ok());
  EXPECT_NE(report.first_error().find("32 bits seen"), std::string::npos)
      << report.first_error();
  EXPECT_FALSE(Dxxwjz1Reader{}.load(darknet.net(), path));

  // the one of the 32 bits converts to the 16 bytes header
  ASSERT_TRUE(write_document(
      path, document_of(spec, WeightsHeader{0, 1, 0, 0xFFFFFFFFULL},
                        spec.layout(), Pattern{SEED})));

  WeightsDocument document;

  ASSERT_TRUE(Dxxwjz1Reader{}.read(path, &darknet.net(), document));
  ASSERT_TRUE(OrigWeightsWriter{}.write(document, weights));
  EXPECT_TRUE(file_matches(weights, WeightsHeader{0, 1, 0, 0xFFFFFFFFULL},
                           spec.layout(), Pattern{SEED}));
}

TEST_F(CTEST_Dxxwjz1, trailing_bytes_of_the_weights_file_are_kept)
{
  const NetSpec spec = every_kind_net();
  const std::string cfg = workspace.write_cfg(spec);
  const Layout layout = spec.layout();
  const Pattern pattern{SEED};
  const WeightsHeader header{0, 2, 5, 640U};
  const uint64_t full = header.size() + count_floats(layout) * sizeof(float);

  DarknetNetwork darknet{cfg, ParseMode::Train};

  // the extra bytes after the arrays and the parts of the floats of the
  // truncated files
  for (const auto& [name, size] : std::vector<std::pair<std::string, uint64_t>>{
           {"extra", full + 13U},
           {"part", full - 3U},
           {"middle", header.size() + (full - header.size()) / 2U + 1U},
           {"header", header.size()}}) {
    SCOPED_TRACE(name);

    const std::string weights = workspace.file(name + ".weights");
    const std::string path = weights + ".dxxwjz1";
    const std::string back = weights + ".back.weights";

    ASSERT_TRUE(write_weights_file(weights, header, layout, pattern));

    if (size > full) {
      ASSERT_TRUE(append_bytes(weights, size - full));
    } else {
      ASSERT_TRUE(truncate_file(weights, size));
    }

    WeightsDocument document;

    ASSERT_TRUE(OrigWeightsReader{}.read(weights, &darknet.net(), document));
    EXPECT_EQ(
        document.trailing.size(),
        size > full ? size - full : (size - header.size()) % sizeof(float));
    EXPECT_EQ(document.file_size(), size);

    ASSERT_TRUE(Dxxwjz1Writer{}.write(document, path));

    if (!document.trailing.empty()) {
      EXPECT_NE(text_of(path).find("\"trailing\": \""), std::string::npos);
    }

    WeightsDocument converted;

    ASSERT_TRUE(Dxxwjz1Reader{}.read(path, &darknet.net(), converted));
    ASSERT_TRUE(OrigWeightsWriter{}.write(converted, back));
    EXPECT_TRUE(files_equal(weights, back));
  }
}

TEST_F(CTEST_Dxxwjz1, reference_weights_files_convert_back_and_forth)
{
  for (const auto& [cfg, weights] :
       std::vector<std::pair<std::string, std::string>>{
           {CTEST_Dxxwjz1_provider::CFG_1, CTEST_Dxxwjz1_provider::W_1},
           {CTEST_Dxxwjz1_provider::CFG_2, CTEST_Dxxwjz1_provider::W_2}}) {
    SCOPED_TRACE(weights);

    const std::string path = workspace.file("reference.dxxwjz1");
    const std::string back = workspace.file("reference.weights");

    DarknetNetwork darknet{cfg, ParseMode::Inference};
    WeightsDocument document;

    ASSERT_TRUE(OrigWeightsReader{}.read(weights, &darknet.net(), document));
    EXPECT_TRUE(document.trailing.empty());
    ASSERT_TRUE(Dxxwjz1Writer{}.write(document, path));

    WeightsDocument converted;

    ASSERT_TRUE(Dxxwjz1Reader{}.read(path, &darknet.net(), converted));
    ASSERT_TRUE(OrigWeightsWriter{}.write(converted, back));
    EXPECT_TRUE(files_equal(weights, back));

    // the dxxwjz1 file loads the network the weights file does
    DarknetNetwork from_weights{cfg, ParseMode::Inference};
    DarknetNetwork from_dxxwjz1{cfg, ParseMode::Inference};

    ASSERT_TRUE(OrigWeightsReader{}.load(from_weights.net(), weights));
    ASSERT_TRUE(Dxxwjz1Reader{}.load(from_dxxwjz1.net(), path));

    const std::string first = workspace.file("from_weights.weights");
    const std::string second = workspace.file("from_dxxwjz1.weights");

    ASSERT_TRUE(OrigWeightsWriter{}.save(from_weights.net(), first));
    ASSERT_TRUE(OrigWeightsWriter{}.save(from_dxxwjz1.net(), second));
    EXPECT_TRUE(files_equal(first, second));
  }
}

TEST_F(CTEST_Dxxwjz1, compression_levels_make_the_same_document)
{
  const NetSpec spec = every_kind_net();
  const std::string cfg = workspace.write_cfg(spec);

  DarknetNetwork darknet{cfg, ParseMode::Train};

  ASSERT_TRUE(darknet.fill(spec.layout(), Pattern{SEED}));

  std::vector<uint64_t> sizes;
  std::string reference;

  for (const int level : {0, zlibi::ZlibController::BEST_SPEED,
                          zlibi::ZlibController::DEFAULT_LEVEL,
                          zlibi::ZlibController::BEST_COMPRESSION}) {
    SCOPED_TRACE("the level " + std::to_string(level));

    const std::string path =
        workspace.file("level" + std::to_string(level) + ".dxxwjz1");

    ASSERT_TRUE(Dxxwjz1Writer{level}.save(darknet.net(), path));

    sizes.push_back(file_size(path));

    const std::string text = text_of(path);

    if (reference.empty()) {
      reference = text;
    } else {
      EXPECT_EQ(text, reference);
    }
  }

  // the stored text takes the most, the best compression the least
  EXPECT_GT(sizes.front(), sizes[1]);
  EXPECT_LE(sizes.back(), sizes[1]);
}

TEST_F(CTEST_Dxxwjz1, missing_files_and_unwritable_paths_fail)
{
  const NetSpec spec = net("missing", 4, 4, 1, {conv(2, 3)});
  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Inference};
  const std::string missing = workspace.file("missing.dxxwjz1");
  const std::string unwritable = workspace.file("no/such/dir/x.dxxwjz1");

  EXPECT_FALSE(Dxxwjz1Reader{}.load(darknet.net(), missing));
  EXPECT_FALSE(Dxxwjz1Reader{}.load(darknet.net(), missing, 1));
  EXPECT_FALSE(Dxxwjz1Validator::validate(missing, nullptr).ok());
  EXPECT_FALSE(Dxxwjz1Writer{}.save(darknet.net(), unwritable));
  EXPECT_FALSE(Dxxwjz1Writer{}.save(darknet.net(), unwritable, 1, true));
  EXPECT_FALSE(Dxxwjz1Writer{}.write(WeightsDocument{}, unwritable));

  // the document of the errors isn't written
  WeightsDocument repeated;
  repeated.layers.resize(2U);

  EXPECT_FALSE(Dxxwjz1Writer{}.write(repeated, missing));
  EXPECT_FALSE(std::filesystem::exists(missing));
}

TEST_F(CTEST_Dxxwjz1, save_weights_upto_writes_the_layers_before_the_cutoff)
{
  const NetSpec spec =
      net("upto", 2, 2, 1, {conv(2, 1), connected(3), batchnorm()});
  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Train};

  ASSERT_TRUE(darknet.fill(spec.layout(), Pattern{SEED}));

  for (const int cutoff : {INT_MIN, -5, 0, 1, 2, 3, 100, INT_MAX}) {
    SCOPED_TRACE("the cutoff " + std::to_string(cutoff));

    const std::string path = workspace.file("upto.dxxwjz1");

    save_upto(darknet.net(), path, cutoff, false);

    fjson document;
    LayoutOptions options;
    options.cutoff = cutoff;

    ASSERT_TRUE(read_document(path, document));
    EXPECT_TRUE(document_matches(document, spec, saved_header(0U),
                                 spec.layout(options), Pattern{SEED}, cutoff));
  }
}
