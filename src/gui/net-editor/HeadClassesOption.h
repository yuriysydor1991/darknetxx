#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_HEADCLASSESOPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_HEADCLASSESOPTION_CLASS_H

#include "src/gui/net-editor/NumberOption.h"

namespace net_editor
{

/// @brief The classes of the detection layer: the count of the class names
/// of the network, which the training and the mAP require.
class HeadClassesOption : public NumberOption
{
 public:
  HeadClassesOption(std::string ndef, std::string nabout);

 protected:
  virtual std::string rule(const OptionContext& ctx) const override;
  virtual std::string check(const std::vector<double>& values,
                            const OptionContext& ctx) const override;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_HEADCLASSESOPTION_CLASS_H
