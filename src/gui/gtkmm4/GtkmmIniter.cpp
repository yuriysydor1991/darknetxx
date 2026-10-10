#include "src/gui/gtkmm4/GtkmmIniter.h"

#include <glibmm/miscutils.h>
#include <gtkmm/application.h>
#include <gtkmm/icontheme.h>

#include <algorithm>
#include <cassert>

#include "project-global-decls.h"
#include "src/log/log.h"

namespace darknetxxGtkmm4
{

GtkmmIniter::GtkmmIniter(std::string nname, std::string nid)
    : mname{std::move(nname)}, mid{std::move(nid)}
{
}

bool GtkmmIniter::run(app::AppCtxPtr ctx,
                      darknetxx011::LibraryContextPtr libctx)
{
  assert(ctx != nullptr);
  assert(libctx != nullptr);

  if (ctx == nullptr || libctx == nullptr) {
    LOGE("No valid application or library context provided");
    return false;
  }

  mwctx = create_window_context(ctx, libctx);

  // the Wayland app id, which names the desktop entry and the icon
  Glib::set_prgname(mid);

  // every process works on it's own
  auto app = Gtk::Application::create(mid, Gio::Application::Flags::NON_UNIQUE);

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
  if (!mwctx->load()) {
    return false;
  }

  auto* window = mwctx->window();

  window->set_title(mname + " " + project_decls::PROJECT_BUILD_VERSION);

  // the Darknet logo of the GResource, which the installed icons are as well,
  // so the binary shows it uninstalled too
  Gtk::IconTheme::get_for_display(window->get_display())
      ->add_resource_path("/ua/org/kytok/darknetxx/gui/icons");
  window->set_icon_name("darknet-logo");

  app.add_window(*window);
  window->present();

  return true;
}

}  // namespace darknetxxGtkmm4
