#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_AWINDOWLOADER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_AWINDOWLOADER_CLASS_H

#include <gtkmm/builder.h>
#include <gtkmm/window.h>

#include <memory>

#include "src/log/log.h"

namespace darknetxxGtkmm4::window
{

/**
 * @brief Loads the main_window of a window description embedded into the
 * binary and provides it's widgets.
 */
class AWindowLoader
{
 public:
  virtual ~AWindowLoader() = default;
  AWindowLoader() = default;

  /// @brief Loads the window and checks every widget of it is there.
  virtual bool load() = 0;

  Gtk::Window* window();

 protected:
  /// @brief Loads the window of the GResource path.
  bool load_window(const char* resource);

  /// @brief The widget of the id, nullptr (logged) for none.
  template <class Widget>
  Widget* widget(const char* id)
  {
    auto* found = builder ? builder->get_widget<Widget>(id) : nullptr;

    if (found == nullptr) {
      LOGE("No " << id << " widget in the window");
    }

    return found;
  }

 private:
  Glib::RefPtr<Gtk::Builder> builder;

  /// @brief The toplevel window, which the builder leaves to it's user.
  std::unique_ptr<Gtk::Window> mwindow;
};

}  // namespace darknetxxGtkmm4::window

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_AWINDOWLOADER_CLASS_H
