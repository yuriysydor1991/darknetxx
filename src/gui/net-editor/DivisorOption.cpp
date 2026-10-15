#include "src/gui/net-editor/DivisorOption.h"

#include <algorithm>

namespace net_editor
{

DivisorOption::DivisorOption(std::string nkey, std::string ndef,
                             std::string nabout, std::string ndividend,
                             const int ndividendDef)
    : NumberOption{std::move(nkey), std::move(ndef), std::move(nabout), true,
                   false},
      mdividend{std::move(ndividend)},
      mdividendDef{ndividendDef}
{
}

NumberOption::Range DivisorOption::range(const OptionContext& ctx) const
{
  return {1.0, std::max(1, dividend(ctx))};
}

std::string DivisorOption::rule(const OptionContext& ctx) const
{
  return "a divisor of the " + mdividend + "=" + std::to_string(dividend(ctx));
}

std::string DivisorOption::check(const std::vector<double>& values,
                                 const OptionContext& ctx) const
{
  const int divisor = static_cast<int>(values.front());

  return dividend(ctx) % divisor == 0
             ? std::string{}
             : "The " + std::to_string(divisor) + " doesn't divide the " +
                   mdividend + "=" + std::to_string(dividend(ctx));
}

int DivisorOption::dividend(const OptionContext& ctx) const
{
  return ctx.section.integer(mdividend, mdividendDef);
}

}  // namespace net_editor
