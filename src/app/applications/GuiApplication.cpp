#include "src/app/applications/GuiApplication.h"

#include <cassert>

#include "src/log/log.h"

namespace app
{

int GuiApplication::perform(AppCtxPtr ctx)
{
  assert(ctx != nullptr);

  auto libctx = create_and_convert_libctx(ctx);

  if (libctx == nullptr) {
    return INVALID;
  }

  // the windows show no OpenCV ones, the training one shows the training
  // chart, saved on every iteration for it unless the cfg savechart= key
  // tells another cadence
  libctx->dont_show = true;
  libctx->dont_save_charts_every_iter = false;

  auto gui = create_gui_initer();

  assert(gui != nullptr);

  const bool shown = gui->run(ctx, libctx);

  gui->deinit();

  if (ctx->stop_signal.load() > 0) {
    return stopped_status(ctx);
  }

  if (!shown) {
    LOGE("The GUI ended with an error");
    return INVALID;
  }

  return 0;
}

}  // namespace app
