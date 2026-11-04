#ifndef DARKNET_CXX_PORT_PROJECT_NETWORK_WEIGHTS_CONVERTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NETWORK_WEIGHTS_CONVERTER_CLASS_H

#include <gmock/gmock.h>

#include <functional>
#include <memory>

#include "src/NetworksLoaders/DataDriversAliases.h"

namespace darknet_adaptor::loaders
{

/// @brief The mock of the converter of the darknet networks the
/// WeightsConversionLib creates.
class NetworkWeightsConverter : public dxxdd::WeightsConverter
{
 public:
  using NetworkWeightsConverterPtr = std::shared_ptr<NetworkWeightsConverter>;

  ~NetworkWeightsConverter() override = default;

  NetworkWeightsConverter()
  {
    if (onMockCreate) {
      onMockCreate(*this);
    }
  }

  inline static std::function<void(NetworkWeightsConverter&)> onMockCreate;

  static NetworkWeightsConverterPtr create()
  {
    return std::make_shared<NetworkWeightsConverter>();
  }

  MOCK_METHOD(bool, convert, (const dxxdd::WeightsConversion& conversion),
              (override));
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_NETWORK_WEIGHTS_CONVERTER_CLASS_H
