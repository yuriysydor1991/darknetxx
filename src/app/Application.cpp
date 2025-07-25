#include "src/app/Application.h"

#include <cassert>
#include <iostream>
#include <memory>

#include "src/darknet-adaptor/DarknetController.h"
#include "src/log/log.h"

namespace app
{

int Application::run(std::shared_ptr<ApplicationContext> ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid context pointer provided");
    return INVALID;
  }

  auto darknet = darknet_adaptor::DarknetController::create();

  assert(darknet != nullptr);

  if (!darknet->perform(ctx)) {
    LOGE("Fail to init the darknet");
    return INVALID;
  }

  return 0;
}

}  // namespace app
