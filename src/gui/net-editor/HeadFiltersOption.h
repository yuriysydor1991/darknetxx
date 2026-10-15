#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_HEADFILTERSOPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_HEADFILTERSOPTION_CLASS_H

#include "src/gui/net-editor/NumberOption.h"

namespace net_editor
{

/**
 * @brief The filters of the [convolutional] layer, the ones of the
 * detection layer after it: the (classes + 5) * masks ones of the [yolo]
 * layer, the (classes + 9) * masks ones of the [Gaussian_yolo] layer and the
 * num * (classes + coords + 1) ones of the [region] layer.
 */
class HeadFiltersOption : public NumberOption
{
 public:
  explicit HeadFiltersOption(std::string nabout);

 protected:
  virtual std::string rule(const OptionContext& ctx) const override;
  virtual std::string check(const std::vector<double>& values,
                            const OptionContext& ctx) const override;

 private:
  /// @brief The filters the layer after the one of the context takes, -1
  /// for any, and their formula.
  static int head(const OptionContext& ctx, std::string& formula);
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_HEADFILTERSOPTION_CLASS_H
