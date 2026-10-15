#ifndef DARKNETXX_PROJECT_GUI_QT6_QTICONS_CLASS_H
#define DARKNETXX_PROJECT_GUI_QT6_QTICONS_CLASS_H

#include <QColor>
#include <QIcon>

#include "src/gui/net-editor/EditorIcon.h"

/**
 * @brief The namespace of the Qt 6 GUI.
 */
namespace darknetxxQt6
{

/**
 * @brief The icons of the editor controls: the Font Awesome ones of the Qt
 * resources of the binary (the :/icons/solid/<name>.svg) drawn in the color
 * of the text, the icon theme ones of the build of no Font Awesome.
 */
class QtIcons
{
 public:
  static QIcon of(const net_editor::EditorIcon& icon, const QColor& color);
};

}  // namespace darknetxxQt6

#endif  // DARKNETXX_PROJECT_GUI_QT6_QTICONS_CLASS_H
