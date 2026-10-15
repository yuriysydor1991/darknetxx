#include "src/gui/net-editor/ScalesOption.h"

#include "src/gui/net-editor/StepsOption.h"

namespace net_editor
{

ScalesOption::ScalesOption(std::string nabout)
    : NumberOption{"scales", "", std::move(nabout), false, true}
{
}

bool ScalesOption::required(const OptionContext& ctx) const
{
  return StepsOption{""}.required(ctx);
}

std::string ScalesOption::rule(const OptionContext& ctx) const
{
  return "as many as the steps (" + std::to_string(ctx.section.count("steps")) +
         ")";
}

std::string ScalesOption::check(const std::vector<double>& values,
                                const OptionContext& ctx) const
{
  return values.size() == ctx.section.count("steps")
             ? std::string{}
             : "The scales aren't as many as the steps (" +
                   std::to_string(ctx.section.count("steps")) + ")";
}

}  // namespace net_editor
