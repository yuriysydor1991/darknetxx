#include <gtest/gtest.h>

#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Base64.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1FloatCodec.h"

using namespace testing;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Base64;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1FloatCodec;

namespace
{

float float_of(const uint32_t bits)
{
  float value = 0.f;
  std::memcpy(&value, &bits, sizeof(value));
  return value;
}

/// @brief The float the JSON text of the codec reads back as: the number or
/// the bit pattern string.
::testing::AssertionResult round_trips(const uint32_t bits)
{
  const float value = float_of(bits);
  const std::string text = Dxxwjz1FloatCodec::encode(value);

  float decoded = 0.f;

  const bool read = text.front() == '"'
                        ? Dxxwjz1FloatCodec::decode_bits(
                              text.substr(1U, text.size() - 2U), decoded)
                        : Dxxwjz1FloatCodec::decode_number(text, decoded);

  if (!read) {
    return ::testing::AssertionFailure() << "the text " << text << " of "
                                         << std::hex << bits << " isn't read";
  }

  if (Dxxwjz1FloatCodec::bits_of(decoded) != bits) {
    return ::testing::AssertionFailure()
           << "the text " << text << " of " << std::hex << bits << " reads "
           << Dxxwjz1FloatCodec::bits_of(decoded);
  }

  return ::testing::AssertionSuccess();
}

}  // namespace

TEST(CTEST_Dxxwjz1_codec, finite_floats_are_the_shortest_json_numbers)
{
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(0.f), "0");
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(1.f), "1");
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(-2.5f), "-2.5");
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(0.1f), "0.1");
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(2.5e-5f), "2.5e-05");
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(1e20f), "1e+20");
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(std::numeric_limits<float>::max()),
            "3.4028235e+38");
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(std::numeric_limits<float>::denorm_min()),
            "1e-45");
}

TEST(CTEST_Dxxwjz1_codec, negative_zero_keeps_the_sign)
{
  // the "-0" is the integer number the JSON readers take for the zero
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(-0.f), "-0.0");
  EXPECT_TRUE(round_trips(0x80000000U));
}

TEST(CTEST_Dxxwjz1_codec, non_finite_floats_are_the_bit_pattern_strings)
{
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(std::numeric_limits<float>::infinity()),
            "\"0x7f800000\"");
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(-std::numeric_limits<float>::infinity()),
            "\"0xff800000\"");
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(float_of(0x7FC00000U)), "\"0x7fc00000\"");
  // the sign and the payload of the NaNs, the signaling ones included
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(float_of(0xFFC12345U)), "\"0xffc12345\"");
  EXPECT_EQ(Dxxwjz1FloatCodec::encode(float_of(0x7FA00001U)), "\"0x7fa00001\"");
}

TEST(CTEST_Dxxwjz1_codec, special_floats_round_trip_bit_exact)
{
  for (const uint32_t bits :
       {0x00000000U, 0x80000000U, 0x00000001U, 0x807FFFFFU, 0x00800000U,
        0x7F7FFFFFU, 0xFF7FFFFFU, 0x7F800000U, 0xFF800000U, 0x7FC00000U,
        0xFFC12345U, 0x7FA00001U, 0x3F800000U, 0x34000000U, 0x3DCCCCCDU,
        0x4B7FFFFFU, 0x4B800000U, 0x4B800001U, 0x5F000000U, 0xDF000000U}) {
    EXPECT_TRUE(round_trips(bits));
  }
}

TEST(CTEST_Dxxwjz1_codec, every_float_of_the_sampled_bit_patterns_round_trips)
{
  // every exponent with the edge and the random mantissas of both signs
  std::mt19937 random{20260929U};

  for (uint32_t sign = 0U; sign < 2U; ++sign) {
    for (uint32_t exponent = 0U; exponent < 256U; ++exponent) {
      const std::vector<uint32_t> mantissas{
          0U,
          1U,
          2U,
          0x400000U,
          0x7FFFFEU,
          0x7FFFFFU,
          static_cast<uint32_t>(random()) & 0x7FFFFFU,
          static_cast<uint32_t>(random()) & 0x7FFFFFU};

      for (const uint32_t mantissa : mantissas) {
        const uint32_t bits = (sign << 31U) | (exponent << 23U) | mantissa;

        ASSERT_TRUE(round_trips(bits));
      }
    }
  }

  // and the million of the random ones
  for (int count = 0; count < 1000000; ++count) {
    ASSERT_TRUE(round_trips(static_cast<uint32_t>(random())));
  }
}

TEST(CTEST_Dxxwjz1_codec, numbers_of_any_json_notation_are_read_exactly)
{
  const std::vector<std::pair<std::string, uint32_t>> numbers{
      {"0", 0x00000000U},        {"-0.0", 0x80000000U},
      {"-0", 0x80000000U},       {"1", 0x3F800000U},
      {"1.0", 0x3F800000U},      {"1E0", 0x3F800000U},
      {"1e+0", 0x3F800000U},     {"10e-1", 0x3F800000U},
      {"0.1", 0x3DCCCCCDU},      {"0.100000001490116119384765625", 0x3DCCCCCDU},
      {"16777217", 0x4B800000U}, {"3.4028235e38", 0x7F7FFFFFU},
      {"1.4e-45", 0x00000001U},  {"-1.17549435E-38", 0x80800000U}};

  for (const auto& [text, bits] : numbers) {
    float value = 0.f;

    ASSERT_TRUE(Dxxwjz1FloatCodec::decode_number(text, value)) << text;
    EXPECT_EQ(Dxxwjz1FloatCodec::bits_of(value), bits) << text;
  }
}

TEST(CTEST_Dxxwjz1_codec, malformed_and_out_of_range_numbers_are_rejected)
{
  for (const std::string& text :
       std::vector<std::string>{"", "1e39", "-3.5e38", "1e-50", "1,5", "1.5x",
                                "abc", "0x7f800000", " 1", "1 "}) {
    float value = 7.f;

    EXPECT_FALSE(Dxxwjz1FloatCodec::decode_number(text, value)) << text;
    EXPECT_EQ(value, 7.f) << text;
  }
}

TEST(CTEST_Dxxwjz1_codec, bit_patterns_of_either_case_are_read)
{
  float value = 0.f;

  ASSERT_TRUE(Dxxwjz1FloatCodec::decode_bits("0x3f800000", value));
  EXPECT_EQ(value, 1.f);
  ASSERT_TRUE(Dxxwjz1FloatCodec::decode_bits("0X3F800000", value));
  EXPECT_EQ(value, 1.f);
  ASSERT_TRUE(Dxxwjz1FloatCodec::decode_bits("0xFFC12345", value));
  EXPECT_EQ(Dxxwjz1FloatCodec::bits_of(value), 0xFFC12345U);

  EXPECT_EQ(Dxxwjz1FloatCodec::bits_text(1.f), "0x3f800000");
  EXPECT_EQ(Dxxwjz1FloatCodec::bits_text(-0.f), "0x80000000");

  for (const std::string& text : std::vector<std::string>{
           "", "0x", "0x3f80000", "0x3f8000000", "3f800000", "0y3f800000",
           "0x3f80000g", "nan", "inf", "-inf", "0x-3f80000"}) {
    EXPECT_FALSE(Dxxwjz1FloatCodec::decode_bits(text, value)) << text;
  }
}

TEST(CTEST_Dxxwjz1_codec, base64_matches_the_rfc_4648_test_vectors)
{
  const std::vector<std::pair<std::string, std::string>> vectors{
      {"", ""},
      {"f", "Zg=="},
      {"fo", "Zm8="},
      {"foo", "Zm9v"},
      {"foob", "Zm9vYg=="},
      {"fooba", "Zm9vYmE="},
      {"foobar", "Zm9vYmFy"}};

  for (const auto& [plain, encoded] : vectors) {
    const std::vector<uint8_t> bytes(plain.begin(), plain.end());

    EXPECT_EQ(Dxxwjz1Base64::encode(bytes), encoded);

    std::vector<uint8_t> decoded{0x42U};

    ASSERT_TRUE(Dxxwjz1Base64::decode(encoded, decoded)) << encoded;
    EXPECT_EQ(decoded, bytes) << encoded;
  }
}

TEST(CTEST_Dxxwjz1_codec, base64_round_trips_every_byte_and_length)
{
  std::vector<uint8_t> bytes;

  for (int length = 0; length <= 300; ++length) {
    std::vector<uint8_t> decoded;

    ASSERT_TRUE(Dxxwjz1Base64::decode(Dxxwjz1Base64::encode(bytes), decoded))
        << length;
    ASSERT_EQ(decoded, bytes) << length;

    bytes.push_back(static_cast<uint8_t>(255 - length % 256));
  }
}

TEST(CTEST_Dxxwjz1_codec, malformed_base64_is_rejected)
{
  for (const std::string& text : std::vector<std::string>{
           "Z", "Zg", "Zg=", "Zg===", "Z===", "=Zg=", "Zg==Zg==", "Zm9v!",
           "Zm 9v", "Z=g=", "Zm=v"}) {
    std::vector<uint8_t> bytes{0x42U};

    EXPECT_FALSE(Dxxwjz1Base64::decode(text, bytes)) << text;
    EXPECT_EQ(bytes, std::vector<uint8_t>{0x42U}) << text;
  }
}
