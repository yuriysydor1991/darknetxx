#include "src/app/GuiApplication.h"

#include <cassert>

#include "src/gui/gtkmm4/GtkmmIniter.h"
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

  // the window shows the training chart, saved on every iteration for it
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

std::shared_ptr<gui::IGUIIniter> GuiApplication::create_gui_initer()
{
  return std::make_shared<darknetxxGtkmm4::GtkmmIniter>();
}

}  // namespace app
