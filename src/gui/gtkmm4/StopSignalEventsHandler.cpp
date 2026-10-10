#include "src/gui/gtkmm4/StopSignalEventsHandler.h"

#include <glibmm/main.h>

namespace darknetxxGtkmm4::window::event_handlers
{

StopSignalEventsHandler::StopSignalEventsHandler(app::AppCtxPtr nactx,
                                                 Gtk::Window& nwindow)
    : actx{std::move(nactx)}, window{nwindow}
{
}

bool StopSignalEventsHandler::subscribe()
{
  static constexpr const unsigned int watchIntervalMs = 200;

  Glib::signal_timeout().connect(
      sigc::mem_fun(*this, &StopSignalEventsHandler::on_watch),
      watchIntervalMs);

  return true;
}

bool StopSignalEventsHandler::on_watch()
{
  if (actx->stop_signal.load() > 0) {
    window.close();
  }

  return true;
}

}  // namespace darknetxxGtkmm4::window::event_handlers
