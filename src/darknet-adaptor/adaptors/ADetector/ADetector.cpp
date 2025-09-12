#include "src/darknet-adaptor/adaptors/ADetector/ADetector.h"

#include <cassert>
#include <memory>

#include "src/darknet-adaptor/adaptors/IDarknetAdaptor.h"
#include "src/detector/subs/tester/test_detector_ctx.h"
#include "src/log/log.h"

namespace darknet_adaptor::adaptors
{

bool ADetector::perform(DarknetContextPtr dctx)
{
  assert(dctx != nullptr);

  if (dctx == nullptr) {
    LOGE("Invalid context pointer provided");
    return false;
  }

  auto odctx = dctx->get_darknet_ctx();

  assert(odctx != nullptr);

  if (odctx == nullptr) {
    LOGE("Context returned invalid orig darknet training context");
    return false;
  }

  const int status = test_detector_ctx(odctx.get());

  if (status == 0) {
    LOGE("Detector trainer returned invalid status");
    return false;
  }

  return true;
}

ADetectorPtr ADetector::create() { return std::make_shared<ADetector>(); }

}  // namespace darknet_adaptor::adaptors
