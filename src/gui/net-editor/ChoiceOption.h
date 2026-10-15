#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_CHOICEOPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_CHOICEOPTION_CLASS_H

#include "src/gui/net-editor/AOption.h"

namespace net_editor
{

/// @brief The option of one of the listed values: the activation, the
/// learning rate policy, the 0 or 1 flag and the like.
class ChoiceOption : public AOption
{
 public:
  ChoiceOption(std::string nkey, std::string ndef, std::string nabout,
               std::vector<std::string> nchoices);

  virtual std::vector<std::string> choices() const override;
  virtual std::string limits(const OptionContext& ctx) const override;
  virtual std::string error(const std::string& value,
                            const OptionContext& ctx) const override;

 private:
  const std::vector<std::string> mchoices;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_CHOICEOPTION_CLASS_H
