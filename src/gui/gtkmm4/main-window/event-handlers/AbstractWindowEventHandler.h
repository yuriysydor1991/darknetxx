#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_ABSTRACTWINDOWEVENTHANDLER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_ABSTRACTWINDOWEVENTHANDLER_CLASS_H

#include <sigc++/trackable.h>

#include <memory>

namespace darknetxxGtkmm4
{
class MainWindowContext;
}  // namespace darknetxxGtkmm4

namespace darknetxxGtkmm4::window::event_handlers
{

/**
 * @brief The handler of the events of a part of the window. It's connections
 * end with it.
 */
class AbstractWindowEventHandler : public sigc::trackable
{
 public:
  virtual ~AbstractWindowEventHandler() = default;
  explicit AbstractWindowEventHandler(
      std::shared_ptr<MainWindowContext> nmwctx);

  /// @brief Connects the handler to the signals of the window.
  virtual bool subscribe() = 0;

 protected:
  std::shared_ptr<MainWindowContext> mwctx;
};

}  // namespace darknetxxGtkmm4::window::event_handlers

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_ABSTRACTWINDOWEVENTHANDLER_CLASS_H
