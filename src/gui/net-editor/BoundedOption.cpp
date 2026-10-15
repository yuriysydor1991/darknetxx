#include "src/gui/net-editor/BoundedOption.h"

namespace net_editor
{

BoundedOption::BoundedOption(std::string nkey, std::string ndef,
                             std::string nabout, const bool nlist,
                             const int nlowest, std::string nbound,
                             const int nboundDef, const bool nbelow)
    : NumberOption{std::move(nkey), std::move(ndef), std::move(nabout), true,
                   nlist},
      mlowest{nlowest},
      mbound{std::move(nbound)},
      mboundDef{nboundDef},
      mbelow{nbelow}
{
}

NumberOption::Range BoundedOption::range(const OptionContext& ctx) const
{
  return {mlowest, ctx.section.integer(mbound, mboundDef) - (mbelow ? 1 : 0)};
}

std::string BoundedOption::rule(const OptionContext& ctx) const
{
  return std::string{mbelow ? "below" : "up to"} + " the " + mbound + "=" +
         std::to_string(ctx.section.integer(mbound, mboundDef));
}

}  // namespace net_editor
