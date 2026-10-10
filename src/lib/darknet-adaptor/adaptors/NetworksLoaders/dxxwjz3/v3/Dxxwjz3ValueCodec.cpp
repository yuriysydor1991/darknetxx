#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3ValueCodec.h"

#include <charconv>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <system_error>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1FloatCodec.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1JsonString.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

using dxxwjz1::v1::Dxxwjz1FloatCodec;
using dxxwjz1::v1::Dxxwjz1JsonString;

std::string Dxxwjz3ValueCodec::json_of(const char* const value)
{
  if (value == nullptr) {
    return "null";
  }

  const std::string text{value};
  std::string items;
  std::size_t start = 0U;

  for (std::size_t comma = text.find(','); comma != std::string::npos;
       start = comma + 1U, comma = text.find(',', start)) {
    items += item_json(text.substr(start, comma - start)) + ",";
  }

  items += item_json(text.substr(start));

  return start == 0U ? items : "[" + items + "]";
}

bool Dxxwjz3ValueCodec::text_of(const json& value, std::string& text)
{
  text.clear();

  if (!value.is_array()) {
    return item_text(value, text) && !text.empty();
  }

  for (std::size_t index = 0U; index < value.size(); ++index) {
    std::string item;

    if (!item_text(value[index], item)) {
      return false;
    }

    text += (index == 0U ? "" : ",") + item;
  }

  return !text.empty();
}

std::string Dxxwjz3ValueCodec::item_json(const std::string& item)
{
  // the from_chars() reads the numbers of no plus sign
  const std::string digits =
      item.size() > 1U && item[0] == '+' ? item.substr(1U) : item;
  const char* const last = digits.data() + digits.size();
  int64_t whole = 0;
  float fraction = 0.f;
  std::string number;

  if (const auto [end, error] = std::from_chars(digits.data(), last, whole);
      error == std::errc{} && end == last) {
    number = std::to_string(whole);
  } else if (Dxxwjz1FloatCodec::decode_number(digits, fraction, '.') &&
             std::isfinite(fraction)) {
    number = fraction_text(fraction);
  }

  // the atof() of the number is the float of the item, the atoi() of the
  // integer part is the one to check
  const bool same =
      !number.empty() && std::strtoll(number.c_str(), nullptr, 10) ==
                             std::strtoll(item.c_str(), nullptr, 10);

  return same ? number : Dxxwjz1JsonString::quote(item);
}

bool Dxxwjz3ValueCodec::item_text(const json& item, std::string& text)
{
  // the signed and the unsigned integers
  if (item.is_number_integer()) {
    text = item.dump();
    return true;
  }

  if (item.is_number_float()) {
    text = fraction_text(static_cast<float>(item.get<double>()));
    return true;
  }

  if (!item.is_string()) {
    return false;
  }

  text = item.get<std::string>();

  return text.find_first_of(" \t\r\n") == std::string::npos;
}

std::string Dxxwjz3ValueCodec::fraction_text(const float value)
{
  // the 39 digits of the greatest float, the 45 decimals of the least one
  char text[64];
  char* const end =
      std::to_chars(text, text + sizeof(text), value, std::chars_format::fixed)
          .ptr;

  return std::string{text, end};
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
