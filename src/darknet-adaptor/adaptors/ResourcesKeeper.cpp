#include "src/darknet-adaptor/adaptors/ResourcesKeeper.h"

#include <algorithm>
#include <utility>

namespace darknet_adaptor::adaptors
{

void ResourcesKeeper::keep(Resource resource)
{
  resources.push_back(std::move(resource));
}

void ResourcesKeeper::release(const void* address)
{
  const auto kept =
      std::find_if(resources.begin(), resources.end(),
                   [address](const Resource& r) { return r.get() == address; });

  if (kept != resources.end()) {
    resources.erase(kept);
  }
}

}  // namespace darknet_adaptor::adaptors
