#include <gtest/gtest.h>

#include <clocale>
#include <cstdint>
#include <cstring>
#include <limits>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Parser.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet.h"

using namespace testing;
using darknet_adaptor::loaders::WeightsArray;
using darknet_adaptor::loaders::WeightsArrayKind;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsReport;
using darknet_adaptor::loaders::WeightsSubLayer;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Parser;

namespace
{

const std::string HEADER =
    R"("header": {"major": 0, "minor": 2, "revision": 5, "seen": 64})";

/// @brief The document of the layers.
std::string document(const std::string& layers)
{
  return R"({"format": "dxxwjz1", "version": 1, )" + HEADER +
         R"(, "layers": [)" + layers + "]}";
}

bool parse(const std::string& text, WeightsDocument& result,
           WeightsReport& report)
{
  std::istringstream json{text};

  return Dxxwjz1Parser::parse(json, result, report);
}

/// @brief Parses the text of the error, expected to fail with the error
/// mentioning the words.
::testing::AssertionResult rejects(const std::string& text,
                                   const std::vector<std::string>& words)
{
  WeightsDocument result;
  WeightsReport report;

  if (parse(text, result, report)) {
    return ::testing::AssertionFailure() << "the text is accepted: " << text;
  }

  if (report.errors().size() != 1U) {
    return ::testing::AssertionFailure()
           << report.errors().size() << " errors of the text " << text;
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

std::vector<uint32_t> bits_of(const std::vector<float>& values)
{
  std::vector<uint32_t> result;

  for (const float value : values) {
    uint32_t bits = 0U;
    std::memcpy(&bits, &value, sizeof(bits));
    result.push_back(bits);
  }

  return result;
}

}  // namespace

TEST(CTEST_Dxxwjz1_parser, reads_the_document)
{
  WeightsDocument result;
  WeightsReport report;

  ASSERT_TRUE(parse(
      document(
          R"({"index": 0, "type": "convolutional", "arrays": {"biases": [0.5, -1], "weights": [1, 2, "0x7fc00000", -0.0]}},)"
          R"({"index": 1, "type": "maxpool"},)"
          R"({"index": 2, "type": "rnn", "sublayers": {"input_layer": {"arrays": {"biases": [3]}}, "self_layer": {"arrays": {"weights": [4, 5]}}}})"),
      result, report))
      << report.first_error();

  EXPECT_TRUE(report.warnings().empty());
  EXPECT_EQ(result.header.major, 0);
  EXPECT_EQ(result.header.minor, 2);
  EXPECT_EQ(result.header.revision, 5);
  EXPECT_EQ(result.header.seen, 64U);
  EXPECT_TRUE(result.trailing.empty());

  ASSERT_EQ(result.layers.size(), 3U);

  EXPECT_EQ(result.layers[0].index, 0);
  EXPECT_EQ(result.layers[0].type, CONVOLUTIONAL);
  ASSERT_EQ(result.layers[0].arrays.size(), 2U);
  EXPECT_EQ(result.layers[0].arrays[0].kind, WeightsArrayKind::Biases);
  EXPECT_EQ(result.layers[0].arrays[0].values,
            (std::vector<float>{0.5f, -1.f}));
  EXPECT_EQ(result.layers[0].arrays[1].kind, WeightsArrayKind::Weights);
  EXPECT_EQ(bits_of(result.layers[0].arrays[1].values),
            (std::vector<uint32_t>{0x3F800000U, 0x40000000U, 0x7FC00000U,
                                   0x80000000U}));

  EXPECT_EQ(result.layers[1].type, MAXPOOL);
  EXPECT_TRUE(result.layers[1].arrays.empty());

  EXPECT_EQ(result.layers[2].type, RNN);
  ASSERT_EQ(result.layers[2].arrays.size(), 2U);
  EXPECT_EQ(result.layers[2].arrays[0].sub, WeightsSubLayer::InputLayer);
  EXPECT_EQ(result.layers[2].arrays[0].values, (std::vector<float>{3.f}));
  EXPECT_EQ(result.layers[2].arrays[1].sub, WeightsSubLayer::SelfLayer);
  EXPECT_EQ(result.layers[2].arrays[1].kind, WeightsArrayKind::Weights);
}

TEST(CTEST_Dxxwjz1_parser, keys_of_any_order_make_the_normalized_document)
{
  WeightsDocument result;
  WeightsReport report;

  // the layers after the version, the type after the arrays, the arrays out
  // of the file order, the layers out of the index order
  ASSERT_TRUE(parse(
      R"({"layers": [)"
      R"({"arrays": {"weights": [4], "rolling_variance": [3], "scales": [2], "biases": [1]}, "type": "connected", "index": 5},)"
      R"({"type": "gru", "index": 2, "sublayers": {"state_h_layer": {"arrays": {"biases": [7]}}, "input_z_layer": {"arrays": {"weights": [6]}}}})"
      R"(], "header": {"seen": 3, "revision": 2, "minor": 1, "major": 7}, "version": 1, "format": "dxxwjz1"})",
      result, report))
      << report.first_error();

  EXPECT_EQ(result.header.major, 7);
  EXPECT_EQ(result.header.minor, 1);
  EXPECT_EQ(result.header.revision, 2);
  EXPECT_EQ(result.header.seen, 3U);

  ASSERT_EQ(result.layers.size(), 2U);
  EXPECT_EQ(result.layers[0].index, 2);
  EXPECT_EQ(result.layers[1].index, 5);

  // the file order of the connected arrays: biases, weights, scales, rolling
  // mean and variance
  std::vector<WeightsArrayKind> kinds;
  for (const WeightsArray& array : result.layers[1].arrays) {
    kinds.push_back(array.kind);
  }

  EXPECT_EQ(kinds,
            (std::vector<WeightsArrayKind>{
                WeightsArrayKind::Biases, WeightsArrayKind::Weights,
                WeightsArrayKind::Scales, WeightsArrayKind::RollingVariance}));

  ASSERT_EQ(result.layers[0].arrays.size(), 2U);
  EXPECT_EQ(result.layers[0].arrays[0].sub, WeightsSubLayer::InputZLayer);
  EXPECT_EQ(result.layers[0].arrays[1].sub, WeightsSubLayer::StateHLayer);
}

TEST(CTEST_Dxxwjz1_parser, unknown_keys_are_skipped_with_the_warnings)
{
  WeightsDocument result;
  WeightsReport report;

  ASSERT_TRUE(parse(
      R"({"format": "dxxwjz1", "comment": {"nested": [1, {"deep": [[2], "x"]}], "weights": [9]}, "version": 1, )"
      R"("generator": "somebody", "extra": [{"layers": []}], "number": 1.5, "flag": true, "nothing": null, )" +
          HEADER.substr(0U, HEADER.size() - 1U) +
          R"(, "note": "hi"}, "layers": [{"index": 0, "type": "shortcut", "why": "not", "arrays": {"weights": [1]}}]})",
      result, report))
      << report.first_error();

  EXPECT_EQ(report.warnings().size(), 7U);
  ASSERT_EQ(result.layers.size(), 1U);
  EXPECT_EQ(result.layers[0].arrays.size(), 1U);
  EXPECT_EQ(result.header.seen, 64U);
}

TEST(CTEST_Dxxwjz1_parser, trailing_bytes_are_the_base64_text)
{
  WeightsDocument result;
  WeightsReport report;

  ASSERT_TRUE(parse(R"({"format": "dxxwjz1", "version": 1, )" + HEADER +
                        R"(, "layers": [], "trailing": "AAEC/w=="})",
                    result, report))
      << report.first_error();

  EXPECT_EQ(result.trailing,
            (std::vector<uint8_t>{0x00U, 0x01U, 0x02U, 0xFFU}));
}

TEST(CTEST_Dxxwjz1_parser, header_numbers_take_the_whole_ranges)
{
  WeightsDocument result;
  WeightsReport report;

  ASSERT_TRUE(parse(
      R"({"format": "dxxwjz1", "version": 1, "header": {"major": -2147483648, "minor": 2147483647, "revision": 0, "seen": 18446744073709551615}, "layers": []})",
      result, report))
      << report.first_error();

  EXPECT_EQ(result.header.major, std::numeric_limits<int32_t>::min());
  EXPECT_EQ(result.header.minor, std::numeric_limits<int32_t>::max());
  EXPECT_EQ(result.header.seen, std::numeric_limits<uint64_t>::max());
}

TEST(CTEST_Dxxwjz1_parser, integer_values_become_the_nearest_floats)
{
  WeightsDocument result;
  WeightsReport report;

  // the "-0" is the integer zero of the JSON, the "-0.0" keeps the sign
  ASSERT_TRUE(parse(
      document(
          R"({"index": 0, "type": "implicit", "arrays": {"weights": [16777217, -3, -0, 18446744073709551615, 123456789012345678901234567890]}})"),
      result, report))
      << report.first_error();

  ASSERT_EQ(result.layers.size(), 1U);
  EXPECT_EQ(bits_of(result.layers[0].arrays[0].values),
            (std::vector<uint32_t>{0x4B800000U, 0xC0400000U, 0x00000000U,
                                   0x5F800000U, 0x6FC77488U}));
}

/**
 * @brief Checks the fractions are read in the first installed locale of the
 * names, the global one of the application, false for none installed.
 */
bool check_fractions_in_locale(const std::vector<std::string>& names)
{
  const std::string previous = std::setlocale(LC_NUMERIC, nullptr);
  std::string installed;

  for (const std::string& name : names) {
    if (std::setlocale(LC_NUMERIC, name.c_str()) != nullptr) {
      installed = name;
      break;
    }
  }

  if (installed.empty()) {
    return false;
  }

  WeightsDocument result;
  WeightsReport report;

  const bool parsed = parse(
      document(
          R"({"index": 0, "type": "implicit", "arrays": {"weights": [0.1, 2.5e-05, -1.5, 1E3]}})"),
      result, report);

  // the parse keeps the locale of the application
  EXPECT_EQ(std::setlocale(LC_NUMERIC, nullptr), installed);

  std::setlocale(LC_NUMERIC, previous.c_str());

  EXPECT_TRUE(parsed) << installed << ": " << report.first_error();

  if (parsed) {
    EXPECT_EQ(result.layers[0].arrays[0].values,
              (std::vector<float>{0.1f, 2.5e-05f, -1.5f, 1000.f}))
        << installed;
  }

  return true;
}

TEST(CTEST_Dxxwjz1_parser, numbers_are_read_in_the_comma_decimal_locale)
{
  if (!check_fractions_in_locale(
          {"uk_UA.UTF-8", "de_DE.UTF-8", "fr_FR.UTF-8", "ru_RU.UTF-8"})) {
    GTEST_SKIP() << "no comma decimal point locale is installed";
  }
}

TEST(CTEST_Dxxwjz1_parser, numbers_are_read_in_the_multibyte_decimal_locale)
{
  // the U+066B arabic decimal separator, two bytes of the UTF-8
  if (!check_fractions_in_locale({"ps_AF.UTF-8"})) {
    GTEST_SKIP() << "no multibyte decimal point locale is installed";
  }
}

TEST(CTEST_Dxxwjz1_parser, empty_arrays_and_layers_are_kept)
{
  WeightsDocument result;
  WeightsReport report;

  ASSERT_TRUE(parse(
      document(
          R"({"index": 3, "type": "convolutional", "arrays": {"biases": []}},)"
          R"({"index": 4, "type": "lstm", "sublayers": {}},)"
          R"({"index": 5, "type": "crnn", "sublayers": {"self_layer": {}}})"),
      result, report))
      << report.first_error();

  ASSERT_EQ(result.layers.size(), 3U);
  ASSERT_EQ(result.layers[0].arrays.size(), 1U);
  EXPECT_TRUE(result.layers[0].arrays[0].values.empty());
  EXPECT_TRUE(result.layers[1].arrays.empty());
  EXPECT_TRUE(result.layers[2].arrays.empty());
}

TEST(CTEST_Dxxwjz1_parser, json_syntax_errors_are_rejected)
{
  for (
      const std::string& text :
      {std::string{""}, std::string{"   "}, std::string{"{"},
       document("").substr(0U, 40U), document("") + "}", document("") + " x",
       document("") + document(""), std::string{R"({"format": "dxxwjz1",})"},
       document(
           R"({"index": 0, "type": "implicit", "arrays": {"weights": [1,]}})"),
       document(
           R"({"index": 0, "type": "implicit", "arrays": {"weights": [NaN]}})"),
       document(
           R"({"index": 0, "type": "implicit", "arrays": {"weights": [Infinity]}})"),
       document(
           R"({"index": 0, "type": "implicit", "arrays": {"weights": [01]}})"),
       document(
           R"({"index": 0, "type": "implicit", "arrays": {"weights": [.5]}})")}) {
    SCOPED_TRACE(text);

    WeightsDocument result;
    WeightsReport report;

    EXPECT_FALSE(parse(text, result, report));
    EXPECT_FALSE(report.ok());
    EXPECT_TRUE(result.layers.empty());
  }
}

TEST(CTEST_Dxxwjz1_parser, documents_of_the_wrong_structure_are_rejected)
{
  const std::vector<std::pair<std::string, std::vector<std::string>>> cases{
      {"[]", {"isn't a JSON object"}},
      {"42", {"isn't a JSON object"}},
      {"\"dxxwjz1\"", {"isn't a JSON object"}},
      {R"({"format": "dxxwjz2", "version": 1})", {"dxxwjz2", "format"}},
      {R"({"format": 1})", {"format", "integer"}},
      {R"({"format": "dxxwjz1", "version": 2})", {"version 2", "supported"}},
      {R"({"format": "dxxwjz1", "version": "1"})", {"version", "string"}},
      {R"({"format": "dxxwjz1", "version": 1.0})", {"version", "fraction"}},
      {R"({"version": 1, )" + HEADER + R"(, "layers": []})", {"\"format\""}},
      {R"({"format": "dxxwjz1", )" + HEADER + R"(, "layers": []})",
       {"\"version\""}},
      {R"({"format": "dxxwjz1", "version": 1, "layers": []})", {"\"header\""}},
      {R"({"format": "dxxwjz1", "version": 1, )" + HEADER + "}",
       {"\"layers\""}},
      {R"({"format": "dxxwjz1", "version": 1, "header": {"major": 0, "minor": 2, "revision": 5}, "layers": []})",
       {"header", "\"seen\""}},
      {R"({"format": "dxxwjz1", "version": 1, "header": [], "layers": []})",
       {"header", "array"}},
      {R"({"format": "dxxwjz1", "version": 1, "header": {"major": 2147483648, "minor": 0, "revision": 0, "seen": 0}, "layers": []})",
       {"major", "32 bits"}},
      {R"({"format": "dxxwjz1", "version": 1, "header": {"major": 0, "minor": -2147483649, "revision": 0, "seen": 0}, "layers": []})",
       {"minor", "32 bits"}},
      {R"({"format": "dxxwjz1", "version": 1, "header": {"major": 0, "minor": 0, "revision": 0, "seen": -1}, "layers": []})",
       {"seen", "negative"}},
      {R"({"format": "dxxwjz1", "version": 1, "header": {"major": 0, "minor": 0, "revision": 0, "seen": 1.5}, "layers": []})",
       {"seen", "fraction"}},
      {R"({"format": "dxxwjz1", "version": 1, )" + HEADER +
           R"(, "layers": {}})",
       {"layers", "object"}},
      {document("1"), {"layers", "layer object"}},
      {document("[]"), {"layers", "layer object"}},
      {document(R"({"type": "maxpool"})"), {"layers[0]", "\"index\""}},
      {document(R"({"index": 0})"), {"layers[0]", "\"type\""}},
      {document(R"({"index": -1, "type": "maxpool"})"), {"index", "range"}},
      {document(R"({"index": 2147483648, "type": "maxpool"})"),
       {"index", "range"}},
      {document(R"({"index": "0", "type": "maxpool"})"), {"index", "string"}},
      {document(R"({"index": 0, "type": "maxpooling"})"),
       {"maxpooling", "layer type"}},
      {document(R"({"index": 0, "type": 3})"), {"type", "integer"}},
      {document(R"({"index": 0, "index": 1, "type": "maxpool"})"),
       {"\"index\"", "twice"}},
      {document(R"({"index": 0, "type": "convolutional", "arrays": []})"),
       {"arrays", "array"}},
      {document(
           R"({"index": 0, "type": "convolutional", "arrays": {"bias": [1]}})"),
       {"bias", "array name"}},
      {document(
           R"({"index": 0, "type": "convolutional", "arrays": {"biases": [1], "biases": [2]}})"),
       {"biases", "twice"}},
      {document(
           R"({"index": 0, "type": "convolutional", "arrays": {"biases": {}}})"),
       {"biases", "object"}},
      {document(
           R"({"index": 0, "type": "convolutional", "arrays": {"biases": 1}})"),
       {"biases", "integer"}},
      {document(
           R"({"index": 0, "type": "rnn", "sublayers": {"input": {"arrays": {}}}})"),
       {"input", "sub-layer name"}},
      {document(
           R"({"index": 0, "type": "rnn", "sublayers": {"input_layer": []}})"),
       {"input_layer", "array"}},
      {document(
           R"({"index": 0, "type": "rnn", "sublayers": {"input_layer": {}, "input_layer": {}}})"),
       {"input_layer", "twice"}},
      {document(
           R"({"index": 0, "type": "convolutional", "arrays": {"biases": [1, null]}})"),
       {"layers[0].arrays.biases[1]", "null"}},
      {document(
           R"({"index": 0, "type": "convolutional", "arrays": {"biases": [true]}})"),
       {"biases[0]", "boolean"}},
      {document(
           R"({"index": 0, "type": "convolutional", "arrays": {"biases": [[1]]}})"),
       {"biases[0]", "array"}},
      {document(
           R"({"index": 0, "type": "convolutional", "arrays": {"biases": [{}]}})"),
       {"biases[0]", "object"}},
      {document(
           R"({"index": 0, "type": "convolutional", "arrays": {"biases": ["nan"]}})"),
       {"biases[0]", "bit pattern"}},
      {document(
           R"({"index": 0, "type": "convolutional", "arrays": {"biases": ["0x7fc0000"]}})"),
       {"biases[0]", "bit pattern"}},
      {document(
           R"({"index": 0, "type": "convolutional", "arrays": {"biases": [1, 2, 1e39]}})"),
       {"biases[2]", "float range"}},
      {document(
           R"({"index": 0, "type": "rnn", "sublayers": {"self_layer": {"arrays": {"weights": [-1e-50]}}}})"),
       {"layers[0].sublayers.self_layer.arrays.weights[0]", "float range"}},
      {R"({"format": "dxxwjz1", "version": 1, )" + HEADER +
           R"(, "layers": [], "trailing": "AAE"})",
       {"trailing", "base64"}},
      {R"({"format": "dxxwjz1", "version": 1, )" + HEADER +
           R"(, "layers": [], "trailing": 1})",
       {"trailing", "integer"}}};

  for (const auto& [text, words] : cases) {
    SCOPED_TRACE(text);

    EXPECT_TRUE(rejects(text, words));
  }
}
