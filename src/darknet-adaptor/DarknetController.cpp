#include "src/darknet-adaptor/DarknetController.h"

#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/log/log.h"

namespace darknet_adaptor
{

bool DarknetController::init([[maybe_unused]] AppCtxPtr ctx)
{
  return true;
}

}  // namespace darknet_adaptor