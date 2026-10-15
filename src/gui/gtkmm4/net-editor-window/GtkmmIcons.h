#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_GTKMMICONS_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_GTKMMICONS_CLASS_H

#include <gtkmm/image.h>

#include <string>

#include "src/gui/net-editor/EditorIcon.h"

namespace darknetxxGtkmm4::window::widgets
{

/**
 * @brief The icons of the editor controls: the symbolic Font Awesome ones of
 * the GResource of the binary (the darknetxx-<name>-symbolic of the icon
 * theme the GtkmmIniter adds it's icons to), the icon theme ones of the
 * build of no Font Awesome.
 */
class GtkmmIcons
{
 public:
  /// @brief The icon theme name of the icon.
  static std::string name_of(const net_editor::EditorIcon& icon);

  static void set(Gtk::Image& image, const net_editor::EditorIcon& icon);
};

}  // namespace darknetxxGtkmm4::window::widgets

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_GTKMMICONS_CLASS_H
