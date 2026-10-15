#include "src/gui/net-editor/HeadClassesOption.h"

namespace net_editor
{

HeadClassesOption::HeadClassesOption(std::string ndef, std::string nabout)
    : NumberOption{"classes", std::move(ndef), std::move(nabout),
                   true,      false,           {1.0, NONE}}
{
}

std::string HeadClassesOption::rule(const OptionContext& ctx) const
{
  const std::size_t names = ctx.model.classes.size();

  return names == 0U
             ? std::string{}
             : "the " + std::to_string(names) + " class names of the network";
}

std::string HeadClassesOption::check(const std::vector<double>& values,
                                     const OptionContext& ctx) const
{
  const std::size_t names = ctx.model.classes.size();

  return names == 0U || static_cast<std::size_t>(values.front()) == names
             ? std::string{}
             : "The network names " + std::to_string(names) + " classes";
}

}  // namespace net_editor
