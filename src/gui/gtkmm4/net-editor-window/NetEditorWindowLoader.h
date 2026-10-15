#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_NETEDITORWINDOWLOADER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_NETEDITORWINDOWLOADER_CLASS_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/grid.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/textview.h>

#include "src/gui/gtkmm4/AWindowLoader.h"

namespace darknetxxGtkmm4::window
{

/**
 * @brief Loads the window of the NetEditorWindow.ui and provides it's
 * widgets.
 */
class NetEditorWindowLoader : public AWindowLoader
{
 public:
  virtual bool load() override;

  /// @brief The buttons of the file of the network and their icons.
  Gtk::Button* new_button() { return widget<Gtk::Button>("new_button"); }
  Gtk::Button* open_button() { return widget<Gtk::Button>("open_button"); }
  Gtk::Button* save_button() { return widget<Gtk::Button>("save_button"); }
  Gtk::Button* save_as_button()
  {
    return widget<Gtk::Button>("save_as_button");
  }
  Gtk::Image* new_image() { return widget<Gtk::Image>("new_image"); }
  Gtk::Image* open_image() { return widget<Gtk::Image>("open_image"); }
  Gtk::Image* save_image() { return widget<Gtk::Image>("save_image"); }
  Gtk::Image* save_as_image() { return widget<Gtk::Image>("save_as_image"); }
  Gtk::Label* file_label() { return widget<Gtk::Label>("file_label"); }

  /// @brief The rows of the [net] options, the class names and the layers.
  Gtk::Grid* net_grid() { return widget<Gtk::Grid>("net_grid"); }
  Gtk::TextView* classes_view()
  {
    return widget<Gtk::TextView>("classes_view");
  }
  Gtk::Box* layers_box() { return widget<Gtk::Box>("layers_box"); }

  /// @brief The type of the layer to add, it's button and description.
  Gtk::DropDown* layer_type_dropdown()
  {
    return widget<Gtk::DropDown>("layer_type_dropdown");
  }
  Gtk::Button* add_layer_button()
  {
    return widget<Gtk::Button>("add_layer_button");
  }
  Gtk::Image* add_layer_image()
  {
    return widget<Gtk::Image>("add_layer_image");
  }
  Gtk::Label* layer_type_label()
  {
    return widget<Gtk::Label>("layer_type_label");
  }

  Gtk::Label* status_label() { return widget<Gtk::Label>("status_label"); }
};

}  // namespace darknetxxGtkmm4::window

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_NETEDITORWINDOWLOADER_CLASS_H
