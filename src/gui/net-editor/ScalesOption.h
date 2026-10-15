#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_SCALESOPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_SCALESOPTION_CLASS_H

#include "src/gui/net-editor/NumberOption.h"

namespace net_editor
{

/// @brief The scales of the [net] section: the factors of the learning rate
/// at the steps, as many as the steps, which the steps policy requires.
class ScalesOption : public NumberOption
{
 public:
  explicit ScalesOption(std::string nabout);

  virtual bool required(const OptionContext& ctx) const override;

 protected:
  virtual std::string rule(const OptionContext& ctx) const override;
  virtual std::string check(const std::vector<double>& values,
                            const OptionContext& ctx) const override;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_SCALESOPTION_CLASS_H
