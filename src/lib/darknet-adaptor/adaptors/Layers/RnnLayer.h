#ifndef DARKNET_CXX_PORT_PROJECT_RNN_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_RNN_LAYER_CLASS_H

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [rnn] layer: the C++ port of the make_rnn_layer()
 * function of the original darknet rnn_layer.c, the computation stays there.
 */
class RnnLayer
{
 public:
  /// @brief The make_rnn_layer() with it's connected sub-layers, the output and
  /// the delta of the layer are the output sub-layer ones.
  static ALayerAdaptorPtr make(int batch, int inputs, int hidden, int outputs,
                               int steps, ACTIVATION activation,
                               int batch_normalize, int log);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_RNN_LAYER_CLASS_H
