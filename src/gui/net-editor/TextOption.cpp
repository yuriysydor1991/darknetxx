#include "src/gui/net-editor/TextOption.h"

#include <algorithm>
#include <cctype>

namespace net_editor
{

bool TextOption::typable(const std::string& text) const
{
  return std::none_of(text.begin(), text.end(), [](unsigned char symbol) {
    return std::isspace(symbol) != 0;
  });
}

std::string TextOption::limits(const OptionContext& ctx) const
{
  (void)ctx;
  return "any text of no white space";
}

std::string TextOption::error(const std::string& value,
                              const OptionContext& ctx) const
{
  (void)ctx;
  return typable(value) ? std::string{} : "The darknet drops the white space";
}

}  // namespace net_editor
