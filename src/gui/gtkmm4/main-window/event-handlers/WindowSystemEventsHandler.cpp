#include "src/gui/gtkmm4/main-window/event-handlers/WindowSystemEventsHandler.h"

#include <glibmm/main.h>

#include "src/gui/gtkmm4/MainWindowContext.h"

namespace darknetxxGtkmm4::window::event_handlers
{

WindowSystemEventsHandler::WindowSystemEventsHandler(
    std::shared_ptr<MainWindowContext> nmwctx)
    : AbstractWindowEventHandler{nmwctx}
{
}

bool WindowSystemEventsHandler::subscribe()
{
  static constexpr const unsigned int watchIntervalMs = 200;

  mwctx->wloader->window()->signal_close_request().connect(
      sigc::mem_fun(*this, &WindowSystemEventsHandler::on_close_request),
      false);

  Glib::signal_timeout().connect(
      sigc::mem_fun(*this, &WindowSystemEventsHandler::on_watch),
      watchIntervalMs);

  return true;
}

bool WindowSystemEventsHandler::on_close_request()
{
  mclosing = true;

  if (!mwctx->trainer->running()) {
    return false;
  }

  mwctx->trainer->stop();

  mwctx->wloader->train_button()->set_sensitive(false);
  mwctx->wloader->status_label()->set_text(
      "Stopping the training to close the window, the weights are saved");

  return true;
}

bool WindowSystemEventsHandler::on_watch()
{
  const bool close = mclosing ? !mwctx->trainer->running()
                              : mwctx->actx->stop_signal.load() > 0;

  if (close) {
    mwctx->wloader->window()->close();
  }

  return true;
}

}  // namespace darknetxxGtkmm4::window::event_handlers
