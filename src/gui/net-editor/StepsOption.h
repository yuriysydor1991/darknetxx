#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_STEPSOPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_STEPSOPTION_CLASS_H

#include "src/gui/net-editor/NumberOption.h"

namespace net_editor
{

/// @brief The steps of the [net] section: the ascending iterations of the
/// learning rate scales up to the max_batches, as many as the scales, which
/// the steps policy requires.
class StepsOption : public NumberOption
{
 public:
  explicit StepsOption(std::string nabout);

  virtual bool required(const OptionContext& ctx) const override;

 protected:
  virtual Range range(const OptionContext& ctx) const override;
  virtual std::string rule(const OptionContext& ctx) const override;
  virtual std::string check(const std::vector<double>& values,
                            const OptionContext& ctx) const override;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_STEPSOPTION_CLASS_H
