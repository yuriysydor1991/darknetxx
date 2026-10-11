#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1JsonString.h"

#include <cstddef>
#include <string>

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

std::string Dxxwjz1JsonString::quote(const std::string& text)
{
  std::string result{"\""};

  for (std::size_t position = 0U; position < text.size();) {
    const std::size_t length = sequence(text, position);

    if (length == 0U) {
      result += "\xEF\xBF\xBD";
      ++position;
    } else if (length == 1U) {
      escape(text[position++], result);
    } else {
      result.append(text, position, length);
      position += length;
    }
  }

  return result + "\"";
}

std::size_t Dxxwjz1JsonString::sequence(const std::string& text,
                                        const std::size_t position)
{
  const auto byte = [&text, position](const std::size_t offset) {
    return position + offset < text.size()
               ? static_cast<unsigned>(
                     static_cast<unsigned char>(text[position + offset]))
               : 0x100U;
  };

  const unsigned first = byte(0U);

  if (first < 0x80U) {
    return 1U;
  }

  // the length and the second byte range of the leading byte (RFC 3629)
  std::size_t length = 0U;
  unsigned low = 0x80U;
  unsigned high = 0xBFU;

  if (first >= 0xC2U && first <= 0xDFU) {
    length = 2U;
  } else if (first >= 0xE0U && first <= 0xEFU) {
    length = 3U;
    low = first == 0xE0U ? 0xA0U : low;
    high = first == 0xEDU ? 0x9FU : high;
  } else if (first >= 0xF0U && first <= 0xF4U) {
    length = 4U;
    low = first == 0xF0U ? 0x90U : low;
    high = first == 0xF4U ? 0x8FU : high;
  } else {
    return 0U;
  }

  if (byte(1U) < low || byte(1U) > high) {
    return 0U;
  }

  for (std::size_t offset = 2U; offset < length; ++offset) {
    if (byte(offset) < 0x80U || byte(offset) > 0xBFU) {
      return 0U;
    }
  }

  return length;
}

void Dxxwjz1JsonString::escape(const char symbol, std::string& result)
{
  static constexpr const char* const HEX = "0123456789abcdef";

  if (symbol == '"' || symbol == '\\') {
    result.push_back('\\');
    result.push_back(symbol);
  } else if (symbol == '\n') {
    result += "\\n";
  } else if (symbol == '\r') {
    result += "\\r";
  } else if (symbol == '\t') {
    result += "\\t";
  } else if (static_cast<unsigned char>(symbol) < 0x20U) {
    result += "\\u00";
    result.push_back(HEX[(static_cast<unsigned char>(symbol) >> 4U) & 0xFU]);
    result.push_back(HEX[static_cast<unsigned char>(symbol) & 0xFU]);
  } else {
    result.push_back(symbol);
  }
}

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1
