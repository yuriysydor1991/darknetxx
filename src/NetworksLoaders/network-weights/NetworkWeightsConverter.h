#ifndef DARKNET_CXX_PORT_PROJECT_NETWORK_WEIGHTS_CONVERTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NETWORK_WEIGHTS_CONVERTER_CLASS_H

#include <memory>
#include <string>

#include "src/NetworksLoaders/DataDriversAliases.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The converter of the network weights files of the
 * darknetxx-datadrivers library (see the dxxdd::WeightsConverter) of the
 * darknet networks: the files of no structure of their own (the original
 * darknet weights ones) are read by the darknet network of the cfg file the
 * OrigDefaultLoader makes, the rest is validated against it, the very network
 * the darknetxx trains and detects with. The darknetxxConverter binary of the
 * library reads them by the structure of the cfg the library makes itself (the
 * dxxdd::CfgNetworkStructure), which the darknetxx tests check is the layout
 * of the very same arrays.
 */
class NetworkWeightsConverter : public dxxdd::WeightsConverter
{
 public:
  using NetworkWeightsConverterPtr = std::shared_ptr<NetworkWeightsConverter>;

  ~NetworkWeightsConverter() override = default;
  NetworkWeightsConverter() = default;

  static NetworkWeightsConverterPtr create();

 protected:
  /// @brief The darknet network of the cfg file, throws on the failure.
  dxxdd::INetworkWeightsPtr create_network(const std::string& cfg) override;
};

using NetworkWeightsConverterPtr =
    NetworkWeightsConverter::NetworkWeightsConverterPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_NETWORK_WEIGHTS_CONVERTER_CLASS_H
