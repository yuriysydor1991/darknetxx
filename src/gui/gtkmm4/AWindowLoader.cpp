#include "src/gui/gtkmm4/AWindowLoader.h"

namespace darknetxxGtkmm4::window
{

Gtk::Window* AWindowLoader::window() { return mwindow.get(); }

bool AWindowLoader::load_window(const char* resource)
{
  try {
    builder = Gtk::Builder::create_from_resource(resource);
  }
  catch (const Glib::Error& e) {
    LOGE("Fail to load the window: " << e.what());
    return false;
  }

  mwindow.reset(widget<Gtk::Window>("main_window"));

  return window() != nullptr;
}

}  // namespace darknetxxGtkmm4::window
