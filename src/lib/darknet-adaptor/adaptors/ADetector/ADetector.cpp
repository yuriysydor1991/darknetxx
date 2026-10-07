#include "src/lib/darknet-adaptor/adaptors/ADetector/ADetector.h"

#include <cassert>
#include <memory>

#include "src/detector/subs/tester/test_detector_ctx.h"
#include "src/lib/darknet-adaptor/adaptors/IDarknetAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
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

  // Without the image, the cfg or the weights the detection reports them
  // missing first, so the network is loaded for the complete parameters
  // only. It's memory stays on the C++ side.
  if (odctx->filename != nullptr && odctx->cfg != nullptr &&
      odctx->weights != nullptr) {
    auto loader = create_loader();

    if (loader == nullptr || !loader->load(dctx)) {
      LOGE("Failure to load the detection network");
      dctx->release_networks();
      return false;
    }
  }

  const int status = test_detector_ctx(odctx.get());

  dctx->release_networks();

  if (status == 0) {
    if (dctx->get_stop()) {
      LOGW("The detection is stopped by request");
      return false;
    }

    LOGE("Detector returned invalid status");
    return false;
  }

  return true;
}

ADetectorPtr ADetector::create() { return std::make_shared<ADetector>(); }

loaders::INetworkLoaderPtr ADetector::create_loader()
{
  return loaders::OrigDefaultLoader::create();
}

}  // namespace darknet_adaptor::adaptors
