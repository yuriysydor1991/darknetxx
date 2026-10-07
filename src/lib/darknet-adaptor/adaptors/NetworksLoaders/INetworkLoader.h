#ifndef DARKNET_CXX_PORT_PROJECT_I_NETWORK_LOADER_ABSTRACT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_I_NETWORK_LOADER_ABSTRACT_CLASS_H

#include <memory>

#include "src/lib/darknet-adaptor/DarknetContext.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The NN loader abstract class.
 *
 * The loader makes the networks the work of the context computes with and
 * hands them over to the context (the DarknetContext::set_train_networks()
 * and the like). The networks memory stays on the C++ side, the darknet C
 * code computes with their views.
 */
class INetworkLoader
{
 public:
  using INetworkLoaderPtr = std::shared_ptr<INetworkLoader>;

  virtual ~INetworkLoader() = default;
  INetworkLoader() = default;

  /// @brief Loads the networks of the context work, false for the failure.
  virtual bool load(DarknetContextPtr dctx) = 0;
};

using INetworkLoaderPtr = INetworkLoader::INetworkLoaderPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_I_NETWORK_LOADER_ABSTRACT_CLASS_H
