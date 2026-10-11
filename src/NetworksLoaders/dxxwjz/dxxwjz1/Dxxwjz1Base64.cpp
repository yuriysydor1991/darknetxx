#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Base64.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

namespace
{

constexpr const char* const ALPHABET =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

constexpr const char PADDING = '=';

int value_of(const char symbol)
{
  if (symbol >= 'A' && symbol <= 'Z') {
    return symbol - 'A';
  }

  if (symbol >= 'a' && symbol <= 'z') {
    return symbol - 'a' + 26;
  }

  if (symbol >= '0' && symbol <= '9') {
    return symbol - '0' + 52;
  }

  if (symbol == '+') {
    return 62;
  }

  if (symbol == '/') {
    return 63;
  }

  return -1;
}

}  // namespace

std::string Dxxwjz1Base64::encode(const std::vector<uint8_t>& bytes)
{
  std::string text;
  text.reserve((bytes.size() + 2U) / 3U * 4U);

  for (std::size_t index = 0U; index < bytes.size(); index += 3U) {
    const std::size_t count = std::min<std::size_t>(3U, bytes.size() - index);

    uint32_t group = static_cast<uint32_t>(bytes[index]) << 16U;

    if (count > 1U) {
      group |= static_cast<uint32_t>(bytes[index + 1U]) << 8U;
    }

    if (count > 2U) {
      group |= static_cast<uint32_t>(bytes[index + 2U]);
    }

    text.push_back(ALPHABET[(group >> 18U) & 0x3FU]);
    text.push_back(ALPHABET[(group >> 12U) & 0x3FU]);
    text.push_back(count > 1U ? ALPHABET[(group >> 6U) & 0x3FU] : PADDING);
    text.push_back(count > 2U ? ALPHABET[group & 0x3FU] : PADDING);
  }

  return text;
}

bool Dxxwjz1Base64::decode(const std::string& text, std::vector<uint8_t>& bytes)
{
  if (text.size() % 4U != 0U) {
    return false;
  }

  std::vector<uint8_t> decoded;
  decoded.reserve(text.size() / 4U * 3U);

  for (std::size_t index = 0U; index < text.size(); index += 4U) {
    const bool last = index + 4U == text.size();
    // the padding ends the last group alone: "xx==" or "xxx="
    const std::size_t padding =
        last ? (text[index + 3U] == PADDING ? 1U : 0U) +
                   (text[index + 2U] == PADDING ? 1U : 0U)
             : 0U;

    if (last && text[index + 2U] == PADDING && text[index + 3U] != PADDING) {
      return false;
    }

    uint32_t group = 0U;

    for (std::size_t symbol = 0U; symbol < 4U; ++symbol) {
      const bool padded = symbol >= 4U - padding;
      const int value = padded ? 0 : value_of(text[index + symbol]);

      if (value < 0) {
        return false;
      }

      group = (group << 6U) | static_cast<uint32_t>(value);
    }

    decoded.push_back(static_cast<uint8_t>((group >> 16U) & 0xFFU));

    if (padding < 2U) {
      decoded.push_back(static_cast<uint8_t>((group >> 8U) & 0xFFU));
    }

    if (padding < 1U) {
      decoded.push_back(static_cast<uint8_t>(group & 0xFFU));
    }
  }

  bytes = std::move(decoded);

  return true;
}

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1
