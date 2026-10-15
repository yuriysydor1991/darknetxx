#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_OPTIONCONTEXT_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_OPTIONCONTEXT_CLASS_H

#include "src/gui/net-editor/NetworkModel.h"
#include "src/gui/net-editor/NetworkSection.h"

namespace net_editor
{

/**
 * @brief The network and the section an option is of, which the limits of
 * the option depend on.
 */
class OptionContext
{
 public:
  OptionContext(const NetworkModel& nmodel, const NetworkSection& nsection);

  const NetworkModel& model;
  const NetworkSection& section;

  /// @brief The index of the layer of the section, -1 for the [net] one.
  int index() const;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_OPTIONCONTEXT_CLASS_H
