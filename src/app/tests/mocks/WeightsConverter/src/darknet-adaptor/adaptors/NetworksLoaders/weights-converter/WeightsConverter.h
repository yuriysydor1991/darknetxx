#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERTER_CLASS_H

#include <gmock/gmock.h>

#include <functional>
#include <memory>
#include <string>

namespace darknet_adaptor::loaders
{

enum class WeightsFormat
{
  Original,
  Dxxwjz1
};

struct WeightsConversion
{
  WeightsFormat from{WeightsFormat::Original};
  std::string src;
  WeightsFormat to{WeightsFormat::Dxxwjz1};
  std::string dst;
  std::string cfg;
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
