#include "src/gui/gtkmm4/train-window/WindowSystemEventsHandler.h"

#include <glibmm/main.h>

#include "src/gui/gtkmm4/train-window/TrainWindowContext.h"

namespace darknetxxGtkmm4::window::event_handlers
{

WindowSystemEventsHandler::WindowSystemEventsHandler(
    std::shared_ptr<TrainWindowContext> nmwctx)
    : mwctx{std::move(nmwctx)}
{
}

bool WindowSystemEventsHandler::subscribe()
{
  static constexpr const unsigned int watchIntervalMs = 200;

  mwctx->wloader.window()->signal_close_request().connect(
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

  mwctx->wloader.train_button()->set_sensitive(false);
  mwctx->wloader.status_label()->set_text(
      "Stopping the training to close the window, the weights are saved");

  return true;
}

bool WindowSystemEventsHandler::on_watch()
{
  if (mclosing && !mwctx->trainer->running()) {
    mwctx->window()->close();
  }

  return true;
}

}  // namespace darknetxxGtkmm4::window::event_handlers
