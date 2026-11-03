#include <gtest/gtest.h>
#include <zlib.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "src/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Parser.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Reader.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Validator.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Writer.h"
#include "src/NetworksLoaders/weights-converter/WeightsConverter.h"
#include "src/NetworksLoaders/weights-formats/NetworkSource.h"
#include "src/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/zlib/ZlibController.h"

using darknet_adaptor::loaders::NetworkDescription;
using darknet_adaptor::loaders::NetworkSource;
using darknet_adaptor::loaders::OrigDefaultLoader;
using darknet_adaptor::loaders::WeightsConversion;
using darknet_adaptor::loaders::WeightsConverter;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsFileFormats;
using darknet_adaptor::loaders::WeightsReport;
using darknet_adaptor::loaders::dxxwjz4::v4::Dxxwjz4Parser;
using darknet_adaptor::loaders::dxxwjz4::v4::Dxxwjz4Reader;
using darknet_adaptor::loaders::dxxwjz4::v4::Dxxwjz4Validator;
using darknet_adaptor::loaders::dxxwjz4::v4::Dxxwjz4Writer;

namespace fs = std::filesystem;

namespace
{

const std::string NETWORK_MEMBER =
    R"("network": {"net": {"width": 8}, "layers": [{"type": "convolutional", "arrays": {"biases": [0.5, -1]}}, {"type": "maxpool"}], "classes": ["cat", "dog"]})";

const std::string WEIGHTS_MEMBERS =
    R"("header": {"major": 0, "minor": 2, "revision": 5, "seen": 192}, "trailing": [7])";

const std::string READ_CFG = "[net]\nwidth=8\n\n[convolutional]\n\n[maxpool]\n";

std::string document(const std::string& members)
{
  return R"({"format": "dxxwjz4", "version": 4, )" + members + "}";
}

/// @brief The JSON text of the gzip compressed file.
std::string gunzipped_text(const std::string& path)
{
  std::string text;
  gzFile file = gzopen(path.c_str(), "rb");
  char chunk[4096];
  int count = 0;

  while (file != nullptr && (count = gzread(file, chunk, sizeof(chunk))) > 0) {
    text.append(chunk, static_cast<std::size_t>(count));
  }

  if (file != nullptr) {
    gzclose(file);
  }

  return text;
}

/// @brief The bytes of the file.
std::string bytes_of(const std::string& path)
{
  std::ifstream file{path, std::ios::binary};

  return std::string{std::istreambuf_iterator<char>{file},
                     std::istreambuf_iterator<char>{}};
}

/// @brief The description of every kind of text: the lines, the quotes and
/// the backslashes, the tabs, the UTF-8 of several languages and the length
/// of the megabytes.
std::string long_description()
{
  std::string text =
      "The digits of the parking lot cameras.\n\tTrained on \"8x8\" glyphs, "
      "C:\\data\\set; Ukrainian: Цифри, Japanese: 数字, emoji: "
      "\xF0\x9F\x93\xB7\n";

  while (text.size() < 3U * 1024U * 1024U) {
    text +=
        "Another line of the comments: " + std::to_string(text.size()) + "\n";
  }

  return text;
}

}  // namespace

class CTEST_Dxxwjz4 : public testing::Test
{
 public:
  CTEST_Dxxwjz4()
      : dir{fs::path{CTEST_Dxxwjz4_DATA_DIR} /
            testing::UnitTest::GetInstance()->current_test_info()->name()}
  {
    fs::remove_all(dir);
    fs::create_directories(dir);
  }

  ~CTEST_Dxxwjz4() override { fs::remove_all(dir); }

  /// @brief The gzip compressed file of the text.
  std::string file_of(const std::string& text)
  {
    const std::string path = (dir / "net.dxxwjz4").string();
    std::unique_ptr<std::ostream> file =
        zlibi::ZlibController::create()->create_file_writer(path);

    *file << text;
    file.reset();

    return path;
  }

  /// @brief The conversion of the file, the cfg one of the original weights.
  static bool convert(const std::string& from, const std::string& src,
                      const std::string& to, const std::string& dst,
                      const std::string& description = {})
  {
    WeightsConversion conversion;
    conversion.from = WeightsFileFormats::named(from);
    conversion.src = src;
    conversion.to = WeightsFileFormats::named(to);
    conversion.dst = dst;
    conversion.cfg = from == "weights" ? CTEST_Dxxwjz4_CFG : "";
    conversion.description = description;
    return WeightsConverter{}.convert(conversion);
  }

  fs::path dir;
};

TEST_F(CTEST_Dxxwjz4, format_is_registered)
{
  const auto format = WeightsFileFormats::named("dxxwjz4");

  ASSERT_NE(format, nullptr);
  EXPECT_EQ(format->extension(), ".dxxwjz4");
  EXPECT_EQ(WeightsFileFormats::of_file("a/net.dxxwjz4"), format);
  EXPECT_TRUE(format->keeps_structure());
  EXPECT_TRUE(format->keeps_network());
  EXPECT_TRUE(format->keeps_description());
  EXPECT_FALSE(format->keeps_annotations());
  EXPECT_FALSE(format->keeps_images());
  EXPECT_TRUE(NetworkSource::describes("net.dxxwjz4"));

  // the earlier formats keep no description
  EXPECT_FALSE(WeightsFileFormats::named("dxxwjz3")->keeps_description());
  EXPECT_FALSE(WeightsFileFormats::named("dxxwjz3p")->keeps_description());
}

TEST_F(CTEST_Dxxwjz4, reads_the_description_and_the_json_network)
{
  WeightsDocument result;
  WeightsReport report;

  ASSERT_TRUE(Dxxwjz4Parser::parse_file(
      file_of(document(R"("description": "line 1\nline \"2\"", )" +
                       NETWORK_MEMBER + ", " + WEIGHTS_MEMBERS)),
      result, report))
      << report.first_error();

  EXPECT_TRUE(report.warnings().empty());
  EXPECT_EQ(result.description, "line 1\nline \"2\"");
  EXPECT_EQ(result.network.cfg(), READ_CFG);
  EXPECT_EQ(result.network.classes(), (std::vector<std::string>{"cat", "dog"}));
  ASSERT_EQ(result.layers.size(), 1U);
  EXPECT_EQ(result.layers[0].arrays.at(0).values,
            (std::vector<float>{0.5f, -1.f}));
  EXPECT_EQ(result.trailing, std::vector<uint8_t>{7U});
}

TEST_F(CTEST_Dxxwjz4, description_is_optional_and_a_string)
{
  WeightsDocument result;
  WeightsReport report;

  // the document of no description is the one of the empty description
  ASSERT_TRUE(Dxxwjz4Parser::parse_file(file_of(document(NETWORK_MEMBER)),
                                        result, report))
      << report.first_error();
  EXPECT_TRUE(result.description.empty());

  for (const std::string& wrong :
       {std::string{"42"}, std::string{"[\"a\"]"}, std::string{"{\"a\": 1}"},
        std::string{"null"}, std::string{"true"}}) {
    std::istringstream text{
        document(R"("description": )" + wrong + ", " + NETWORK_MEMBER)};
    WeightsReport wrong_report;

    EXPECT_FALSE(Dxxwjz4Parser::parse(text, result, wrong_report)) << wrong;
    EXPECT_NE(wrong_report.first_error().find("\"description\" isn't the "
                                              "string"),
              std::string::npos)
        << wrong_report.first_error();
  }

  // the repeated description key
  std::istringstream twice{
      document(R"("description": "a", "description": "b", )" + NETWORK_MEMBER)};
  WeightsReport twice_report;

  EXPECT_FALSE(Dxxwjz4Parser::parse(twice, result, twice_report));
}

TEST_F(CTEST_Dxxwjz4, other_formats_and_versions_are_refused)
{
  WeightsDocument result;

  for (const std::string& text :
       {std::string{R"({"format": "dxxwjz3", "version": 3, )"} +
            NETWORK_MEMBER + "}",
        std::string{R"({"format": "dxxwjz4", "version": 3, )"} +
            NETWORK_MEMBER + "}",
        std::string{R"({"format": "dxxwjz4", "version": 5, )"} +
            NETWORK_MEMBER + "}"}) {
    std::istringstream json{text};
    WeightsReport report;

    EXPECT_FALSE(Dxxwjz4Parser::parse(json, result, report)) << text;
  }
}

TEST_F(CTEST_Dxxwjz4, description_of_any_length_round_trips)
{
  const std::string path = (dir / "net.dxxwjz4").string();
  const std::string text = long_description();
  WeightsDocument document;

  ASSERT_TRUE(convert("weights", CTEST_Dxxwjz4_WEIGHTS, "dxxwjz4", path));
  ASSERT_TRUE(Dxxwjz4Reader{}.read(path, nullptr, document));

  document.description = text;

  ASSERT_TRUE(Dxxwjz4Writer{}.write(document, path));

  // the writers write the description before the network
  const std::string json_text = gunzipped_text(path);
  const nlohmann::json json = nlohmann::json::parse(json_text, nullptr, false);

  ASSERT_TRUE(json.is_object());
  EXPECT_EQ(json["format"], "dxxwjz4");
  EXPECT_EQ(json["version"], 4);
  EXPECT_EQ(json["description"], text);
  EXPECT_LT(json_text.find("\"description\""), json_text.find("\"network\""));

  WeightsDocument read;
  const WeightsReport report = Dxxwjz4Validator::validate(path, nullptr, read);

  ASSERT_TRUE(report.ok()) << report.first_error();
  EXPECT_EQ(read.description, text);
  EXPECT_EQ(read.network.cfg(), document.network.cfg());
  EXPECT_EQ(read.floats(), document.floats());

  // the peek reads the description without the weights
  WeightsDocument rest;
  bool weights = false;

  ASSERT_TRUE(Dxxwjz4Reader{}.peek(path, rest, weights));
  EXPECT_TRUE(weights);
  EXPECT_TRUE(rest.layers.empty());
  EXPECT_EQ(rest.description, text);
}

TEST_F(CTEST_Dxxwjz4, converts_back_and_forth_of_the_same_weights)
{
  const std::string v4 = (dir / "net.dxxwjz4").string();
  const std::string v3 = (dir / "net.dxxwjz3").string();
  const std::string back = (dir / "back.weights").string();
  const std::string again = (dir / "again.dxxwjz4").string();
  WeightsDocument source;
  WeightsDocument read;
  WeightsDocument v3_read;

  // the description of the command line goes into the dxxwjz4 file
  ASSERT_TRUE(convert("weights", CTEST_Dxxwjz4_WEIGHTS, "dxxwjz4", v4,
                      "the reference network"));
  ASSERT_TRUE(Dxxwjz4Reader{}.read(v4, nullptr, read));
  ASSERT_TRUE(WeightsFileFormats::named("weights")->create_reader()->read(
      CTEST_Dxxwjz4_WEIGHTS,
      OrigDefaultLoader{}
          .parse_network_cfg_custom(CTEST_Dxxwjz4_CFG, 1, 1)
          ->get()
          .get(),
      source));

  EXPECT_EQ(read.description, "the reference network");
  EXPECT_EQ(read.floats(), source.floats());

  // the original weights file of the same bytes
  ASSERT_TRUE(convert("dxxwjz4", v4, "weights", back));
  EXPECT_EQ(bytes_of(back), bytes_of(CTEST_Dxxwjz4_WEIGHTS));

  // the dxxwjz3 file of the same weights, of no description, and back
  ASSERT_TRUE(convert("dxxwjz4", v4, "dxxwjz3", v3));
  ASSERT_TRUE(WeightsFileFormats::named("dxxwjz3")->create_reader()->read(
      v3, nullptr, v3_read));
  EXPECT_TRUE(v3_read.description.empty());
  EXPECT_EQ(v3_read.floats(), read.floats());

  ASSERT_TRUE(convert("dxxwjz3", v3, "dxxwjz4", again, "back again"));

  WeightsDocument again_read;

  ASSERT_TRUE(Dxxwjz4Reader{}.read(again, nullptr, again_read));
  EXPECT_EQ(again_read.description, "back again");
  EXPECT_EQ(again_read.network.cfg(), read.network.cfg());
  ASSERT_EQ(again_read.layers.size(), read.layers.size());

  for (std::size_t at = 0U; at < read.layers.size(); ++at) {
    ASSERT_EQ(again_read.layers[at].arrays.size(),
              read.layers[at].arrays.size());

    for (std::size_t array = 0U; array < read.layers[at].arrays.size();
         ++array) {
      EXPECT_EQ(again_read.layers[at].arrays[array].values,
                read.layers[at].arrays[array].values);
    }
  }

  // the dxxwjz4 file of the dxxwjz4 one keeps the description of the source
  ASSERT_TRUE(convert("dxxwjz4", again, "dxxwjz4", v4));
  ASSERT_TRUE(Dxxwjz4Reader{}.read(v4, nullptr, read));
  EXPECT_EQ(read.description, "back again");

  // the file is the network and the weights of the inference
  std::vector<std::string> classes;

  ASSERT_NE(OrigDefaultLoader{}.load_inference_network(v4, v4, "", classes),
            nullptr);
}

TEST_F(CTEST_Dxxwjz4, network_to_train_of_the_cfg_keeps_the_description)
{
  const std::string path = (dir / "train.dxxwjz4").string();
  WeightsConversion conversion;

  conversion.to = WeightsFileFormats::named("dxxwjz4");
  conversion.dst = path;
  conversion.cfg = CTEST_Dxxwjz4_CFG;
  conversion.description = "to be trained";

  ASSERT_TRUE(WeightsConverter{}.convert(conversion));

  WeightsDocument rest;
  bool weights = true;

  ASSERT_TRUE(Dxxwjz4Reader{}.peek(path, rest, weights));
  EXPECT_FALSE(weights);
  EXPECT_EQ(rest.description, "to be trained");

  const nlohmann::json json =
      nlohmann::json::parse(gunzipped_text(path), nullptr, false);

  EXPECT_FALSE(json.contains("header"));
}

TEST_F(CTEST_Dxxwjz4, writer_refuses_the_cfg_the_json_cannot_keep)
{
  const std::string path = (dir / "net.dxxwjz4").string();
  WeightsDocument document;

  document.network = NetworkDescription{"[maxpool]\n", {"cat"}};
  document.description = "no [net] section first";

  EXPECT_FALSE(Dxxwjz4Writer{}.write(document, path));
  EXPECT_FALSE(fs::exists(path));
}
