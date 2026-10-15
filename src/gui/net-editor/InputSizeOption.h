#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_INPUTSIZEOPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_INPUTSIZEOPTION_CLASS_H

#include "src/gui/net-editor/NumberOption.h"

namespace net_editor
{

/**
 * @brief The width or the height of the network input of the [net] section,
 * which the network of no inputs= requires: the multiple of the stride the
 * layers downsample it by for the detection layers (the grid of the
 * [yolo] ones, for example).
 */
class InputSizeOption : public NumberOption
{
 public:
  InputSizeOption(std::string nkey, std::string nabout);

  virtual bool required(const OptionContext& ctx) const override;

  /// @brief The highest factor the layers downsample the input by, the one
  /// of the network of the detection layers alone, 1 for none.
  static int stride(const NetworkModel& model);

 protected:
  virtual std::string rule(const OptionContext& ctx) const override;
  virtual std::string check(const std::vector<double>& values,
                            const OptionContext& ctx) const override;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_INPUTSIZEOPTION_CLASS_H
