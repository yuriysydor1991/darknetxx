#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_TRAINWINDOWLOADER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_TRAINWINDOWLOADER_CLASS_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/label.h>
#include <gtkmm/picture.h>
#include <gtkmm/progressbar.h>

#include "src/gui/gtkmm4/AWindowLoader.h"

namespace darknetxxGtkmm4::window
{

/**
 * @brief Loads the window of the TrainWindow.ui and provides it's widgets.
 */
class TrainWindowLoader : public AWindowLoader
{
 public:
  virtual bool load() override;

  /// @brief The path of the YOLO4 folder or of the dxxwjz2p or dxxwjz3p
  /// project file.
  Gtk::Entry* project_entry() { return widget<Gtk::Entry>("project_entry"); }
  Gtk::Button* folder_button() { return widget<Gtk::Button>("folder_button"); }
  Gtk::Button* project_button()
  {
    return widget<Gtk::Button>("project_button");
  }
  Gtk::Label* status_label() { return widget<Gtk::Label>("status_label"); }

  /// @brief The training widgets below, hidden till a training starts.
  Gtk::Box* training_box() { return widget<Gtk::Box>("training_box"); }
  Gtk::ProgressBar* progress_bar()
  {
    return widget<Gtk::ProgressBar>("progress_bar");
  }
  Gtk::Label* loss_label() { return widget<Gtk::Label>("loss_label"); }
  Gtk::Label* details_label() { return widget<Gtk::Label>("details_label"); }
  Gtk::Button* train_button() { return widget<Gtk::Button>("train_button"); }
  Gtk::Picture* chart_picture()
  {
    return widget<Gtk::Picture>("chart_picture");
  }
};

}  // namespace darknetxxGtkmm4::window

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_TRAINWINDOWLOADER_CLASS_H
