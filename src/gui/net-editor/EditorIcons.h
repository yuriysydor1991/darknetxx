#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_EDITORICONS_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_EDITORICONS_CLASS_H

#include "src/gui/net-editor/EditorIcon.h"

namespace net_editor
{

/// @brief The icons of the editor controls, the Font Awesome ones the
/// darknetxx-fontawesome-enabler embeds.
class EditorIcons
{
 public:
  static constexpr EditorIcon CREATE{"file-circle-plus", "document-new"};
  static constexpr EditorIcon OPEN{"folder-open", "document-open"};
  static constexpr EditorIcon SAVE{"floppy-disk", "document-save"};
  static constexpr EditorIcon SAVE_AS{"file-export", "document-save-as"};
  static constexpr EditorIcon ADD{"plus", "list-add"};
  static constexpr EditorIcon REMOVE{"minus", "list-remove"};
  static constexpr EditorIcon DRAG{"grip-vertical", "list-drag-handle"};
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_EDITORICONS_CLASS_H
