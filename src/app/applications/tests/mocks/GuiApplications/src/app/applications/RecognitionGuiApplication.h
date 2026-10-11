#ifndef DARKNETXX_PROJECT_RECOGNITIONGUIAPPLICATION_CLASS_H
#define DARKNETXX_PROJECT_RECOGNITIONGUIAPPLICATION_CLASS_H

#include <gmock/gmock.h>

#include <functional>
#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/applications/IApplication.h"

namespace app
{

class RecognitionGuiApplication : public IApplication
{
 public:
  virtual ~RecognitionGuiApplication() = default;
  RecognitionGuiApplication()
  {
    if (onMockCreate) {
      onMockCreate(*this);
    }
  }

  inline static std::function<void(RecognitionGuiApplication&)> onMockCreate;

  MOCK_METHOD(int, run, (std::shared_ptr<ApplicationContext> ctx), (override));
};

}  // namespace app

#endif  // DARKNETXX_PROJECT_RECOGNITIONGUIAPPLICATION_CLASS_H
