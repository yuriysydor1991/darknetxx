#include "src/darknet-adaptor/DarknetController.h"

#include <memory>

namespace darknet_adaptor
{

DarknetController::DarknetControllerPtr DarknetController::create()
{
  return std::make_shared<DarknetController>();
}

}  // namespace darknet_adaptor
