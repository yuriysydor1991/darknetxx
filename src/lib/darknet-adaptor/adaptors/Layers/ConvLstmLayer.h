#ifndef DARKNET_CXX_PORT_PROJECT_CONV_LSTM_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CONV_LSTM_LAYER_CLASS_H

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [conv_lstm] and the [history] layer: the C++ port
 * of the make_conv_lstm_layer(), make_history_layer() and
 * resize_conv_lstm_layer() functions of the original darknet conv_lstm_layer.c,
 * the computation stays there.
 */
class ConvLstmLayer
{
 public:
  /// @brief The make_conv_lstm_layer() with it's convolutional sub-layers. As
  /// in the original, the peephole less layer keeps the empty vf, vi and vo
  /// sub-layers and the bottleneck one the empty wi, wg and wo ones.
  static ALayerAdaptorPtr make(int batch, int h, int w, int c,
                               int output_filters, int groups, int steps,
                               int size, int stride, int dilation, int pad,
                               ACTIVATION activation, int batch_normalize,
                               int peephole, int xnor, int bottleneck,
                               int train);

  /// @brief The make_history_layer().
  static ALayerAdaptorPtr make_history(int batch, int h, int w, int c,
                                       int history_size, int steps, int train);

  /// @brief The resize_conv_lstm_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_CONV_LSTM_LAYER_CLASS_H
