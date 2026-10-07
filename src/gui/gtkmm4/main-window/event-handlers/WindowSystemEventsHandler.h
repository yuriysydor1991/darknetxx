#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_WINDOWSYSTEMEVENTSHANDLER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_WINDOWSYSTEMEVENTSHANDLER_CLASS_H

#include <memory>

#include "src/gui/gtkmm4/main-window/event-handlers/AbstractWindowEventHandler.h"

namespace darknetxxGtkmm4::window::event_handlers
{

/**
 * @brief The close of the window, on the user request or on an OS stop
 * signal, which waits for the running training to save it's weights.
 */
class WindowSystemEventsHandler : public AbstractWindowEventHandler
{
 public:
  virtual ~WindowSystemEventsHandler() = default;
  explicit WindowSystemEventsHandler(std::shared_ptr<MainWindowContext> nmwctx);

  virtual bool subscribe() override;

 protected:
  /// @brief Keeps the window till the running training stops.
  virtual bool on_close_request();

  /// @brief Closes the window once the awaited training stops or on an OS
  /// stop signal.
  virtual bool on_watch();

 private:
  bool mclosing{false};
};

}  // namespace darknetxxGtkmm4::window::event_handlers

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_WINDOWSYSTEMEVENTSHANDLER_CLASS_H
