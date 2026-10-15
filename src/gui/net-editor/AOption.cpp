#include "src/gui/net-editor/AOption.h"

namespace net_editor
{

AOption::AOption(std::string nkey, std::string ndef, std::string nabout)
    : mkey{std::move(nkey)}, mdef{std::move(ndef)}, mabout{std::move(nabout)}
{
}

const std::string& AOption::key() const { return mkey; }

const std::string& AOption::def() const { return mdef; }

const std::string& AOption::about() const { return mabout; }

std::vector<std::string> AOption::choices() const { return {}; }

bool AOption::typable(const std::string& text) const
{
  (void)text;
  return true;
}

bool AOption::required(const OptionContext& ctx) const
{
  (void)ctx;
  return false;
}

}  // namespace net_editor
