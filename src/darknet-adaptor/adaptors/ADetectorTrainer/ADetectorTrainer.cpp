#include "src/darknet-adaptor/adaptors/ADetectorTrainer/ADetectorTrainer.h"

#include <cassert>
#include <memory>

#include "src/darknet-adaptor/adaptors/IDarknetAdaptor.h"
#include "src/detector/subs/trainer/train_detector_ctx.h"
#include "src/log/log.h"

namespace darknet_adaptor::adaptors
{

bool ADetectorTrainer::perform(DarknetContextPtr dctx)
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

  const int status = train_detector_ctx(odctx.get());

  if (status == 0) {
    LOGE("Detector trainer returned invalid status");
    return false;
  }

  return true;
}

ADetectorTrainer::ADetectorTrainerPtr ADetectorTrainer::create()
{
  return std::make_shared<ADetectorTrainer>();
}

}  // namespace darknet_adaptor::adaptors
