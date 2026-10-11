#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConversion.h"

#include "src/log/log.h"

namespace darknet_adaptor::loaders
{

bool WeightsConversion::stopped() const
{
  if (stop_requested && stop_requested()) {
    LOGW("The conversion into " << dst << " is stopped");
    return true;
  }

  return false;
}

}  // namespace darknet_adaptor::loaders
