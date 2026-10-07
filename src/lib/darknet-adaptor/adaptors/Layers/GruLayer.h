#ifndef DARKNET_CXX_PORT_PROJECT_GRU_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_GRU_LAYER_CLASS_H

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [gru] layer: the C++ port of the make_gru_layer()
 * function of the original darknet gru_layer.c, the computation stays there.
 */
class GruLayer
{
 public:
  /// @brief The make_gru_layer() with it's connected sub-layers.
  static ALayerAdaptorPtr make(int batch, int inputs, int outputs, int steps,
                               int batch_normalize);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_GRU_LAYER_CLASS_H
