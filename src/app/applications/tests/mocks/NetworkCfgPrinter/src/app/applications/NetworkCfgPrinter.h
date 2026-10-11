#ifndef DARKNETXX_PROJECT_NETWORKCFGPRINTER_CLASS_H
#define DARKNETXX_PROJECT_NETWORKCFGPRINTER_CLASS_H

#include <gmock/gmock.h>

#include <functional>
#include <memory>

#include "src/app/applications/IApplication.h"

namespace app
{

class NetworkCfgPrinter : public IApplication
{
 public:
  virtual ~NetworkCfgPrinter() = default;

  NetworkCfgPrinter()
  {
    if (onMockCreate) {
      onMockCreate(*this);
    }
  }

  inline static std::function<void(NetworkCfgPrinter&)> onMockCreate;

  MOCK_METHOD(int, run, (std::shared_ptr<ApplicationContext> ctx), (override));
};

}  // namespace app

#endif  // DARKNETXX_PROJECT_NETWORKCFGPRINTER_CLASS_H
