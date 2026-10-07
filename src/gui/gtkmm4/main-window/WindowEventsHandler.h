#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_WINDOWEVENTSHANDLER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_WINDOWEVENTSHANDLER_CLASS_H

#include <memory>
#include <vector>

namespace darknetxxGtkmm4
{
class MainWindowContext;
}  // namespace darknetxxGtkmm4

namespace darknetxxGtkmm4::window::event_handlers
{
class AbstractWindowEventHandler;
}  // namespace darknetxxGtkmm4::window::event_handlers

namespace darknetxxGtkmm4::window
{

/**
 * @brief Connects the event handlers of the window, one per it's part.
 */
class WindowEventsHandler
{
 public:
  using HandlerPtr =
      std::shared_ptr<event_handlers::AbstractWindowEventHandler>;
  using Handlers = std::vector<HandlerPtr>;

  virtual ~WindowEventsHandler() = default;
  WindowEventsHandler() = default;

  virtual bool init(std::shared_ptr<MainWindowContext> nmwctx);

 protected:
  virtual Handlers create_handlers(std::shared_ptr<MainWindowContext> nmwctx);

 private:
  Handlers handlers;
};

}  // namespace darknetxxGtkmm4::window

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_WINDOWEVENTSHANDLER_CLASS_H
