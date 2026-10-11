#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "CTEST_Dxxwjz2_provider.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Parser.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/zlib/ZlibController.h"

using namespace testing;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsHeader;
using darknet_adaptor::loaders::WeightsReport;
using darknet_adaptor::loaders::dxxwjz2::v2::Dxxwjz2Parser;

namespace
{

const std::string NETWORK_MEMBER =
    R"("network": {"cfg": "[net]\nwidth=8\n", "classes": [{"index": 0, "name": "cat"}, {"index": 1, "name": "dog"}]})";

const std::string TRAINING_MEMBER =
    R"("training": {"seconds": 75, "iteration": [1, 2, 3], "loss": [9.5, 8.25, 7], "avg_loss": [9.5, 9.375, 9.1375], "map_iteration": [2], "map": [0.25]})";

const std::string HEADER_MEMBER =
    R"("header": {"major": 0, "minor": 2, "revision": 5, "seen": 192})";

const std::string LAYERS_MEMBER =
    R"("layers": [{"index": 0, "type": "convolutional", "arrays": {"biases": [0.5, -1]}}, {"index": 1, "type": "maxpool"}])";

/// @brief The document of the members.
std::string document(const std::string& members)
{
  return R"({"format": "dxxwjz2", "version": 2, )" + members + "}";
}

bool parse(const std::string& text, WeightsDocument& result,
           WeightsReport& report)
{
  std::istringstream json{text};

  return Dxxwjz2Parser::parse(json, result, report);
}

/// @brief Parses the text expected to fail with the error of the words.
::testing::AssertionResult rejects(const std::string& text,
                                   const std::vector<std::string>& words)
{
  WeightsDocument result;
  WeightsReport report;

  if (parse(text, result, report)) {
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

uint32_t bits_of(const float value)
{
  uint32_t bits = 0U;
  std::memcpy(&bits, &value, sizeof(bits));
  return bits;
}

}  // namespace

class CTEST_Dxxwjz2_parser : public Test
{
 public:
  CTEST_Dxxwjz2_parser()
      : dir{std::filesystem::path{CTEST_Dxxwjz2_provider::GENERATED_DIR} /
            UnitTest::GetInstance()->current_test_info()->name()}
  {
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);
  }

  ~CTEST_Dxxwjz2_parser() override { std::filesystem::remove_all(dir); }

  /// @brief The gzip compressed file of the text.
  std::string file_of(const std::string& text)
  {
    const std::string path = (dir / "peeked.dxxwjz2").string();
    std::unique_ptr<std::ostream> file =
        zlibi::ZlibController::create()->create_file_writer(path);

    *file << text;
    file.reset();

    return path;
  }

  std::filesystem::path dir;
};

TEST_F(CTEST_Dxxwjz2_parser, reads_the_document)
{
  WeightsDocument result;
  WeightsReport report;

  ASSERT_TRUE(parse(document(NETWORK_MEMBER + ", " + TRAINING_MEMBER + ", " +
                             HEADER_MEMBER + ", " + LAYERS_MEMBER),
                    result, report))
      << report.first_error();

  EXPECT_TRUE(report.warnings().empty());
  EXPECT_EQ(result.network.cfg(), "[net]\nwidth=8\n");
  EXPECT_EQ(result.network.classes(), (std::vector<std::string>{"cat", "dog"}));
  EXPECT_EQ(result.training.seconds(), 75.0);
  EXPECT_EQ(result.training.iterations(), (std::vector<int>{1, 2, 3}));
  EXPECT_EQ(result.training.losses(), (std::vector<float>{9.5f, 8.25f, 7.f}));
  EXPECT_EQ(result.training.avg_losses(),
            (std::vector<float>{9.5f, 9.375f, 9.1375f}));
  EXPECT_EQ(result.training.map_iterations(), (std::vector<int>{2}));
  EXPECT_EQ(result.training.maps(), (std::vector<float>{0.25f}));
  EXPECT_EQ(result.header.seen, 192U);
  ASSERT_EQ(result.layers.size(), 2U);
  EXPECT_EQ(result.layers[0].arrays[0].values,
            (std::vector<float>{0.5f, -1.f}));
  EXPECT_TRUE(result.layers[1].arrays.empty());
}

TEST_F(CTEST_Dxxwjz2_parser, the_network_alone_is_the_network_to_train)
{
  WeightsDocument result;
  WeightsReport report;

  ASSERT_TRUE(parse(document(R"("network": {"cfg": "[net]"})"), result, report))
      << report.first_error();

  EXPECT_EQ(result.network.cfg(), "[net]");
  EXPECT_TRUE(result.network.classes().empty());
  EXPECT_EQ(result.training.size(), 0U);
  EXPECT_EQ(result.training.seconds(), 0.0);
  EXPECT_TRUE(result.layers.empty());
  EXPECT_EQ(result.floats(), 0U);
  EXPECT_EQ(result.header, WeightsHeader::current(0U));
}

TEST_F(CTEST_Dxxwjz2_parser, classes_of_any_order_take_their_indexes)
{
  WeightsDocument result;
  WeightsReport report;

  ASSERT_TRUE(parse(
      document(
          R"("network": {"classes": [{"name": "dog", "index": 1}, {"index": 0, "name": "cat"}], "cfg": "[net]"})"),
      result, report))
      << report.first_error();

  EXPECT_EQ(result.network.classes(), (std::vector<std::string>{"cat", "dog"}));
}

TEST_F(CTEST_Dxxwjz2_parser, non_finite_losses_are_their_bit_patterns)
{
  WeightsDocument result;
  WeightsReport report;

  ASSERT_TRUE(parse(
      document(
          NETWORK_MEMBER +
          R"(, "training": {"seconds": 1.5, "iteration": [4, 5], "loss": ["0x7fc00001", -0.0], "avg_loss": ["0xff800000", 1e-45]})"),
      result, report))
      << report.first_error();

  ASSERT_EQ(result.training.size(), 2U);
  EXPECT_EQ(bits_of(result.training.losses()[0]), 0x7FC00001U);
  EXPECT_EQ(bits_of(result.training.losses()[1]), 0x80000000U);
  EXPECT_EQ(bits_of(result.training.avg_losses()[0]), 0xFF800000U);
  EXPECT_EQ(bits_of(result.training.avg_losses()[1]), 0x00000001U);
  EXPECT_EQ(result.training.seconds(), 1.5);
}

TEST_F(CTEST_Dxxwjz2_parser, unknown_keys_are_skipped_with_the_warnings)
{
  WeightsDocument result;
  WeightsReport report;

  ASSERT_TRUE(parse(
      document(
          R"("network": {"cfg": "[net]", "author": "me", "classes": [{"index": 0, "name": "a", "color": "red"}]}, )"
          R"("training": {"seconds": 0, "iteration": [], "loss": [], "avg_loss": [], "rate": [0.5]}, "later": {"a": [1]})"),
      result, report))
      << report.first_error();

  EXPECT_EQ(report.warnings().size(), 4U);
  EXPECT_EQ(result.network.classes(), (std::vector<std::string>{"a"}));
}

TEST_F(CTEST_Dxxwjz2_parser, wrong_structures_are_rejected)
{
  const std::vector<std::pair<std::string, std::vector<std::string>>> cases{
      {document(HEADER_MEMBER), {"has no \"network\" key"}},
      {document(R"("network": 5)"), {"network isn't an object"}},
      {document(R"("network": {})"), {"network has no \"cfg\" key"}},
      {document(R"("network": {"cfg": ""})"), {"network.\"cfg\""}},
      {document(R"("network": {"cfg": 7})"), {"network.\"cfg\""}},
      {document(R"("network": {"cfg": "[net]", "classes": {}})"),
       {"\"classes\" isn't an array"}},
      {document(
           R"("network": {"cfg": "[net]", "classes": [{"index": 0, "name": "a"}, {"index": 0, "name": "b"}]})"),
       {"classes[1].\"index\" 0 is given twice"}},
      {document(
           R"("network": {"cfg": "[net]", "classes": [{"index": 1, "name": "a"}]})"),
       {"isn't an index of the 1 classes"}},
      {document(
           R"("network": {"cfg": "[net]", "classes": [{"index": -1, "name": "a"}]})"),
       {"isn't an index"}},
      {document(R"("network": {"cfg": "[net]", "classes": [{"index": 0}]})"),
       {"classes[0] has no \"name\" key"}},
      {document(
           R"("network": {"cfg": "[net]", "classes": [{"index": 0, "name": 1}]})"),
       {"\"name\" isn't a string"}},
      {document(R"("network": {"cfg": "[net]", "cfg": "[net]"})"),
       {"is given twice"}},
      {document(
           NETWORK_MEMBER +
           R"(, "training": {"seconds": 0, "iteration": [1], "loss": [], "avg_loss": []})"),
       {"columns are of the different sizes"}},
      {document(
           NETWORK_MEMBER +
           R"(, "training": {"iteration": [], "loss": [], "avg_loss": []})"),
       {"training has no \"seconds\" key"}},
      {document(
           NETWORK_MEMBER +
           R"(, "training": {"seconds": -1, "iteration": [], "loss": [], "avg_loss": []})"),
       {"isn't a count of the seconds"}},
      {document(
           NETWORK_MEMBER +
           R"(, "training": {"seconds": 0, "iteration": [1.5], "loss": [1], "avg_loss": [1]})"),
       {"\"iteration\"[0] isn't an iteration number"}},
      {document(
           NETWORK_MEMBER +
           R"(, "training": {"seconds": 0, "iteration": [1], "loss": ["one"], "avg_loss": [1]})"),
       {"\"loss\"[0] isn't a float"}},
      {document(
           NETWORK_MEMBER +
           R"(, "training": {"seconds": 0, "iteration": [], "loss": [], "avg_loss": [], "map": [0.5]})"),
       {"columns go together"}},
      {document(
           NETWORK_MEMBER +
           R"(, "training": {"seconds": 0, "iteration": [], "loss": [], "avg_loss": [], "map_iteration": [1, 2], "map": [0.5]})"),
       {"\"map_iteration\" and the", "different sizes"}},
      {document(
           NETWORK_MEMBER +
           R"(, "training": {"seconds": 0, "iteration": [], "loss": [], "avg_loss": [], "map_iteration": [2, 2], "map": [0.5, 0.6]})"),
       {"isn't of the ascending iterations"}},
      {R"({"format": "dxxwjz1", "version": 2, )" + NETWORK_MEMBER + "}",
       {"isn't the dxxwjz2 one"}},
      {R"({"format": "dxxwjz2", "version": 1, )" + NETWORK_MEMBER + "}",
       {"version 1 isn't supported"}}};

  for (const auto& [text, words] : cases) {
    EXPECT_TRUE(rejects(text, words));
  }
}

TEST_F(CTEST_Dxxwjz2_parser, peek_stops_at_the_weights)
{
  WeightsDocument rest;
  WeightsReport report;
  bool weights = false;

  ASSERT_TRUE(Dxxwjz2Parser::peek(
      file_of(document(NETWORK_MEMBER + ", " + TRAINING_MEMBER + ", " +
                       HEADER_MEMBER + ", " + LAYERS_MEMBER)),
      rest, weights, report))
      << report.first_error();

  EXPECT_TRUE(weights);
  EXPECT_EQ(rest.network.classes(), (std::vector<std::string>{"cat", "dog"}));
  EXPECT_EQ(rest.training.size(), 3U);
  EXPECT_EQ(rest.header.seen, 192U);
  EXPECT_TRUE(rest.layers.empty());
}

TEST_F(CTEST_Dxxwjz2_parser, peek_tells_the_network_to_train)
{
  WeightsDocument rest;
  WeightsReport report;
  bool weights = true;

  ASSERT_TRUE(Dxxwjz2Parser::peek(
      file_of(document(
          NETWORK_MEMBER +
          R"(, "layers": [{"index": 0, "type": "convolutional", "arrays": {"biases": []}}, {"index": 1, "type": "maxpool"}])")),
      rest, weights, report))
      << report.first_error();

  EXPECT_FALSE(weights);
  EXPECT_EQ(rest.network.cfg(), "[net]\nwidth=8\n");
}

TEST_F(CTEST_Dxxwjz2_parser, peek_reads_the_network_after_the_weights)
{
  WeightsDocument rest;
  WeightsReport report;
  bool weights = false;

  ASSERT_TRUE(Dxxwjz2Parser::peek(
      file_of(document(LAYERS_MEMBER + ", " + NETWORK_MEMBER)), rest, weights,
      report))
      << report.first_error();

  EXPECT_TRUE(weights);
  EXPECT_EQ(rest.network.classes(), (std::vector<std::string>{"cat", "dog"}));
}

TEST_F(CTEST_Dxxwjz2_parser, peek_of_the_broken_file_fails)
{
  WeightsDocument rest;
  WeightsReport report;
  bool weights = false;

  EXPECT_FALSE(Dxxwjz2Parser::peek(file_of(document(R"("network": {)")), rest,
                                   weights, report));
  EXPECT_FALSE(report.ok());
}
