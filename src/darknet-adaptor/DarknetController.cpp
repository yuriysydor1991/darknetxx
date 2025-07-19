#include "src/darknet-adaptor/DarknetController.h"

#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/log/log.h"

namespace darknet_adaptor
{

DarknetControllerPtr DarknetController::create()
{
  return std::make_shared<DarknetController>();
}

bool DarknetController::init([[maybe_unused]] AppCtxPtr ctx)
{
  LOGD("Initing the darknet");

  return true;
}

}  // namespace darknet_adaptor