#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_WINDOWLOADER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_WINDOWLOADER_CLASS_H

#include <gtkmm/box.h>
#include <gtkmm/builder.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/picture.h>
#include <gtkmm/progressbar.h>
#include <gtkmm/window.h>

#include <memory>

namespace darknetxxGtkmm4::window
{

/**
 * @brief Loads the window of the GtkmmWindow.ui embedded into the binary and
 * provides it's widgets.
 */
class WindowLoader
{
 public:
  virtual ~WindowLoader() = default;
  WindowLoader() = default;

  /// @brief Loads the window and checks every widget below is there.
  virtual bool load();

  virtual Gtk::Window* window();
  virtual Gtk::Entry* folder_entry();
  virtual Gtk::Button* folder_button();
  virtual Gtk::Label* status_label();

  /// @brief The training widgets below, hidden till a training starts.
  virtual Gtk::Box* training_box();
  virtual Gtk::ProgressBar* progress_bar();
  virtual Gtk::Label* loss_label();
  virtual Gtk::Label* details_label();
  virtual Gtk::Button* train_button();
  virtual Gtk::Picture* chart_picture();

 private:
  template <class Widget>
  Widget* widget(const char* id);

  Glib::RefPtr<Gtk::Builder> builder;

  /// @brief The toplevel window, which the builder leaves to it's user.
  std::unique_ptr<Gtk::Window> mwindow;
};

}  // namespace darknetxxGtkmm4::window

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_WINDOWLOADER_CLASS_H
