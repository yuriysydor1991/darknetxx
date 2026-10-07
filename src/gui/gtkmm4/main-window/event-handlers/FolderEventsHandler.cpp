#include "src/gui/gtkmm4/main-window/event-handlers/FolderEventsHandler.h"

#include <giomm/file.h>

#include <filesystem>
#include <system_error>

#include "src/gui/gtkmm4/MainWindowContext.h"
#include "src/log/log.h"

namespace darknetxxGtkmm4::window::event_handlers
{

FolderEventsHandler::FolderEventsHandler(
    std::shared_ptr<MainWindowContext> nmwctx)
    : AbstractWindowEventHandler{nmwctx}
{
}

bool FolderEventsHandler::subscribe()
{
  std::error_code error;

  mwctx->wloader->folder_entry()->set_text(
      std::filesystem::current_path(error).string());

  mwctx->wloader->folder_button()->signal_clicked().connect(
      sigc::mem_fun(*this, &FolderEventsHandler::choose));

  return true;
}

void FolderEventsHandler::choose()
{
  if (chooser == nullptr) {
    chooser = std::make_unique<Gtk::FileChooserDialog>(
        *mwctx->wloader->window(), "Choose the YOLO4 folder",
        Gtk::FileChooser::Action::SELECT_FOLDER);

    chooser->set_modal(true);
    chooser->set_hide_on_close(true);
    chooser->add_button("_Cancel", Gtk::ResponseType::CANCEL);
    chooser->add_button("_Open", Gtk::ResponseType::ACCEPT);
    chooser->signal_response().connect(
        sigc::mem_fun(*this, &FolderEventsHandler::on_chosen));
  }

  try {
    chooser->set_current_folder(Gio::File::create_for_path(
        mwctx->wloader->folder_entry()->get_text().raw()));
  }
  catch (const Glib::Error& e) {
    LOGD("The chooser starts at it's own folder: " << e.what());
  }

  chooser->present();
}

void FolderEventsHandler::on_chosen(const int response)
{
  chooser->set_visible(false);

  const auto folder = chooser->get_file();

  if (response != Gtk::ResponseType::ACCEPT || !folder) {
    return;
  }

  auto* entry = mwctx->wloader->folder_entry();

  entry->set_text(folder->get_path());

  // the Enter key press of the entry, which the gtk_widget_activate() is not
  g_signal_emit_by_name(entry->gobj(), "activate");
}

}  // namespace darknetxxGtkmm4::window::event_handlers
