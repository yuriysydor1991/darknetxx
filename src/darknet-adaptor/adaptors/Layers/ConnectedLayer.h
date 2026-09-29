#ifndef DARKNET_CXX_PORT_PROJECT_CONNECTED_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CONNECTED_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [connected] layer: the C++ port of the
 * make_connected_layer() function of the original darknet connected_layer.c,
 * the computation stays there.
 */
class ConnectedLayer
{
 public:
  /// @brief The make_connected_layer(), the random weights initialization
  /// included.
  static ALayerAdaptorPtr make(int batch, int steps, int inputs, int outputs,
                               ACTIVATION activation, int batch_normalize);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_CONNECTED_LAYER_CLASS_H
