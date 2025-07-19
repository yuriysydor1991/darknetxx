#include "src/darknet-adaptor/DarknetController.h"

#include <cassert>
#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/darknet-adaptor/DarknetStrings.h"
#include "src/darknet-adaptor/adaptors/ADetector.h"
#include "src/log/log.h"

namespace darknet_adaptor
{

DarknetControllerPtr DarknetController::create()
{
  return std::make_shared<DarknetController>();
}

bool DarknetController::perform([[maybe_unused]] AppCtxPtr ctx)
{
  LOGD("Initing adaptors context");

  auto dctx = create_context(ctx);

  assert(ctx != nullptr);

  LOGD("creating appropriate worker");

  auto worker = create_appropriate_worker(dctx);

  assert(worker != nullptr);

  if (worker == nullptr) {
    LOGE("Failure whire creating appropriate worker");
    return false;
  }

  LOGD("Trying to perform command");

  if (!worker->perform(dctx)) {
    LOGE("Worker signalled error state");
    return false;
  }

  return true;
}

DarknetContextPtr DarknetController::create_context(AppCtxPtr actx)
{
  assert(actx != nullptr);

  LOGD("Creating the context");

  return std::make_shared<DarknetContext>(actx);
}

DarknetController::IDarknetAdaptorPtr
DarknetController::create_appropriate_worker(DarknetContextPtr dctx)
{
  assert(dctx != nullptr);

  if (dctx == nullptr) {
    LOGE("Invalid context pointer provided");
    return {};
  }

  if (dctx->command == DarknetStrings::detector) {
    return std::make_shared<adaptors::ADetector>();
  }

  LOGE("Can't create worker for the unknown command: " << dctx->command);

  return {};
}

}  // namespace darknet_adaptor