#include "src/gui/gtkmm4/net-editor-window/GtkmmIcons.h"

namespace darknetxxGtkmm4::window::widgets
{

std::string GtkmmIcons::name_of(const net_editor::EditorIcon& icon)
{
#ifdef DARKNETXX_FONTAWESOME
  return std::string{"darknetxx-"} + icon.awesome + "-symbolic";
#else
  return std::string{icon.theme} + "-symbolic";
#endif  // DARKNETXX_FONTAWESOME
}

void GtkmmIcons::set(Gtk::Image& image, const net_editor::EditorIcon& icon)
{
  image.set_from_icon_name(name_of(icon));
}

}  // namespace darknetxxGtkmm4::window::widgets
