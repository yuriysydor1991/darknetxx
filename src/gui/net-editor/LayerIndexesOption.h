#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_LAYERINDEXESOPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_LAYERINDEXESOPTION_CLASS_H

#include "src/gui/net-editor/NumberOption.h"

namespace net_editor
{

/**
 * @brief The indexes of the layers before the layer, which it takes the
 * outputs of: the layers= of the [route] layer, the from= of the [shortcut]
 * one and the like (see the LayerReferences). The negative index counts back
 * from the layer, the rest are the absolute ones.
 */
class LayerIndexesOption : public NumberOption
{
 public:
  LayerIndexesOption(std::string nkey, std::string nabout, bool nlist);

  virtual bool required(const OptionContext& ctx) const override;

 protected:
  virtual Range range(const OptionContext& ctx) const override;
  virtual std::string rule(const OptionContext& ctx) const override;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_LAYERINDEXESOPTION_CLASS_H
