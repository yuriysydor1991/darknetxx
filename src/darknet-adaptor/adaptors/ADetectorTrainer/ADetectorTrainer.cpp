#include "src/darknet-adaptor/adaptors/ADetectorTrainer/ADetectorTrainer.h"

#include <cassert>
#include <memory>

#include "src/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
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

  // the networks the C code trains: their memory stays on the C++ side
  auto loader = create_loader();

  if (loader == nullptr || !loader->load(dctx)) {
    LOGE("Failure to load the networks to train");
    dctx->release_networks();
    return false;
  }

  const int status = train_detector_ctx(odctx.get());

  dctx->release_networks();

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

loaders::INetworkLoaderPtr ADetectorTrainer::create_loader()
{
  return loaders::OrigDefaultLoader::create();
}

}  // namespace darknet_adaptor::adaptors
