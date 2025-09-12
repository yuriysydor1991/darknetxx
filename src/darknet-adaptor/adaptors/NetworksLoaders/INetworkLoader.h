#ifndef DARKNET_CXX_PORT_PROJECT_I_NETWORK_LOADER_ABSTRACT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_I_NETWORK_LOADER_ABSTRACT_CLASS_H

#include <memory>

#include "src/darknet-adaptor/DarknetContext.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The NN loader abstract class.
 */
class INetworkLoader
{
 public:
  using INetworkLoaderPtr = std::shared_ptr<INetworkLoader>;

  virtual ~INetworkLoader() = default;
  INetworkLoader() = default;

  virtual bool load(DarknetContextPtr dctx) = 0;
};

using INetworkLoaderPtr = INetworkLoader::INetworkLoaderPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_I_NETWORK_LOADER_ABSTRACT_CLASS_H
