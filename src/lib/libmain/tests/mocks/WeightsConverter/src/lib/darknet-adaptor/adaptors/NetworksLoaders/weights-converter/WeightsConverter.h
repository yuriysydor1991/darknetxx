#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERTER_CLASS_H

#include <gmock/gmock.h>

#include <functional>
#include <memory>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders
{

struct WeightsConversion
{
  IWeightsFileFormatPtr from;
  std::string src;
  IWeightsFileFormatPtr to;
  std::string dst;
  std::string cfg;
  std::string names;
  std::function<bool()> stop_requested;
};

class WeightsConverter
{
 public:
  using WeightsConverterPtr = std::shared_ptr<WeightsConverter>;

  virtual ~WeightsConverter() = default;

  WeightsConverter()
  {
    if (onMockCreate) {
      onMockCreate(*this);
    }
  }

  inline static std::function<void(WeightsConverter&)> onMockCreate;

  static WeightsConverterPtr create()
  {
    return std::make_shared<WeightsConverter>();
  }

  MOCK_METHOD(bool, convert, (const WeightsConversion& conversion));
};

using WeightsConverterPtr = WeightsConverter::WeightsConverterPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERTER_CLASS_H
