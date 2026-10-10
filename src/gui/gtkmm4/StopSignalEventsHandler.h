#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_STOPSIGNALEVENTSHANDLER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_STOPSIGNALEVENTSHANDLER_CLASS_H

#include <gtkmm/window.h>

#include "src/app/ApplicationContext.h"
#include "src/gui/gtkmm4/IWindowEventHandler.h"

namespace darknetxxGtkmm4::window::event_handlers
{

/**
 * @brief Closes the window on an OS stop signal (see the
 * app::ApplicationContext::stop_signal), the way the user does.
 */
class StopSignalEventsHandler : public IWindowEventHandler
{
 public:
  virtual ~StopSignalEventsHandler() = default;
  StopSignalEventsHandler(app::AppCtxPtr nactx, Gtk::Window& nwindow);

  virtual bool subscribe() override;

 protected:
  virtual bool on_watch();

 private:
  app::AppCtxPtr actx;
  Gtk::Window& window;
};

}  // namespace darknetxxGtkmm4::window::event_handlers

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_STOPSIGNALEVENTSHANDLER_CLASS_H
