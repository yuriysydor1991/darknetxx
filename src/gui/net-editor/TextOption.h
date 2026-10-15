#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_TEXTOPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_TEXTOPTION_CLASS_H

#include "src/gui/net-editor/AOption.h"

namespace net_editor
{

/// @brief The option of a text of no white space, a file path for example
/// (the darknet drops the white space of the cfg lines).
class TextOption : public AOption
{
 public:
  using AOption::AOption;

  virtual bool typable(const std::string& text) const override;
  virtual std::string limits(const OptionContext& ctx) const override;
  virtual std::string error(const std::string& value,
                            const OptionContext& ctx) const override;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_TEXTOPTION_CLASS_H
