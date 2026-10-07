#include "src/lib/darknet-adaptor/adaptors/NetworksCApi/NetworksRegistry.h"

#include <map>
#include <memory>
#include <mutex>

#include "src/lib/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"

namespace darknet_adaptor::adaptors
{

std::mutex& NetworksRegistry::mutex()
{
  static std::mutex registry_mutex;
  return registry_mutex;
}

NetworksRegistry::Entries& NetworksRegistry::entries()
{
  static Entries registry_entries;
  return registry_entries;
}

void NetworksRegistry::add(const ANetworkAdaptorPtr& net, bool owned)
{
  if (!net) {
    return;
  }

  const layer* const key = net->get()->layers;

  std::lock_guard<std::mutex> lock{mutex()};

  Entries& known = entries();

  // the dead entries keep no memory but the key, drop them on the way
  for (auto entry = known.begin(); entry != known.end();) {
    if (entry->second.known.expired()) {
      entry = known.erase(entry);
    } else {
      ++entry;
    }
  }

  Entry& entry = known[key];
  entry.known = net;
  entry.owned = owned ? net : ANetworkAdaptorPtr{};
}

ANetworkAdaptorPtr NetworksRegistry::find(const network& view)
{
  std::lock_guard<std::mutex> lock{mutex()};

  const auto found = entries().find(view.layers);

  if (found == entries().end()) {
    return {};
  }

  return found->second.known.lock();
}

bool NetworksRegistry::owns(const network& view)
{
  std::lock_guard<std::mutex> lock{mutex()};

  const auto found = entries().find(view.layers);

  return found != entries().end() && found->second.owned != nullptr;
}

bool NetworksRegistry::remove(const network& view)
{
  ANetworkAdaptorPtr released;

  {
    std::lock_guard<std::mutex> lock{mutex()};

    const auto found = entries().find(view.layers);

    if (found == entries().end()) {
      return false;
    }

    const bool alive = !found->second.known.expired();

    // the network is released outside the lock
    released = std::move(found->second.owned);
    entries().erase(found);

    if (!alive) {
      return false;
    }
  }

  return true;
}

size_t NetworksRegistry::size()
{
  std::lock_guard<std::mutex> lock{mutex()};

  size_t count = 0U;

  for (const auto& [key, entry] : entries()) {
    (void)key;
    if (!entry.known.expired()) ++count;
  }

  return count;
}

}  // namespace darknet_adaptor::adaptors
