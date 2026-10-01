#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1FloatCodec.h"

#include <charconv>
#include <clocale>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <string>
#include <system_error>

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

namespace
{

constexpr const char* const HEX_DIGITS = "0123456789abcdef";

/// @brief The count of the hexadecimal digits of the bit pattern.
constexpr const std::size_t BITS_DIGITS = 8U;

int hex_value(const char symbol)
{
  if (symbol >= '0' && symbol <= '9') {
    return symbol - '0';
  }

  if (symbol >= 'a' && symbol <= 'f') {
    return symbol - 'a' + 10;
  }

  if (symbol >= 'A' && symbol <= 'F') {
    return symbol - 'A' + 10;
  }

  return -1;
}

float float_of(const uint32_t bits)
{
  float value = 0.f;
  std::memcpy(&value, &bits, sizeof(value));
  return value;
}

}  // namespace

uint32_t Dxxwjz1FloatCodec::bits_of(const float value)
{
  uint32_t bits = 0U;
  std::memcpy(&bits, &value, sizeof(bits));
  return bits;
}

char* Dxxwjz1FloatCodec::encode(const float value, char* first)
{
  if (!std::isfinite(value)) {
    const uint32_t bits = bits_of(value);

    *first++ = '"';
    *first++ = '0';
    *first++ = 'x';

    for (std::size_t digit = BITS_DIGITS; digit > 0U; --digit) {
      *first++ = HEX_DIGITS[(bits >> ((digit - 1U) * 4U)) & 0xFU];
    }

    *first++ = '"';

    return first;
  }

  if (value == 0.f && std::signbit(value)) {
    // the "-0" is the integer number the JSON readers take for the zero
    static constexpr const char NEGATIVE_ZERO[] = "-0.0";

    std::memcpy(first, NEGATIVE_ZERO, sizeof(NEGATIVE_ZERO) - 1U);

    return first + sizeof(NEGATIVE_ZERO) - 1U;
  }

  // the shortest decimal the float round trips of, the "C" locale one
  return std::to_chars(first, first + MAX_TEXT, value).ptr;
}

std::string Dxxwjz1FloatCodec::encode(const float value)
{
  char text[MAX_TEXT];

  return std::string{text, encode(value, text)};
}

bool Dxxwjz1FloatCodec::decode_number(const std::string& text, float& value)
{
  return decode_number(text, value, decimal_point());
}

bool Dxxwjz1FloatCodec::decode_number(const std::string& text, float& value,
                                      const char point)
{
  if (text.empty()) {
    return false;
  }

  const std::string* source = &text;
  std::string normalized;

  if (point != '.' && text.find(point) != std::string::npos) {
    normalized = text;

    for (char& symbol : normalized) {
      if (symbol == point) {
        symbol = '.';
      }
    }

    source = &normalized;
  }

  const char* const first = source->data();
  const char* const last = first + source->size();

  float parsed = 0.f;
  const std::from_chars_result result = std::from_chars(first, last, parsed);

  if (result.ec != std::errc{} || result.ptr != last) {
    return false;
  }

  value = parsed;

  return true;
}

char Dxxwjz1FloatCodec::decimal_point()
{
  const char* const point = std::localeconv()->decimal_point;

  // the first byte of the point, the one the JSON lexer puts in the number
  // texts
  return point != nullptr && point[0] != '\0' ? point[0] : '.';
}

bool Dxxwjz1FloatCodec::decode_bits(const std::string& text, float& value)
{
  if (text.size() != BITS_DIGITS + 2U || text[0] != '0' ||
      (text[1] != 'x' && text[1] != 'X')) {
    return false;
  }

  uint32_t bits = 0U;

  for (std::size_t index = 2U; index < text.size(); ++index) {
    const int digit = hex_value(text[index]);

    if (digit < 0) {
      return false;
    }

    bits = (bits << 4U) | static_cast<uint32_t>(digit);
  }

  value = float_of(bits);

  return true;
}

std::string Dxxwjz1FloatCodec::bits_text(const float value)
{
  const uint32_t bits = bits_of(value);
  std::string text{"0x"};

  for (std::size_t digit = BITS_DIGITS; digit > 0U; --digit) {
    text.push_back(HEX_DIGITS[(bits >> ((digit - 1U) * 4U)) & 0xFU]);
  }

  return text;
}

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1
