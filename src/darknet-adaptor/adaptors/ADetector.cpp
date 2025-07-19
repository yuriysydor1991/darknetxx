#include "src/darknet-adaptor/adaptors/ADetector.h"

#include <memory>
#include <cassert>

#include "src/darknet-adaptor/adaptors/IDarknetAdaptor.h"
#include "src/log/log.h"

namespace darknet_adaptor::adaptors
{

bool ADetector::perform([[maybe_unused]] DarknetContextPtr dctx)
{
  assert(dctx != nullptr);

  if (dctx == nullptr) {
    LOGE("Invalid context pointer provided");
    return false;
  }

  LOGE("implement the detector adaptor worker!");

  return true;
}

using ADetectorPtr = ADetector::ADetectorPtr;

}  // namespace darknet_adaptor::adaptors
