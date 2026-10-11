#ifndef DARKNET_CXX_PORT_PROJECT_SCALE_CHANNELS_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_SCALE_CHANNELS_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [scale_channels] layer: the C++ port of the
 * make_scale_channels_layer() and resize_scale_channels_layer() functions of
 * the original darknet scale_channels_layer.c, the computation stays there.
 */
class ScaleChannelsLayer
{
 public:
  /// @brief The make_scale_channels_layer().
  static ALayerAdaptorPtr make(int batch, int index, int w, int h, int c,
                               int w2, int h2, int c2, int scale_wh);

  /// @brief The resize_scale_channels_layer() to the resized network layers.
  static void resize(ALayerAdaptor& adaptor, const network& net);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_SCALE_CHANNELS_LAYER_CLASS_H
