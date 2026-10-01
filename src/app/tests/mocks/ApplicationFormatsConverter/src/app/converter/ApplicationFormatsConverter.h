#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONFORMATSCONVERTER_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONFORMATSCONVERTER_CLASS_H

#include <gmock/gmock.h>

#include <functional>
#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/converter/IFormatsConverter.h"

namespace app::converter
{

class ApplicationFormatsConverter : public IFormatsConverter
{
 public:
  virtual ~ApplicationFormatsConverter() = default;

  ApplicationFormatsConverter()
  {
    if (onMockCreate) {
      onMockCreate(*this);
    }
  }

  inline static std::function<void(ApplicationFormatsConverter&)> onMockCreate;

  MOCK_METHOD(bool, convert, (std::shared_ptr<ApplicationContext> ctx),
              (override));
};

}  // namespace app::converter

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONFORMATSCONVERTER_CLASS_H
