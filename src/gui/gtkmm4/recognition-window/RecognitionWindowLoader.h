#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_RECOGNITIONWINDOWLOADER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_RECOGNITIONWINDOWLOADER_CLASS_H

#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/picture.h>
#include <gtkmm/spinbutton.h>

#include "src/gui/gtkmm4/AWindowLoader.h"

namespace darknetxxGtkmm4::window
{

/**
 * @brief Loads the window of the RecognitionWindow.ui and provides it's
 * widgets.
 */
class RecognitionWindowLoader : public AWindowLoader
{
 public:
  virtual bool load() override;

  /// @brief The files of the network and the image to recognize.
  Gtk::Entry* weights_entry() { return widget<Gtk::Entry>("weights_entry"); }
  Gtk::Button* weights_button()
  {
    return widget<Gtk::Button>("weights_button");
  }
  Gtk::Entry* cfg_entry() { return widget<Gtk::Entry>("cfg_entry"); }
  Gtk::Button* cfg_button() { return widget<Gtk::Button>("cfg_button"); }
  Gtk::Entry* names_entry() { return widget<Gtk::Entry>("names_entry"); }
  Gtk::Button* names_button() { return widget<Gtk::Button>("names_button"); }
  Gtk::Entry* image_entry() { return widget<Gtk::Entry>("image_entry"); }
  Gtk::Button* image_button() { return widget<Gtk::Button>("image_button"); }

  Gtk::SpinButton* thresh_spin()
  {
    return widget<Gtk::SpinButton>("thresh_spin");
  }
  Gtk::Button* recognize_button()
  {
    return widget<Gtk::Button>("recognize_button");
  }
  Gtk::Label* status_label() { return widget<Gtk::Label>("status_label"); }

  /// @brief The recognized image with the objects boxed and their list.
  Gtk::Picture* image_picture()
  {
    return widget<Gtk::Picture>("image_picture");
  }
  Gtk::Label* objects_label() { return widget<Gtk::Label>("objects_label"); }
};

}  // namespace darknetxxGtkmm4::window

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_RECOGNITIONWINDOWLOADER_CLASS_H
