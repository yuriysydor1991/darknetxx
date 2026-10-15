#include "src/gui/net-editor/SectionSchema.h"

#include <algorithm>

namespace net_editor
{

SectionSchema::SectionSchema(std::string nabout,
                             std::vector<AOptionPtr> noptions,
                             NetworkSection::Options ninitial,
                             std::vector<AOptionPtr> nextras)
    : mabout{std::move(nabout)},
      moptions{std::move(noptions)},
      minitial{std::move(ninitial)},
      mextras{std::move(nextras)}
{
}

const std::string& SectionSchema::about() const { return mabout; }

const std::vector<AOptionPtr>& SectionSchema::options() const
{
  return moptions;
}

const NetworkSection::Options& SectionSchema::initial() const
{
  return minitial;
}

AOptionPtr SectionSchema::find(const std::string& key) const
{
  for (const auto* options : {&moptions, &mextras}) {
    const auto found = std::find_if(
        options->begin(), options->end(),
        [&key](const AOptionPtr& one) { return one->key() == key; });

    if (found != options->end()) {
      return *found;
    }
  }

  return nullptr;
}

}  // namespace net_editor
