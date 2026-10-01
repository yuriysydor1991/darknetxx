#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONVERTERCOMPONENT_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONVERTERCOMPONENT_CLASS_H

#include <gmock/gmock.h>

#include <functional>
#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/IApplication.h"
#include "src/app/converter/IFormatsConverter.h"

namespace app::converter
{

class ApplicationConverterComponent : public IApplication
{
 public:
  virtual ~ApplicationConverterComponent() = default;

  explicit ApplicationConverterComponent(const IFormatsConverterPtr& nconverter)
      : converter{nconverter}
  {
    if (onMockCreate) {
      onMockCreate(*this);
    }
  }

  inline static std::function<void(ApplicationConverterComponent&)>
      onMockCreate;

  MOCK_METHOD(int, run, (std::shared_ptr<ApplicationContext> ctx), (override));

  /// @brief The converter the factory handed over.
  IFormatsConverterPtr converter;
};

}  // namespace app::converter

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONVERTERCOMPONENT_CLASS_H
