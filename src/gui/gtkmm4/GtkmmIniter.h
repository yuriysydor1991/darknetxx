#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_GTKMMINITER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_GTKMMINITER_CLASS_H

#include <memory>
#include <string>

#include "src/gui/IGUIIniter.h"
#include "src/gui/gtkmm4/AWindowContext.h"

namespace Gtk
{
class Application;
}  // namespace Gtk

namespace darknetxxGtkmm4
{

/**
 * @brief The GTKmm 4 GUI of a binary: the application of it's id, which shows
 * the window of the GUI.
 */
class GtkmmIniter : public gui::IGUIIniter
{
 public:
  virtual ~GtkmmIniter() = default;

  /**
   * @param nname The name of the GUI binary, the title of the window.
   * @param nid The application id: the D-Bus name and the Wayland app id,
   * which names the desktop entry and the installed icon.
   */
  GtkmmIniter(std::string nname, std::string nid);

  virtual bool run(app::AppCtxPtr ctx,
                   darknetxx011::LibraryContextPtr libctx) override;
  virtual void deinit() override;

 protected:
  /// @brief The window of the GUI, not loaded yet.
  virtual std::shared_ptr<AWindowContext> create_window_context(
      app::AppCtxPtr ctx, darknetxx011::LibraryContextPtr libctx) = 0;

  /// @brief Loads the window, connects it's handlers and shows it.
  virtual bool show_window(Gtk::Application& app);

 private:
  const std::string mname;
  const std::string mid;
  std::shared_ptr<AWindowContext> mwctx;
};

}  // namespace darknetxxGtkmm4

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_GTKMMINITER_CLASS_H
