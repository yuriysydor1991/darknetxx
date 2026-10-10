#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_CHOOSEREVENTSHANDLER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_CHOOSEREVENTSHANDLER_CLASS_H

#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/filefilter.h>

#include <memory>
#include <string>

#include "src/gui/gtkmm4/IWindowEventHandler.h"

namespace darknetxxGtkmm4::window::event_handlers
{

/**
 * @brief The choice of the file or the folder of an entry: the button of the
 * chooser dialog, which starts at the path of the entry, puts the chosen path
 * into the entry and activates it.
 */
class ChooserEventsHandler : public IWindowEventHandler
{
 public:
  virtual ~ChooserEventsHandler() = default;

  /// @param nfilter The files the chooser shows first, beside all the others.
  ChooserEventsHandler(Gtk::Window& nwindow, Gtk::Entry& nentry,
                       Gtk::Button& nbutton, std::string ntitle,
                       Gtk::FileChooser::Action naction,
                       Glib::RefPtr<Gtk::FileFilter> nfilter = {});

  virtual bool subscribe() override;

 protected:
  virtual void choose();
  virtual void on_chosen(int response);

 private:
  Gtk::Window& window;
  Gtk::Entry& entry;
  Gtk::Button& button;
  const std::string title;
  const Gtk::FileChooser::Action action;
  const Glib::RefPtr<Gtk::FileFilter> filter;
  std::unique_ptr<Gtk::FileChooserDialog> chooser;
};

}  // namespace darknetxxGtkmm4::window::event_handlers

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_CHOOSEREVENTSHANDLER_CLASS_H
