#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_IWINDOWEVENTHANDLER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_IWINDOWEVENTHANDLER_CLASS_H

#include <sigc++/trackable.h>

#include <memory>

namespace darknetxxGtkmm4::window::event_handlers
{

/**
 * @brief The handler of the events of a part of a window. It's connections
 * end with it.
 */
class IWindowEventHandler : public sigc::trackable
{
 public:
  virtual ~IWindowEventHandler() = default;
  IWindowEventHandler() = default;

  /// @brief Connects the handler to the signals of the window.
  virtual bool subscribe() = 0;
};

using IWindowEventHandlerPtr = std::shared_ptr<IWindowEventHandler>;

}  // namespace darknetxxGtkmm4::window::event_handlers

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_IWINDOWEVENTHANDLER_CLASS_H
