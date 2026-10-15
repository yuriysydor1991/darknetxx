#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_SECTIONSCHEMAS_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_SECTIONSCHEMAS_CLASS_H

#include <string>
#include <vector>

#include "src/gui/net-editor/SectionSchema.h"

namespace net_editor
{

/**
 * @brief The cfg sections the editor knows: the [net] one and the layers the
 * darknet parser makes (see the OrigDefaultLoader), the options and the
 * defaults of them the parser reads.
 */
class SectionSchemas
{
 public:
  /// @brief The [net] section.
  static const SectionSchema& net();

  /// @brief The layer of the type (the cfg section name of no brackets, the
  /// "conv" and the "convolutional" are the same), the schema of no options
  /// for the unknown types.
  static const SectionSchema& of(const std::string& type);

  /// @brief The layer types the editor adds, the most used first.
  static const std::vector<std::string>& layer_types();
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_SECTIONSCHEMAS_CLASS_H
