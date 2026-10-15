#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_NETWORKVALIDATOR_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_NETWORKVALIDATOR_CLASS_H

#include <string>
#include <vector>

#include "src/gui/net-editor/NetworkModel.h"

namespace net_editor
{

/// @brief The wrong values of the network the rows of the editor tell (see
/// the OptionPresenter).
class NetworkValidator
{
 public:
  /// @brief The wrong options of the network, the section of each told.
  static std::vector<std::string> errors(const NetworkModel& model);

  /// @brief The name of the section in the messages, e.g. "[net]" or
  /// "layer 3 [maxpool]".
  static std::string name_of(const NetworkModel& model,
                             const NetworkSection& section);
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_NETWORKVALIDATOR_CLASS_H
