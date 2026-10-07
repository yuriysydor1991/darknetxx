#include "src/gui/gtkmm4/main-window/WindowLoader.h"

#include "src/log/log.h"

namespace darknetxxGtkmm4::window
{

namespace
{
const char* const UI_RESOURCE = "/ua/org/kytok/darknetxx/gui/GtkmmWindow.ui";
}  // namespace

template <class Widget>
Widget* WindowLoader::widget(const char* id)
{
  if (!builder) {
    return nullptr;
  }

  auto* found = builder->get_widget<Widget>(id);

  if (found == nullptr) {
    LOGE("No " << id << " widget in the window");
  }

  return found;
}

bool WindowLoader::load()
{
  try {
    builder = Gtk::Builder::create_from_resource(UI_RESOURCE);
  }
  catch (const Glib::Error& e) {
    LOGE("Fail to load the window: " << e.what());
    return false;
  }

  mwindow.reset(widget<Gtk::Window>("main_window"));

  return window() != nullptr && folder_entry() != nullptr &&
         folder_button() != nullptr && status_label() != nullptr &&
         training_box() != nullptr && progress_bar() != nullptr &&
         loss_label() != nullptr && details_label() != nullptr &&
         train_button() != nullptr && chart_picture() != nullptr;
}

Gtk::Window* WindowLoader::window() { return mwindow.get(); }

Gtk::Entry* WindowLoader::folder_entry()
{
  return widget<Gtk::Entry>("folder_entry");
}

Gtk::Button* WindowLoader::folder_button()
{
  return widget<Gtk::Button>("folder_button");
}

Gtk::Label* WindowLoader::status_label()
{
  return widget<Gtk::Label>("status_label");
}

Gtk::Box* WindowLoader::training_box()
{
  return widget<Gtk::Box>("training_box");
}

Gtk::ProgressBar* WindowLoader::progress_bar()
{
  return widget<Gtk::ProgressBar>("progress_bar");
}

Gtk::Label* WindowLoader::loss_label()
{
  return widget<Gtk::Label>("loss_label");
}

Gtk::Label* WindowLoader::details_label()
{
  return widget<Gtk::Label>("details_label");
}

Gtk::Button* WindowLoader::train_button()
{
  return widget<Gtk::Button>("train_button");
}

Gtk::Picture* WindowLoader::chart_picture()
{
  return widget<Gtk::Picture>("chart_picture");
}

}  // namespace darknetxxGtkmm4::window
