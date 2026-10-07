#ifndef DARKNET_CXX_PORT_PROJECT_REORG_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_REORG_LAYER_CLASS_H

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [reorg3d] layer: the C++ port of the
 * make_reorg_layer() and resize_reorg_layer() functions of the original darknet
 * reorg_layer.c, the computation stays there.
 */
class ReorgLayer
{
 public:
  /// @brief The make_reorg_layer().
  static ALayerAdaptorPtr make(int batch, int w, int h, int c, int stride,
                               int reverse);

  /// @brief The resize_reorg_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_REORG_LAYER_CLASS_H
