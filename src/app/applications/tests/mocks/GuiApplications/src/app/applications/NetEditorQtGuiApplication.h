#ifndef DARKNETXX_PROJECT_NETEDITORQTGUIAPPLICATION_CLASS_H
#define DARKNETXX_PROJECT_NETEDITORQTGUIAPPLICATION_CLASS_H

#include <gmock/gmock.h>

#include <functional>
#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/applications/IApplication.h"

namespace app
{

class NetEditorQtGuiApplication : public IApplication
{
 public:
  virtual ~NetEditorQtGuiApplication() = default;
  NetEditorQtGuiApplication()
  {
    if (onMockCreate) {
      onMockCreate(*this);
    }
  }

  inline static std::function<void(NetEditorQtGuiApplication&)> onMockCreate;

  MOCK_METHOD(int, run, (std::shared_ptr<ApplicationContext> ctx), (override));
};

}  // namespace app

#endif  // DARKNETXX_PROJECT_NETEDITORQTGUIAPPLICATION_CLASS_H
