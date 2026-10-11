#ifndef DARKNET_CXX_PORT_PROJECT_MAXPOOL_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_MAXPOOL_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [maxpool] and the [local_avgpool] layer: the C++
 * port of the make_maxpool_layer() and resize_maxpool_layer() functions of the
 * original darknet maxpool_layer.c, the computation stays there.
 */
class MaxpoolLayer
{
 public:
  /// @brief The make_maxpool_layer(), the local average pooling one for the non
  /// zero avgpool.
  static ALayerAdaptorPtr make(int batch, int h, int w, int c, int size,
                               int stride_x, int stride_y, int padding,
                               int maxpool_depth, int out_channels,
                               int antialiasing, int avgpool, int train);

  /// @brief The resize_maxpool_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_MAXPOOL_LAYER_CLASS_H
