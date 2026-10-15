#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_EDITORICON_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_EDITORICON_CLASS_H

namespace net_editor
{

/**
 * @brief An icon of the editor controls: the Font Awesome solid one the GUI
 * binaries embed (see the darknetxx-fontawesome-enabler), the freedesktop
 * icon theme one of the build of no Font Awesome.
 */
class EditorIcon
{
 public:
  constexpr EditorIcon(const char* nawesome, const char* ntheme)
      : awesome{nawesome}, theme{ntheme}
  {
  }

  /// @brief The name of the svgs/solid/<name>.svg of the Font Awesome.
  const char* const awesome;
  /// @brief The name of the icon theme.
  const char* const theme;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_EDITORICON_CLASS_H
