#include "src/NetworksLoaders/network-weights/NetworkWeightsConverter.h"

#include <memory>
#include <string>

#include "src/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/NetworksLoaders/network-weights/NetworkWeights.h"

namespace darknet_adaptor::loaders
{

NetworkWeightsConverterPtr NetworkWeightsConverter::create()
{
  return std::make_shared<NetworkWeightsConverter>();
}

dxxdd::INetworkWeightsPtr NetworkWeightsConverter::create_network(
    const std::string& cfg)
{
  // the single image batch network of the least memory: the arrays of the
  // weights don't depend on the batch
  return NetworkWeights::create(
      OrigDefaultLoader{}.parse_network_cfg_custom(cfg, 1, 1));
}

}  // namespace darknet_adaptor::loaders
