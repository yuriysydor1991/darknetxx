#include "src/gui/net-editor/AnchorsOption.h"

namespace net_editor
{

AnchorsOption::AnchorsOption(std::string nabout)
    : NumberOption{"anchors", "", std::move(nabout), false, true, {0.0, NONE}}
{
}

std::string AnchorsOption::rule(const OptionContext& ctx) const
{
  return "2 * num = " + std::to_string(2 * ctx.section.integer("num", 1)) +
         " values";
}

std::string AnchorsOption::check(const std::vector<double>& values,
                                 const OptionContext& ctx) const
{
  const auto count =
      static_cast<std::size_t>(2 * ctx.section.integer("num", 1));

  return values.size() == count
             ? std::string{}
             : "The anchors aren't the 2 * num = " + std::to_string(count) +
                   " values";
}

}  // namespace net_editor
