#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_DIVISOROPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_DIVISOROPTION_CLASS_H

#include "src/gui/net-editor/NumberOption.h"

namespace net_editor
{

/// @brief The integer dividing the integer of another option of the section:
/// the subdivisions of the batch, the groups of the filters.
class DivisorOption : public NumberOption
{
 public:
  /// @param ndividend The key of the divided option.
  /// @param ndividendDef The divided value of no key.
  DivisorOption(std::string nkey, std::string ndef, std::string nabout,
                std::string ndividend, int ndividendDef);

 protected:
  virtual Range range(const OptionContext& ctx) const override;
  virtual std::string rule(const OptionContext& ctx) const override;
  virtual std::string check(const std::vector<double>& values,
                            const OptionContext& ctx) const override;

 private:
  int dividend(const OptionContext& ctx) const;

  const std::string mdividend;
  const int mdividendDef;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_DIVISOROPTION_CLASS_H
