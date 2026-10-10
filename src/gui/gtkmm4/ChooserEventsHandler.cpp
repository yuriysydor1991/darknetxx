#include "src/gui/gtkmm4/ChooserEventsHandler.h"

#include <giomm/file.h>

#include "src/log/log.h"

namespace darknetxxGtkmm4::window::event_handlers
{

ChooserEventsHandler::ChooserEventsHandler(
    Gtk::Window& nwindow, Gtk::Entry& nentry, Gtk::Button& nbutton,
    std::string ntitle, const Gtk::FileChooser::Action naction,
    Glib::RefPtr<Gtk::FileFilter> nfilter)
    : window{nwindow},
      entry{nentry},
      button{nbutton},
      title{std::move(ntitle)},
      action{naction},
      filter{std::move(nfilter)}
{
}

bool ChooserEventsHandler::subscribe()
{
  button.signal_clicked().connect(
      sigc::mem_fun(*this, &ChooserEventsHandler::choose));

  return true;
}

void ChooserEventsHandler::choose()
{
  if (chooser == nullptr) {
    chooser = std::make_unique<Gtk::FileChooserDialog>(window, title, action);

    chooser->set_modal(true);
    chooser->set_hide_on_close(true);
    chooser->add_button("_Cancel", Gtk::ResponseType::CANCEL);
    chooser->add_button("_Open", Gtk::ResponseType::ACCEPT);
    chooser->signal_response().connect(
        sigc::mem_fun(*this, &ChooserEventsHandler::on_chosen));

    if (filter) {
      auto all = Gtk::FileFilter::create();

      all->set_name("All files");
      all->add_pattern("*");

      chooser->add_filter(filter);
      chooser->add_filter(all);
    }
  }

  const std::string path = entry.get_text().raw();

  try {
    // the file of the entry selected, the folder of it or the working one
    if (action == Gtk::FileChooser::Action::OPEN && !path.empty()) {
      chooser->set_file(Gio::File::create_for_path(path));
    } else {
      chooser->set_current_folder(
          Gio::File::create_for_path(path.empty() ? "." : path));
    }
  }
  catch (const Glib::Error& e) {
    LOGD("The chooser starts at it's own folder: " << e.what());
  }

  chooser->present();
}

void ChooserEventsHandler::on_chosen(const int response)
{
  chooser->set_visible(false);

  const auto chosen = chooser->get_file();

  if (response != Gtk::ResponseType::ACCEPT || !chosen) {
    return;
  }

  entry.set_text(chosen->get_path());

  // the Enter key press of the entry, which the gtk_widget_activate() is not
  g_signal_emit_by_name(entry.gobj(), "activate");
}

}  // namespace darknetxxGtkmm4::window::event_handlers
