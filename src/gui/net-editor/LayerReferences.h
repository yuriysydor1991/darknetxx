#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_LAYERREFERENCES_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_LAYERREFERENCES_CLASS_H

#include <string>
#include <vector>

#include "src/gui/net-editor/NetworkSection.h"

namespace net_editor
{

/**
 * @brief The options of the layers referring to the other layers by their
 * indexes: the layers= of the [route] and the from= of the [shortcut], the
 * [scale_channels] and the [sam] ones, the negative index of them is the one
 * back from the layer, the rest is the absolute one.
 */
class LayerReferences
{
 public:
  /// @brief The option of the layer type referring to the layers, empty for
  /// none.
  static std::string key_of(const std::string& type);

  /**
   * @brief Rewrites the references of the layers moved or removed, so they
   * refer to the same layers: the relative ones stay relative, the absolute
   * ones absolute. The references to the removed layers stay as they were.
   *
   * @param before The layers before the change.
   * @param after The same layers in their new order, of none removed.
   */
  static void remap(const std::vector<NetworkSectionPtr>& before,
                    const std::vector<NetworkSectionPtr>& after);
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_LAYERREFERENCES_CLASS_H
