#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_BOUNDEDOPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_BOUNDEDOPTION_CLASS_H

#include "src/gui/net-editor/NumberOption.h"

namespace net_editor
{

/**
 * @brief The integers up to the integer of another option of the section:
 * the burn_in up to the max_batches, the mask of the [yolo] layer below it's
 * num, the group_id of the [route] layer below it's groups.
 */
class BoundedOption : public NumberOption
{
 public:
  /**
   * @param nlowest The lowest value.
   * @param nbound The key of the option of the highest value.
   * @param nboundDef The highest value of no key.
   * @param nbelow Tells the values are below the bound, up to it otherwise.
   */
  BoundedOption(std::string nkey, std::string ndef, std::string nabout,
                bool nlist, int nlowest, std::string nbound, int nboundDef,
                bool nbelow);

 protected:
  virtual Range range(const OptionContext& ctx) const override;
  virtual std::string rule(const OptionContext& ctx) const override;

 private:
  const int mlowest;
  const std::string mbound;
  const int mboundDef;
  const bool mbelow;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_BOUNDEDOPTION_CLASS_H
