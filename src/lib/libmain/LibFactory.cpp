#include "src/lib/libmain/LibFactory.h"

#include <cassert>

#include "src/lib/libmain/LibMain.h"
#include "src/lib/libmain/WeightsConversionLib.h"
#include "src/lib/recognition/NetworkRecognizer.h"
#include "src/lib/training/BackgroundTrainer.h"
#include "src/log/log.h"

namespace darknetxx_impl
{

LibFactory::LibFactory()
{
  // once, before the very first library work
  [[maybe_unused]] static const bool loggerReady = []() {
    LOG_INIT_DEFAULTS_IF_OWN();
    return true;
  }();
}

LibFactory::LibraryContextPtr LibFactory::create_default_context()
{
  return std::make_shared<darknetxx011::LibraryContext>();
}

LibFactory::ILibPtr LibFactory::create_library(LibraryContextPtr ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid library context provided");
    return {};
  }

  if (ctx->conversion_requested()) {
    return create_conversion_lib();
  }

  // the weights file alone may keep it's network (the dxxwjz2 and dxxwjz3
  // ones)
  if (!ctx->detect_cfg.empty() || !ctx->train_cfg.empty() ||
      !ctx->latest_weights.empty()) {
    return create_darknet_lib();
  }

  LOGE(
      "The context asks for no work: give it the training or the detection "
      "network cfg file, the dxxwjz2 or dxxwjz3 weights file or the network "
      "weights file conversion");

  return {};
}

LibFactory::ITrainerPtr LibFactory::create_trainer(LibraryContextPtr ctx)
{
  assert(ctx != nullptr);

  return std::make_shared<BackgroundTrainer>(ctx, create_darknet_lib());
}

LibFactory::IRecognizerPtr LibFactory::create_recognizer()
{
  return std::make_shared<NetworkRecognizer>();
}

LibFactoryPtr LibFactory::create_factory()
{
  return std::make_shared<LibFactory>();
}

LibFactory::ILibPtr LibFactory::create_conversion_lib()
{
  return std::make_shared<WeightsConversionLib>();
}

LibFactory::ILibPtr LibFactory::create_darknet_lib()
{
  return std::make_shared<LibMain>();
}

}  // namespace darknetxx_impl
