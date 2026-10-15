#include "src/gui/net-editor/StepsOption.h"

#include <algorithm>

namespace net_editor
{

StepsOption::StepsOption(std::string nabout)
    : NumberOption{"steps", "", std::move(nabout), true, true}
{
}

bool StepsOption::required(const OptionContext& ctx) const
{
  const std::string* policy = ctx.section.find("policy");

  return policy != nullptr && *policy == "steps";
}

NumberOption::Range StepsOption::range(const OptionContext& ctx) const
{
  return {0.0, ctx.section.integer("max_batches", 0)};
}

std::string StepsOption::rule(const OptionContext& ctx) const
{
  return "ascending, as many as the scales (" +
         std::to_string(ctx.section.count("scales")) + ")";
}

std::string StepsOption::check(const std::vector<double>& values,
                               const OptionContext& ctx) const
{
  if (!std::is_sorted(values.begin(), values.end())) {
    return "The steps aren't ascending";
  }

  return values.size() == ctx.section.count("scales")
             ? std::string{}
             : "The steps aren't as many as the scales (" +
                   std::to_string(ctx.section.count("scales")) + ")";
}

}  // namespace net_editor
