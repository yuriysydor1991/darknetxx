#include "src/darknet-adaptor/DarknetController.h"

#include <cassert>
#include <memory>

#include "src/darknet-adaptor/DarknetStrings.h"
#include "src/darknet-adaptor/adaptors/ADetector/ADetector.h"
#include "src/darknet-adaptor/adaptors/ADetectorTrainer/ADetectorTrainer.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/NetworkSource.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/facade/public/LibraryContext.h"
#include "src/log/log.h"

namespace darknet_adaptor
{

DarknetControllerPtr DarknetController::create()
{
  return std::make_shared<DarknetController>();
}

bool DarknetController::perform([[maybe_unused]] LibCtxPtr ctx)
{
  LOGD("Initing adaptors context");

  auto dctx = create_context(ctx);

  assert(ctx != nullptr);

  LOGD("creating appropriate worker");

  auto worker = create_appropriate_worker(dctx);

  // no worker is a user error (neither --train-cfg nor --detect-cfg given)
  if (worker == nullptr) {
    LOGE(
        "Failure while creating appropriate worker: provide the "
        "--train-cfg or the --detect-cfg parameter, or the dxxwjz2 or "
        "dxxwjz3 --weights file alone");
    return false;
  }

  take_network_file_weights(dctx);

  LOGD("Trying to perform command");

  if (!worker->perform(dctx)) {
    LOGE("Worker signalled error state");
    return false;
  }

  return true;
}

DarknetContextPtr DarknetController::create_context(LibCtxPtr lctx)
{
  assert(lctx != nullptr);

  LOGD("Creating the context");

  auto ctx = std::make_shared<DarknetContext>(lctx);

  ctx->init();

  return ctx;
}

DarknetController::IDarknetAdaptorPtr
DarknetController::create_appropriate_worker(DarknetContextPtr dctx)
{
  assert(dctx != nullptr);

  if (dctx == nullptr) {
    LOGE("Invalid context pointer provided");
    return {};
  }

  if (!dctx->get_Detect_cfg().empty()) {
    LOGD("Found the detect cfg file: " << dctx->get_Detect_cfg());
    return adaptors::ADetector::create();
  }

  if (!dctx->get_Train_cfg().empty()) {
    LOGD("Found the train cfg file: " << dctx->get_Train_cfg());
    return adaptors::ADetectorTrainer::create();
  }

  return create_network_file_worker(dctx);
}

DarknetController::IDarknetAdaptorPtr
DarknetController::create_network_file_worker(DarknetContextPtr dctx)
{
  const std::string weights = dctx->get_Latest_weights();

  if (weights.empty() || !loaders::NetworkSource::describes(weights)) {
    LOGE("No other adaptors so far!");
    return {};
  }

  loaders::WeightsDocument rest;
  bool held = false;

  if (!loaders::WeightsFileFormats::of_file(weights)->create_reader()->peek(
          weights, rest, held)) {
    return {};
  }

  if (held) {
    LOGI("Detecting with the network of the " << weights << " file");
    dctx->set_Detect_cfg(weights);
    return adaptors::ADetector::create();
  }

  LOGI("The " << weights << " file holds no weights, training it's network");
  dctx->set_Train_cfg(weights);

  return adaptors::ADetectorTrainer::create();
}

void DarknetController::take_network_file_weights(const DarknetContextPtr& dctx)
{
  const std::string& cfg = dctx->get_Detect_cfg().empty()
                               ? dctx->get_Train_cfg()
                               : dctx->get_Detect_cfg();

  if (dctx->get_Latest_weights().empty() &&
      loaders::NetworkSource::describes(cfg)) {
    dctx->set_Latest_weights(cfg);
  }
}

}  // namespace darknet_adaptor
