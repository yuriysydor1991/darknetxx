#ifndef DARKNET_CXX_PORT_PROJECT_NETWORKS_REGISTRY_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NETWORKS_REGISTRY_CLASS_H

#include <map>
#include <memory>
#include <mutex>

#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The networks the darknet C code computes with, found by their
 * struct views.
 *
 * The C code passes the network struct by value, so the darknet C API
 * functions the C++ implements (resize_network(), free_network() and the
 * like) get a copy of the view only. Every copy keeps the layers array of
 * the adaptor, which tells the adaptor apart. The registry keeps:
 * - the networks the C++ owns (DarknetContext hands them to the
 *   computation), known for as long as they live;
 * - the networks the darknet C API made for the C code
 *   (parse_network_cfg() and the like), kept alive by the registry until the
 *   free_network() of their view.
 */
class NetworksRegistry
{
 public:
  /**
   * @brief Registers the network by it's current layers array, the owned
   * one is kept alive until release(). Registering it again changes the
   * ownership only.
   */
  static void add(const ANetworkAdaptorPtr& net, bool owned);

  /// @brief The adaptor of the network of the view, none for the unknown or
  /// the released one.
  static ANetworkAdaptorPtr find(const network& view);

  /// @brief Tells if the registry keeps the network of the view alive.
  static bool owns(const network& view);

  /**
   * @brief Forgets the network of the view, the owned one is released.
   *
   * @return False for the unknown network.
   */
  static bool remove(const network& view);

  /// @brief The count of the known networks, the released ones excluded.
  static size_t size();

 private:
  struct Entry
  {
    std::weak_ptr<ANetworkAdaptor> known;
    ANetworkAdaptorPtr owned;
  };

  using Entries = std::map<const layer*, Entry>;

  static std::mutex& mutex();
  static Entries& entries();
};

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_NETWORKS_REGISTRY_CLASS_H
