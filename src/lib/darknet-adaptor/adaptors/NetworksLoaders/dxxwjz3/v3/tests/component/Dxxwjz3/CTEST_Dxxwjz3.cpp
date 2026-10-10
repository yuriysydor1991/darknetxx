#include <gtest/gtest.h>
#include <zlib.h>

#include <cmath>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "CTEST_Dxxwjz3_provider.h"
#include "DarknetNetwork.h"
#include "WeightsFile.h"
#include "WeightsNetSpec.h"
#include "WeightsWorkspace.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3CfgJson.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Parser.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Reader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3ValueCodec.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Writer.h"
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
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsReport;
using darknet_adaptor::loaders::dxxwjz3::v3::Dxxwjz3CfgJson;
using darknet_adaptor::loaders::dxxwjz3::v3::Dxxwjz3Parser;
using darknet_adaptor::loaders::dxxwjz3::v3::Dxxwjz3Reader;
using darknet_adaptor::loaders::dxxwjz3::v3::Dxxwjz3ValueCodec;
using darknet_adaptor::loaders::dxxwjz3::v3::Dxxwjz3Writer;
using json = nlohmann::ordered_json;

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

constexpr const uint64_t SEED = 0xD3C3A5E3U;

/// @brief The cfg of the darknet notations: the white space, the comments,
/// the [network] section, the repeated key, the option of no '='.
const std::string CFG =
    "[network]\n# the comment\nbatch = 64\nhue=.1\ndecay=0.0005\n"
    "learning_rate=1e-3\nsteps=400,450\nscales=.1,.1\nrandom\nbatch=32\n"
    "policy=steps\n\n[convolutional]\nfilters=8\nactivation=leaky\n\n"
    "[maxpool]\n\n[yolo]\nmask = 0,1,2\nanchors = 10,13,  16,30\n";

/// @brief The cfg text the JSON of the CFG reads into.
const std::string READ_CFG =
    "[net]\nbatch=64\nhue=0.1\ndecay=0.0005\nlearning_rate=1e-3\n"
    "steps=400,450\nscales=0.1,0.1\nrandom\npolicy=steps\n\n"
    "[convolutional]\nfilters=8\nactivation=leaky\n\n[maxpool]\n\n"
    "[yolo]\nmask=0,1,2\nanchors=10,13,16,30\n";

const std::string WEIGHTS_MEMBERS =
    R"("header": {"major": 0, "minor": 2, "revision": 5, "seen": 192}, "layers": [{"index": 0, "type": "maxpool"}])";

NetSpec small_net()
{
  NetSpec spec;
  spec.name = "small";
  spec.layers = {conv(6, 3).with_bn(), maxpool(2, 2), connected(7).with_bn(),
                 softmax()};
  return spec;
}

/// @brief The document of the network member and the rest.
std::string document_text(const std::string& network,
                          const std::string& rest = "")
{
  return R"({"format": "dxxwjz3", "version": 3, "network": )" + network + rest +
         "}";
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

/// @brief Parses the text expected to fail with the error of the words.
::testing::AssertionResult rejects(const std::string& text,
                                   const std::vector<std::string>& words)
{
  std::istringstream stream{text};
  WeightsDocument result;
  WeightsReport report;

  if (Dxxwjz3Parser::parse(stream, result, report)) {
    return ::testing::AssertionFailure() << "the text is accepted: " << text;
  }

  for (const std::string& word : words) {
    if (report.first_error().find(word) == std::string::npos) {
      return ::testing::AssertionFailure()
             << "the error \"" << report.first_error() << "\" has no \"" << word
             << "\"";
    }
  }

  return ::testing::AssertionSuccess();
}

}  // namespace

class CTEST_Dxxwjz3 : public Test
{
 public:
  WeightsWorkspace workspace{CTEST_Dxxwjz3_provider::GENERATED_DIR};
};

TEST_F(CTEST_Dxxwjz3, options_are_the_json_values_of_the_darknet_ones)
{
  const std::vector<std::pair<const char*, std::string>> values{
      {"64", "64"},
      {"-1", "-1"},
      {"+5", "5"},
      {"007", "7"},
      {".1", "0.1"},
      {"1.0", "1"},
      {"-.5", "-0.5"},
      {"0.0005", "0.0005"},
      {"0.949", "0.949"},
      {"1e-3", R"("1e-3")"},
      {"leaky", R"("leaky")"},
      {"nan", R"("nan")"},
      {"0x10", R"("0x10")"},
      {"data/train.txt", R"("data/train.txt")"},
      {"0,1,2", "[0,1,2]"},
      {".1,.1", "[0.1,0.1]"},
      {"1,,2", R"([1,"",2])"},
      {"a,", R"(["a",""])"},
      {nullptr, "null"}};

  for (const auto& [value, text] : values) {
    EXPECT_EQ(Dxxwjz3ValueCodec::json_of(value), text)
        << (value == nullptr ? "NULL" : value);
  }

  // the cfg texts of the JSON values, the darknet reads the same way
  const std::vector<std::pair<std::string, std::string>> texts{
      {"64", "64"},
      {"0.1", "0.1"},
      {"5e-4", "0.0005"},
      {"1e2", "100"},
      {R"("1e-3")", "1e-3"},
      {"[0.1,0.1]", "0.1,0.1"},
      {R"([1,"",2])", "1,,2"},
      {"[7]", "7"},
      {"18446744073709551615", "18446744073709551615"}};

  for (const auto& [value, expected] : texts) {
    std::string text;

    EXPECT_TRUE(Dxxwjz3ValueCodec::text_of(json::parse(value), text)) << value;
    EXPECT_EQ(text, expected);
  }

  for (const char* const wrong :
       {R"("")", "[]", R"([""])", R"("a b")", R"("a\nb")", "true", "{}",
        "[[1]]", "[null]", "null"}) {
    std::string text;

    EXPECT_FALSE(Dxxwjz3ValueCodec::text_of(json::parse(wrong), text)) << wrong;
  }
}

TEST_F(CTEST_Dxxwjz3, cfg_is_the_json_structure_of_the_network)
{
  const std::string path = workspace.file("structure.dxxwjz3");
  WeightsDocument document;
  WeightsDocument read;

  document.network = NetworkDescription{CFG, {"cat", "dog"}};

  ASSERT_TRUE(Dxxwjz3Writer{}.write(document, path));

  const std::string text = gunzipped(path);
  const json written = json::parse(text);

  EXPECT_EQ(written["format"], "dxxwjz3");
  EXPECT_EQ(written["version"], 3);
  EXPECT_FALSE(written["network"].contains("cfg"));

  // the options in the cfg order, the first one of the repeated keys
  EXPECT_EQ(
      written["network"]["net"],
      json::parse(
          R"({"batch": 64, "hue": 0.1, "decay": 0.0005, "learning_rate": "1e-3", "steps": [400, 450], "scales": [0.1, 0.1], "random": null, "policy": "steps"})"));
  EXPECT_EQ(
      written["network"]["layers"],
      json::parse(
          R"([{"type": "convolutional", "options": {"filters": 8, "activation": "leaky"}}, {"type": "maxpool"}, {"type": "yolo", "options": {"mask": [0, 1, 2], "anchors": [10, 13, 16, 30]}}])"));

  // a layer on a line
  EXPECT_NE(text.find("\"network\": {\"net\": {\"batch\": 64, \"hue\": 0.1, "),
            std::string::npos)
      << text;
  EXPECT_NE(text.find("\"layers\": [\n    {\"type\": \"convolutional\", "
                      "\"options\": {\"filters\": 8, \"activation\": "
                      "\"leaky\"}},\n    {\"type\": \"maxpool\"},\n"),
            std::string::npos)
      << text;

  ASSERT_TRUE(Dxxwjz3Reader{}.read(path, nullptr, read));

  EXPECT_EQ(read.network.cfg(), READ_CFG);
  EXPECT_EQ(read.network.classes(), document.network.classes());
}

TEST_F(CTEST_Dxxwjz3, losses_are_the_ones_of_the_iterations_of_their_indexes)
{
  const std::string path = workspace.file("training.dxxwjz3");
  WeightsDocument document;
  WeightsDocument read;

  document.network = NetworkDescription{CFG, {}};
  // the training resumed of the weights of no record of their iteration
  document.training.record(2, 9.5f, 9.f);
  document.training.record(3, 7.f, 8.f);
  document.training.record_map(3, 0.25f);

  ASSERT_TRUE(Dxxwjz3Writer{}.write(document, path));

  const json training = json::parse(gunzipped(path))["training"];

  EXPECT_FALSE(training.contains("iteration"));
  EXPECT_EQ(training["loss"], json::parse(R"(["0x7fc00000", 9.5, 7])"));
  EXPECT_EQ(training["map_iteration"], json::parse("[3]"));

  ASSERT_TRUE(Dxxwjz3Reader{}.read(path, nullptr, read));

  ASSERT_EQ(read.training.iterations(), (std::vector<int>{1, 2, 3}));
  EXPECT_TRUE(std::isnan(read.training.losses()[0]));
  EXPECT_EQ(read.training.avg_losses()[2], 8.f);
  EXPECT_EQ(read.training.map_iterations(), (std::vector<int>{3}));
}

TEST_F(CTEST_Dxxwjz3, saved_network_loads_of_the_cfg_it_keeps)
{
  const NetSpec spec = small_net();
  const std::string cfg = workspace.write_cfg(spec);
  const std::string again = workspace.file("again.cfg");
  const std::string path = workspace.file("small.dxxwjz3");
  const Layout memory = spec.layout();
  WeightsDocument rest;
  WeightsDocument read;
  bool weights = false;

  rest.network = NetworkDescription{spec.cfg(), {"car"}};
  rest.training.record(1, 2.f, 2.f);

  {
    DarknetNetwork saved{cfg, ParseMode::Train};

    ASSERT_TRUE(saved.fill(memory, Pattern{SEED}));
    ASSERT_TRUE(
        Dxxwjz3Writer{}.save(saved.net(), path, saved.net().n, false, rest));
  }

  ASSERT_TRUE(Dxxwjz3Reader{}.peek(path, read, weights));
  EXPECT_TRUE(weights);

  // the JSON of the cfg text read is the one of the cfg
  EXPECT_EQ(Dxxwjz3CfgJson::members(read.network.cfg()),
            Dxxwjz3CfgJson::members(spec.cfg()));

  std::ofstream{again} << read.network.cfg();

  DarknetNetwork loaded{again, ParseMode::Train};

  ASSERT_TRUE(loaded.fill(memory, Pattern{~SEED}));
  ASSERT_TRUE(Dxxwjz3Reader{}.load(loaded.net(), path, loaded.net().n, read));

  EXPECT_TRUE(loaded.holds(memory, Pattern{SEED}));
  EXPECT_EQ(read.network.classes(), rest.network.classes());
  EXPECT_EQ(read.training.size(), 1U);
}

TEST_F(CTEST_Dxxwjz3, original_weights_convert_into_the_same_bytes)
{
  for (const auto& [cfg, weights] :
       {std::pair{CTEST_Dxxwjz3_provider::CFG_1, CTEST_Dxxwjz3_provider::W_1},
        std::pair{CTEST_Dxxwjz3_provider::CFG_2,
                  CTEST_Dxxwjz3_provider::W_2}}) {
    SCOPED_TRACE(weights);

    DarknetNetwork darknet{cfg, ParseMode::Inference};
    WeightsDocument document;
    WeightsDocument read;
    std::string text;

    ASSERT_TRUE(OrigWeightsReader{}.read(weights, &darknet.net(), document));
    ASSERT_TRUE(NetworkFiles::read_text(cfg, text));

    document.network = NetworkDescription{text, {"a", "b"}};

    const std::string dxxwjz3 = workspace.file("reference.dxxwjz3");
    const std::string back = workspace.file("reference.weights");

    ASSERT_TRUE(Dxxwjz3Writer{}.write(document, dxxwjz3));
    ASSERT_TRUE(Dxxwjz3Reader{}.read(dxxwjz3, &darknet.net(), read));

    EXPECT_EQ(Dxxwjz3CfgJson::members(read.network.cfg()),
              Dxxwjz3CfgJson::members(text));
    EXPECT_EQ(read.network.classes(), document.network.classes());

    ASSERT_TRUE(OrigWeightsWriter{}.write(read, back));
    EXPECT_TRUE(files_equal(weights, back));
  }
}

TEST_F(CTEST_Dxxwjz3, trailing_bytes_are_the_array_of_their_values)
{
  const std::string path = workspace.file("trailing.dxxwjz3");
  WeightsDocument document;
  WeightsDocument read;

  document.network = NetworkDescription{"[net]\nwidth=8\n", {}};
  document.layers.push_back({0, MAXPOOL, {}});
  document.trailing = {0U, 1U, 255U};

  ASSERT_TRUE(Dxxwjz3Writer{}.write(document, path));
  EXPECT_NE(gunzipped(path).find("\"trailing\": [0,1,255]\n}"),
            std::string::npos);
  ASSERT_TRUE(Dxxwjz3Reader{}.read(path, nullptr, read));
  EXPECT_EQ(read.trailing, document.trailing);

  for (const char* const wrong :
       {R"("AAE=")", "[256]", "[-1]", "[1.5]", "[[1]]", "7"}) {
    EXPECT_TRUE(rejects(
        document_text(R"({"net": {}, "layers": []})",
                      ", " + WEIGHTS_MEMBERS + R"(, "trailing": )" + wrong),
        {"\"trailing\"", "the integers from 0 to 255"}));
  }
}

TEST_F(CTEST_Dxxwjz3, wrong_networks_are_rejected)
{
  const std::vector<std::pair<std::string, std::vector<std::string>>> cases{
      {document_text(R"({"cfg": "[net]\n"})"), {"has no \"net\" key"}},
      {document_text(R"({"net": [], "layers": []})"),
       {"network.\"net\"", "isn't an object"}},
      {document_text(R"({"net": {}, "layers": {}})"),
       {"network.\"layers\"", "isn't an array"}},
      {document_text(R"({"net": {}, "layers": [{"options": {}}]})"),
       {"has no \"type\" key"}},
      {document_text(R"({"net": {}, "layers": [{"type": "max pool"}]})"),
       {"\"type\"", "isn't the name of a cfg section"}},
      {document_text(
           R"({"net": {}, "layers": [{"type": "maxpool", "options": []}]})"),
       {"\"options\"", "isn't an object"}},
      {document_text(R"({"net": {"a=b": 1}, "layers": []})"),
       {"\"a=b\"", "isn't the key of a cfg option"}},
      {document_text(R"({"net": {"#batch": 1}, "layers": []})"),
       {"isn't the key of a cfg option"}},
      {document_text(R"({"net": {"train": "my data.txt"}, "layers": []})"),
       {"\"train\"", "isn't a cfg option value"}},
      {document_text(R"({"net": {"batch": true}, "layers": []})"),
       {"isn't a cfg option value"}},
      {document_text(R"({"net": {"steps": [[1]]}, "layers": []})"),
       {"isn't a cfg option value"}},
      {R"({"format": "dxxwjz2", "version": 2, "network": {"cfg": "[net]\n"}})",
       {"dxxwjz2"}}};

  for (const auto& [text, words] : cases) {
    EXPECT_TRUE(rejects(text, words));
  }
}

TEST_F(CTEST_Dxxwjz3, cfg_the_json_cannot_keep_is_not_written)
{
  const std::string path = workspace.file("unwritten.dxxwjz3");

  for (const char* const cfg :
       {"[convolutional]\nfilters=8\n", "width=8\n[net]\n", "[net]\n[maxpool\n",
        "# no sections\n"}) {
    WeightsDocument document;

    document.network = NetworkDescription{cfg, {}};

    EXPECT_FALSE(Dxxwjz3Writer{}.write(document, path)) << cfg;
    EXPECT_FALSE(std::filesystem::exists(path));
  }
}
