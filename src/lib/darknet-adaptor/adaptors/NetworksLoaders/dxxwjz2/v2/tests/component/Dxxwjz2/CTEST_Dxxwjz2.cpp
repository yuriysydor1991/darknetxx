#include <gtest/gtest.h>
#include <zlib.h>

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <string>
#include <vector>

#include "CTEST_Dxxwjz2_provider.h"
#include "DarknetNetwork.h"
#include "WeightsFile.h"
#include "WeightsNetSpec.h"
#include "WeightsWorkspace.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Reader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Validator.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Writer.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkFiles.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/log/default-logger/DefaultLogger.h"

using namespace testing;
using namespace weights_factory;

using darknet_adaptor::loaders::NetworkDescription;
using darknet_adaptor::loaders::NetworkFiles;
using darknet_adaptor::loaders::OrigWeightsReader;
using darknet_adaptor::loaders::OrigWeightsWriter;
using darknet_adaptor::loaders::TrainingRecord;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsReport;
using darknet_adaptor::loaders::dxxwjz2::v2::Dxxwjz2Reader;
using darknet_adaptor::loaders::dxxwjz2::v2::Dxxwjz2Validator;
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

constexpr const uint64_t SEED = 0xD2C3A5E2U;

/// @brief The small network of the weights layer kinds of the sub-layers.
NetSpec every_kind_net()
{
  NetSpec spec;
  spec.name = "every_kind";
  spec.width = 16;
  spec.height = 12;
  spec.layers = {conv(6, 3).with_bn(),
                 batchnorm(),
                 maxpool(2, 2),
                 crnn(4, 3, 3).with_bn(),
                 rnn(6, 5).with_bn(),
                 connected(7).with_bn(),
                 softmax()};
  return spec;
}

/// @brief The training record of the losses and the mAPs of every kind of
/// the float.
TrainingRecord record_of(const int iterations)
{
  TrainingRecord record;

  for (int iteration = 1; iteration <= iterations; ++iteration) {
    record.record(iteration, 10.f / static_cast<float>(iteration),
                  9.f / static_cast<float>(iteration));
  }

  record.record(iterations + 1, std::numeric_limits<float>::quiet_NaN(),
                std::numeric_limits<float>::infinity());
  record.record_map(2, 0.125f);
  record.record_map(iterations + 1, std::numeric_limits<float>::quiet_NaN());
  record.seconds(3725.4);

  return record;
}

WeightsDocument rest_of(const std::string& cfg)
{
  WeightsDocument rest;
  rest.network = NetworkDescription{cfg, {"car", "pedestrian", "light"}};
  rest.training = record_of(5);
  return rest;
}

std::vector<uint32_t> bits_of(const std::vector<float>& values)
{
  std::vector<uint32_t> result;

  for (const float value : values) {
    result.push_back(bits(value));
  }

  return result;
}

/// @brief The text of the gzip compressed file.
std::string gunzipped(const std::string& path)
{
  std::string text;
  gzFile file = gzopen(path.c_str(), "rb");
  char chunk[4096];
  int count = 0;

  while (file != nullptr && (count = gzread(file, chunk, sizeof(chunk))) > 0) {
    text.append(chunk, static_cast<size_t>(count));
  }

  if (file != nullptr) {
    gzclose(file);
  }

  return text;
}

::testing::AssertionResult same_records(const TrainingRecord& actual,
                                        const TrainingRecord& expected)
{
  if (actual.iterations() != expected.iterations() ||
      bits_of(actual.losses()) != bits_of(expected.losses()) ||
      bits_of(actual.avg_losses()) != bits_of(expected.avg_losses()) ||
      actual.map_iterations() != expected.map_iterations() ||
      bits_of(actual.maps()) != bits_of(expected.maps())) {
    return ::testing::AssertionFailure() << "the losses or the mAPs differ";
  }

  // the files keep the whole seconds
  if (actual.seconds() != std::round(expected.seconds())) {
    return ::testing::AssertionFailure()
           << actual.seconds() << " seconds instead of " << expected.seconds();
  }

  return ::testing::AssertionSuccess();
}

::testing::AssertionResult same_layers(const WeightsDocument& actual,
                                       const WeightsDocument& expected)
{
  if (actual.header != expected.header ||
      actual.trailing != expected.trailing ||
      actual.layers.size() != expected.layers.size()) {
    return ::testing::AssertionFailure() << "the header, the trailing bytes or "
                                            "the layers count differ";
  }

  for (size_t index = 0U; index < actual.layers.size(); ++index) {
    const auto& a = actual.layers[index];
    const auto& b = expected.layers[index];

    if (a.index != b.index || a.type != b.type ||
        a.arrays.size() != b.arrays.size()) {
      return ::testing::AssertionFailure()
             << "the layer " << index << " differs";
    }

    for (size_t array = 0U; array < a.arrays.size(); ++array) {
      if (a.arrays[array].sub != b.arrays[array].sub ||
          a.arrays[array].kind != b.arrays[array].kind ||
          bits_of(a.arrays[array].values) != bits_of(b.arrays[array].values)) {
        return ::testing::AssertionFailure()
               << "the array " << array << " of the layer " << index
               << " differs";
      }
    }
  }

  return ::testing::AssertionSuccess();
}

}  // namespace

class CTEST_Dxxwjz2 : public Test
{
 public:
  WeightsWorkspace workspace{CTEST_Dxxwjz2_provider::GENERATED_DIR};
};

TEST_F(CTEST_Dxxwjz2, saved_network_loads_with_its_description_and_record)
{
  const NetSpec spec = every_kind_net();
  const std::string cfg = workspace.write_cfg(spec);
  const Layout memory = spec.layout();
  const std::string path = workspace.file("every_kind.dxxwjz2");
  const WeightsDocument rest = rest_of(spec.cfg());
  uint64_t seen = 0U;

  {
    DarknetNetwork saved{cfg, ParseMode::Train};

    ASSERT_TRUE(saved.fill(memory, Pattern{SEED}));
    *saved.net().cur_iteration = 7;

    ASSERT_TRUE(
        Dxxwjz2Writer{}.save(saved.net(), path, saved.net().n, false, rest));

    seen = *saved.net().seen;
  }

  DarknetNetwork loaded{cfg, ParseMode::Train};
  WeightsDocument read;

  ASSERT_TRUE(loaded.fill(memory, Pattern{~SEED}));
  ASSERT_TRUE(Dxxwjz2Reader{}.load(loaded.net(), path, loaded.net().n, read));

  EXPECT_TRUE(loaded.holds(memory, Pattern{SEED}));
  EXPECT_EQ(*loaded.net().seen, seen);
  EXPECT_EQ(read.network.cfg(), spec.cfg());
  EXPECT_EQ(read.network.classes(), rest.network.classes());
  EXPECT_TRUE(same_records(read.training, rest.training));
  EXPECT_TRUE(read.layers.empty());

  // the load of the weights alone of the same file
  DarknetNetwork weights_only{cfg, ParseMode::Train};

  ASSERT_TRUE(Dxxwjz2Reader{}.load(weights_only.net(), path));
  EXPECT_TRUE(weights_only.holds(memory, Pattern{SEED}));
}

TEST_F(CTEST_Dxxwjz2, original_weights_convert_into_the_same_bytes)
{
  for (const auto& [cfg, weights] :
       {std::pair{CTEST_Dxxwjz2_provider::CFG_1, CTEST_Dxxwjz2_provider::W_1},
        std::pair{CTEST_Dxxwjz2_provider::CFG_2,
                  CTEST_Dxxwjz2_provider::W_2}}) {
    SCOPED_TRACE(weights);

    DarknetNetwork darknet{cfg, ParseMode::Inference};
    WeightsDocument document;
    std::string text;

    ASSERT_TRUE(OrigWeightsReader{}.read(weights, &darknet.net(), document));
    ASSERT_TRUE(NetworkFiles::read_text(cfg, text));

    document.network = NetworkDescription{text, {"a", "b"}};
    document.training = record_of(3);

    const std::string dxxwjz2 = workspace.file("reference.dxxwjz2");
    const std::string back = workspace.file("reference.weights");
    WeightsDocument read;

    ASSERT_TRUE(Dxxwjz2Writer{}.write(document, dxxwjz2));
    ASSERT_TRUE(Dxxwjz2Reader{}.read(dxxwjz2, &darknet.net(), read));

    EXPECT_TRUE(same_layers(read, document));
    EXPECT_EQ(read.network.cfg(), text);
    EXPECT_EQ(read.network.classes(), document.network.classes());
    EXPECT_TRUE(same_records(read.training, document.training));

    ASSERT_TRUE(OrigWeightsWriter{}.write(read, back));
    EXPECT_TRUE(files_equal(weights, back));
  }
}

TEST_F(CTEST_Dxxwjz2, file_is_the_gzip_compressed_json_document)
{
  const std::string path = workspace.file("text.dxxwjz2");
  WeightsDocument document = rest_of("[net]\nwidth=8\n");

  // the file of no layers keeps no header either
  document.layers.push_back({0, MAXPOOL, {}});

  ASSERT_TRUE(Dxxwjz2Writer{}.write(document, path));

  const std::string text = gunzipped(path);
  const auto at = [&text](const std::string& part) { return text.find(part); };

  EXPECT_EQ(at("{\n  \"format\": \"dxxwjz2\",\n  \"version\": 2,"), 0U)
      << text.substr(0U, 200U);
  EXPECT_NE(at("\"network\": {\"cfg\": \"[net]\\nwidth=8\\n\", \"classes\": "
               "[\n    {\"index\": 0, \"name\": \"car\"},"),
            std::string::npos)
      << text;
  EXPECT_NE(at("\"training\": {\"seconds\": 3725,\n    \"iteration\": "
               "[1,2,3,4,5,6],"),
            std::string::npos)
      << text;

  // the network, the training and the weights one after another
  EXPECT_LT(at("\"network\""), at("\"training\""));
  EXPECT_LT(at("\"training\""), at("\"header\""));
  EXPECT_LT(at("\"header\""), at("\"layers\""));
}

TEST_F(CTEST_Dxxwjz2, texts_keep_every_character)
{
  const std::string path = workspace.file("characters.dxxwjz2");
  const std::string cfg =
      "[net]\n# \"quoted\" back\\slash\ttab\r\n# \xC3\xA9 \xE6\x97\xA5 "
      "\xF0\x9F\x98\x80 \x01\n";
  WeightsDocument document;
  WeightsDocument read;

  document.network = NetworkDescription{cfg, {"\"a\" \\ b", "\xE2\x82\xAC"}};

  ASSERT_TRUE(Dxxwjz2Writer{}.write(document, path));
  ASSERT_TRUE(Dxxwjz2Reader{}.read(path, nullptr, read));

  EXPECT_EQ(read.network.cfg(), cfg);
  EXPECT_EQ(read.network.classes(), document.network.classes());
}

TEST_F(CTEST_Dxxwjz2, invalid_utf8_texts_are_replaced)
{
  const std::string path = workspace.file("latin1.dxxwjz2");
  WeightsDocument document;
  WeightsDocument read;

  // the Latin-1 comment and the cut sequences of the cfg file
  document.network =
      NetworkDescription{"[net]\n# caf\xE9 \xC3\n\xED\xA0\x80", {"\xFF"}};

  ASSERT_TRUE(Dxxwjz2Writer{}.write(document, path));
  ASSERT_TRUE(Dxxwjz2Reader{}.read(path, nullptr, read));

  EXPECT_EQ(read.network.cfg(),
            "[net]\n# caf\xEF\xBF\xBD \xEF\xBF\xBD\n\xEF\xBF\xBD\xEF\xBF\xBD"
            "\xEF\xBF\xBD");
  EXPECT_EQ(read.network.classes(), (std::vector<std::string>{"\xEF\xBF\xBD"}));
}

TEST_F(CTEST_Dxxwjz2, file_needs_the_network_description)
{
  const NetSpec spec = every_kind_net();
  const std::string cfg = workspace.write_cfg(spec);
  const std::string path = workspace.file("undescribed.dxxwjz2");
  DarknetNetwork darknet{cfg, ParseMode::Inference};

  EXPECT_FALSE(Dxxwjz2Writer{}.save(darknet.net(), path));
  EXPECT_FALSE(Dxxwjz2Writer{}.write(WeightsDocument{}, path));
  EXPECT_FALSE(std::filesystem::exists(path));

  // the document of no cfg text
  WeightsDocument document;
  document.network = NetworkDescription{"", {"a"}};

  EXPECT_FALSE(Dxxwjz2Validator::validate(document, nullptr).ok());
}

TEST_F(CTEST_Dxxwjz2, network_to_train_keeps_the_network_values)
{
  const NetSpec spec = every_kind_net();
  const std::string cfg = workspace.write_cfg(spec);
  const std::string path = workspace.file("to_train.dxxwjz2");
  const Layout memory = spec.layout();
  WeightsDocument document;

  document.network = NetworkDescription{spec.cfg(), {"a"}};

  ASSERT_TRUE(Dxxwjz2Writer{}.write(document, path));

  // the file of the network alone keeps no header
  EXPECT_EQ(gunzipped(path).find("\"header\""), std::string::npos);

  DarknetNetwork darknet{cfg, ParseMode::Train};
  WeightsDocument rest;
  bool weights = true;

  ASSERT_TRUE(darknet.fill(memory, Pattern{SEED}));
  ASSERT_TRUE(Dxxwjz2Reader{}.peek(path, rest, weights));
  EXPECT_FALSE(weights);

  // no warning of every network array the file has none of
  const WeightsReport report = Dxxwjz2Validator::validate(path, &darknet.net());

  EXPECT_TRUE(report.ok());
  EXPECT_TRUE(report.warnings().empty());

  ASSERT_TRUE(Dxxwjz2Reader{}.load(darknet.net(), path, darknet.net().n, rest));

  EXPECT_TRUE(darknet.holds(memory, Pattern{SEED}));
  EXPECT_EQ(*darknet.net().seen, 0U);
  EXPECT_EQ(rest.network.classes(), (std::vector<std::string>{"a"}));

  // the seen and the record of the network alone are of no training
  const std::string headed = workspace.file("headed.dxxwjz2");
  gzFile file = gzopen(headed.c_str(), "wb");

  ASSERT_NE(file, nullptr);
  gzputs(file,
         R"({"format": "dxxwjz2", "version": 2, "network": {"cfg": "[net]"}, )"
         R"("training": {"seconds": 5, "iteration": [1], "loss": [1], )"
         R"("avg_loss": [1]}, "header": {"major": 0, "minor": 2, )"
         R"("revision": 5, "seen": 6400}})");
  gzclose(file);

  ASSERT_TRUE(
      Dxxwjz2Reader{}.load(darknet.net(), headed, darknet.net().n, rest));

  EXPECT_EQ(*darknet.net().seen, 0U);
  EXPECT_EQ(rest.training.size(), 0U);
}

TEST_F(CTEST_Dxxwjz2, class_names_of_another_count_are_the_warning)
{
  DarknetNetwork darknet{CTEST_Dxxwjz2_provider::CFG_1, ParseMode::Inference};
  WeightsDocument document;
  std::string text;

  ASSERT_TRUE(OrigWeightsReader{}.read(CTEST_Dxxwjz2_provider::W_1,
                                       &darknet.net(), document));
  ASSERT_TRUE(NetworkFiles::read_text(CTEST_Dxxwjz2_provider::CFG_1, text));

  document.network = NetworkDescription{text, {"one", "two", "three", "four"}};

  const WeightsReport report =
      Dxxwjz2Validator::validate(document, &darknet.net());

  EXPECT_TRUE(report.ok());
  ASSERT_EQ(report.warnings().size(), 1U);
  EXPECT_NE(report.warnings().front().find("the 4 class names"),
            std::string::npos)
      << report.warnings().front();
}
