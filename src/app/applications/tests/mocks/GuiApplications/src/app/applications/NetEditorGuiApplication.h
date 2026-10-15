#ifndef DARKNETXX_PROJECT_NETEDITORGUIAPPLICATION_CLASS_H
#define DARKNETXX_PROJECT_NETEDITORGUIAPPLICATION_CLASS_H

#include <gmock/gmock.h>

#include <functional>
#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/applications/IApplication.h"

namespace app
{

class NetEditorGuiApplication : public IApplication
{
 public:
  virtual ~NetEditorGuiApplication() = default;
  NetEditorGuiApplication()
  {
    if (onMockCreate) {
      onMockCreate(*this);
    }
  }

  inline static std::function<void(NetEditorGuiApplication&)> onMockCreate;

  MOCK_METHOD(int, run, (std::shared_ptr<ApplicationContext> ctx), (override));
};

}  // namespace app

#endif  // DARKNETXX_PROJECT_NETEDITORGUIAPPLICATION_CLASS_H
