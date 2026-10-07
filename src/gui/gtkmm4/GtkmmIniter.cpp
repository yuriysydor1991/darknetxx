#include "src/gui/gtkmm4/GtkmmIniter.h"

#include <glibmm/miscutils.h>
#include <gtkmm/application.h>

#include <algorithm>
#include <cassert>

#include "project-global-decls.h"
#include "src/gui/gtkmm4/MainWindowContext.h"
#include "src/log/log.h"

namespace darknetxxGtkmm4
{

bool GtkmmIniter::run(app::AppCtxPtr ctx,
                      darknetxx011::LibraryContextPtr libctx)
{
  assert(ctx != nullptr);
  assert(libctx != nullptr);

  if (ctx == nullptr || libctx == nullptr) {
    LOGE("No valid application or library context provided");
    return false;
  }

  mwctx = MainWindowContext::build_context(ctx, libctx);

  // the Wayland app id, which names the desktop entry and the icon
  Glib::set_prgname(project_decls::PROJECT_APPLICATION_ID);

  // every process trains it's own folder
  auto app = Gtk::Application::create(project_decls::PROJECT_APPLICATION_ID,
                                      Gio::Application::Flags::NON_UNIQUE);

  bool shown = false;

  app->signal_activate().connect([this, &app, &shown]() {
    shown = show_window(*app);

    if (!shown) {
      app->quit();
    }
  });

  // the command line options are the application ones, not the toolkit ones
  const int status = app->run(std::min(ctx->argc, 1), ctx->argv);

  return status == 0 && shown;
}

void GtkmmIniter::deinit()
{
  if (mwctx != nullptr) {
    mwctx->clear();
    mwctx.reset();
  }
}

bool GtkmmIniter::show_window(Gtk::Application& app)
{
  if (!mwctx->wloader->load()) {
    LOGE("Fail to load the window");
    return false;
  }

  if (!mwctx->weHandler->init(mwctx)) {
    LOGE("Fail to init the window event handlers");
    return false;
  }

  auto* window = mwctx->wloader->window();

  window->set_title(project_decls::PROJECT_GUI_NAME + " " +
                    project_decls::PROJECT_BUILD_VERSION);
  window->set_icon_name(project_decls::PROJECT_APPLICATION_ID);

  app.add_window(*window);
  window->present();

  return true;
}

}  // namespace darknetxxGtkmm4
