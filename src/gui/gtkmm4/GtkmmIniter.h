#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_GTKMMINITER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_GTKMMINITER_CLASS_H

#include <memory>

#include "src/gui/IGUIIniter.h"

namespace Gtk
{
class Application;
}  // namespace Gtk

namespace darknetxxGtkmm4
{

class MainWindowContext;

/**
 * @brief The GTKmm 4 GUI: the window of the YOLO4 folder training.
 */
class GtkmmIniter : public gui::IGUIIniter
{
 public:
  virtual ~GtkmmIniter() = default;
  GtkmmIniter() = default;

  virtual bool run(app::AppCtxPtr ctx,
                   darknetxx011::LibraryContextPtr libctx) override;
  virtual void deinit() override;

 protected:
  /// @brief Loads the window, connects it's handlers and shows it.
  virtual bool show_window(Gtk::Application& app);

 private:
  std::shared_ptr<MainWindowContext> mwctx;
};

}  // namespace darknetxxGtkmm4

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_GTKMMINITER_CLASS_H
