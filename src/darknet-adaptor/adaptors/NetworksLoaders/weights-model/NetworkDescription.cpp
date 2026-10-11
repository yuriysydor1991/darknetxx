#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkDescription.h"

#include <string>
#include <utility>
#include <vector>

namespace darknet_adaptor::loaders
{

NetworkDescription::NetworkDescription(std::string ncfg,
                                       std::vector<std::string> nclasses)
    : mcfg{std::move(ncfg)}, mclasses{std::move(nclasses)}
{
}

const std::string& NetworkDescription::cfg() const { return mcfg; }

void NetworkDescription::cfg(std::string ncfg) { mcfg = std::move(ncfg); }

const std::vector<std::string>& NetworkDescription::classes() const
{
  return mclasses;
}

void NetworkDescription::classes(std::vector<std::string> nclasses)
{
  mclasses = std::move(nclasses);
}

bool NetworkDescription::empty() const { return mcfg.empty(); }

}  // namespace darknet_adaptor::loaders
