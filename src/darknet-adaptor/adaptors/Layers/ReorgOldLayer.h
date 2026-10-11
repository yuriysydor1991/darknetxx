#ifndef DARKNET_CXX_PORT_PROJECT_REORG_OLD_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_REORG_OLD_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [reorg] layer: the C++ port of the
 * make_reorg_old_layer() and resize_reorg_old_layer() functions of the original
 * darknet reorg_old_layer.c, the computation stays there.
 */
class ReorgOldLayer
{
 public:
  /// @brief The make_reorg_old_layer().
  static ALayerAdaptorPtr make(int batch, int w, int h, int c, int stride,
                               int reverse);

  /// @brief The resize_reorg_old_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_REORG_OLD_LAYER_CLASS_H
