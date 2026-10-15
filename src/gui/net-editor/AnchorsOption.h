#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_ANCHORSOPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_ANCHORSOPTION_CLASS_H

#include "src/gui/net-editor/NumberOption.h"

namespace net_editor
{

/// @brief The anchors of the detection layer: the width and the height of
/// each of the num anchors.
class AnchorsOption : public NumberOption
{
 public:
  explicit AnchorsOption(std::string nabout);

 protected:
  virtual std::string rule(const OptionContext& ctx) const override;
  virtual std::string check(const std::vector<double>& values,
                            const OptionContext& ctx) const override;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_ANCHORSOPTION_CLASS_H
