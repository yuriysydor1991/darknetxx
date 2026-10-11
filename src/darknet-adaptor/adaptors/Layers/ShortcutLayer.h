#ifndef DARKNET_CXX_PORT_PROJECT_SHORTCUT_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_SHORTCUT_LAYER_CLASS_H

#include <vector>

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [shortcut] layer: the C++ port of the
 * make_shortcut_layer() and resize_shortcut_layer() functions of the original
 * darknet shortcut_layer.c, the computation stays there.
 */
class ShortcutLayer
{
 public:
  /// @brief The make_shortcut_layer(): the layers_output and the layers_delta
  /// borrow the output and the delta buffers of the input layers.
  static ALayerAdaptorPtr make(int batch, std::vector<int> input_layers,
                               std::vector<int> input_sizes, int w, int h,
                               int c, vrfloats layers_output,
                               vrfloats layers_delta,
                               WEIGHTS_TYPE_T weights_type,
                               WEIGHTS_NORMALIZATION_T weights_normalization,
                               ACTIVATION activation, int train);

  /// @brief The resize_shortcut_layer() to the w x h input of the resized
  /// network layers.
  static void resize(ALayerAdaptor& adaptor, int w, int h, const network& net);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_SHORTCUT_LAYER_CLASS_H
