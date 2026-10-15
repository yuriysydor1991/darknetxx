#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_NETEDITORWINDOWCONTEXT_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_NETEDITORWINDOWCONTEXT_CLASS_H

#include <sigc++/signal.h>

#include <memory>

#include "src/gui/gtkmm4/AWindowContext.h"
#include "src/gui/gtkmm4/net-editor-window/NetEditorWindowLoader.h"
#include "src/gui/net-editor/NetworkProject.h"

namespace darknetxxGtkmm4::window::event_handlers
{
class NetworkEventsHandler;
class ProjectEventsHandler;
}  // namespace darknetxxGtkmm4::window::event_handlers

namespace darknetxxGtkmm4
{

/**
 * @brief The window of the network editor: the network of the project file
 * it edits and the signals of it's handlers.
 */
class NetEditorWindowContext
    : public AWindowContext,
      public std::enable_shared_from_this<NetEditorWindowContext>
{
 public:
  virtual ~NetEditorWindowContext() = default;
  using AWindowContext::AWindowContext;

  window::NetEditorWindowLoader wloader;
  net_editor::NetworkProject project;

  /// @brief The network of the project is another one (the new, the opened
  /// one), the views show it then.
  sigc::signal<void()> reloaded;

  /// @brief The network of the project changed.
  sigc::signal<void()> edited;

  virtual void clear() override;

  /// @brief Tells the message in the status line, the failure in the error
  /// style.
  void tell(const std::string& message, bool failure = false);

  /// @brief The handlers of the views of the network and of it's file, the
  /// loaded window's.
  std::shared_ptr<window::event_handlers::NetworkEventsHandler> network;
  std::shared_ptr<window::event_handlers::ProjectEventsHandler> file;

 protected:
  virtual window::AWindowLoader& loader() override;
  virtual Handlers create_handlers() override;
};

}  // namespace darknetxxGtkmm4

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_NETEDITORWINDOWCONTEXT_CLASS_H
