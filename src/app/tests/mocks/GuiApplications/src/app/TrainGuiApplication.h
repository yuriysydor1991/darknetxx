#ifndef DARKNETXX_PROJECT_TRAINGUIAPPLICATION_CLASS_H
#define DARKNETXX_PROJECT_TRAINGUIAPPLICATION_CLASS_H

#include <gmock/gmock.h>

#include <functional>
#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/IApplication.h"

namespace app
{

class TrainGuiApplication : public IApplication
{
 public:
  virtual ~TrainGuiApplication() = default;
  TrainGuiApplication()
  {
    if (onMockCreate) {
      onMockCreate(*this);
    }
  }

  inline static std::function<void(TrainGuiApplication&)> onMockCreate;

  MOCK_METHOD(int, run, (std::shared_ptr<ApplicationContext> ctx), (override));
};

}  // namespace app

#endif  // DARKNETXX_PROJECT_TRAINGUIAPPLICATION_CLASS_H
