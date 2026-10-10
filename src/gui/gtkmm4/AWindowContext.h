#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_AWINDOWCONTEXT_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_AWINDOWCONTEXT_CLASS_H

#include <vector>

#include "LibraryContext.h"
#include "src/app/ApplicationContext.h"
#include "src/gui/gtkmm4/AWindowLoader.h"
#include "src/gui/gtkmm4/IWindowEventHandler.h"

/**
 * @brief The namespace of the GTKmm 4 GUI.
 */
namespace darknetxxGtkmm4
{

/**
 * @brief The window of a GUI binary and the parts of it the event handlers
 * share.
 */
class AWindowContext
{
 public:
  using Handlers = std::vector<window::event_handlers::IWindowEventHandlerPtr>;

  virtual ~AWindowContext() = default;
  AWindowContext(app::AppCtxPtr nactx, darknetxx011::LibraryContextPtr nlibctx);

  /// @brief Loads the window and connects the event handlers of it.
  virtual bool load();

  Gtk::Window* window();

  /// @brief Breaks the references of the event handlers to the window, the
  /// work of it waited for.
  virtual void clear();

  app::AppCtxPtr actx;
  darknetxx011::LibraryContextPtr libctx;

 protected:
  virtual window::AWindowLoader& loader() = 0;

  /// @brief The handlers of the events of the loaded window.
  virtual Handlers create_handlers() = 0;

 private:
  Handlers handlers;
};

}  // namespace darknetxxGtkmm4

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_AWINDOWCONTEXT_CLASS_H
