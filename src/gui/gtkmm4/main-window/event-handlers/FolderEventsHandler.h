#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_FOLDEREVENTSHANDLER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_FOLDEREVENTSHANDLER_CLASS_H

#include <gtkmm/filechooserdialog.h>

#include <memory>

#include "src/gui/gtkmm4/main-window/event-handlers/AbstractWindowEventHandler.h"

namespace darknetxxGtkmm4::window::event_handlers
{

/**
 * @brief The choice of the YOLO4 folder: the folder entry, prefilled with the
 * working directory, and the button of the folder chooser dialog, which puts
 * the chosen folder into the entry and activates it.
 */
class FolderEventsHandler : public AbstractWindowEventHandler
{
 public:
  virtual ~FolderEventsHandler() = default;
  explicit FolderEventsHandler(std::shared_ptr<MainWindowContext> nmwctx);

  virtual bool subscribe() override;

 protected:
  virtual void choose();
  virtual void on_chosen(int response);

 private:
  std::unique_ptr<Gtk::FileChooserDialog> chooser;
};

}  // namespace darknetxxGtkmm4::window::event_handlers

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_FOLDEREVENTSHANDLER_CLASS_H
