#ifndef DARKNET_CXX_PORT_PROJECT_AVGPOOL_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_AVGPOOL_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [avgpool] layer: the C++ port of the
 * make_avgpool_layer() and resize_avgpool_layer() functions of the original
 * darknet avgpool_layer.c, the computation stays there.
 */
class AvgpoolLayer
{
 public:
  /// @brief The make_avgpool_layer().
  static ALayerAdaptorPtr make(int batch, int w, int h, int c);

  /// @brief The resize_avgpool_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_AVGPOOL_LAYER_CLASS_H
