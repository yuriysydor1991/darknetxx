#ifndef DARKNET_CXX_PORT_PROJECT_CRNN_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CRNN_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [crnn] layer: the C++ port of the
 * make_crnn_layer() and resize_crnn_layer() functions of the original darknet
 * crnn_layer.c, the computation stays there.
 */
class CrnnLayer
{
 public:
  /// @brief The make_crnn_layer() with it's convolutional sub-layers, the
  /// output and the delta of the layer are the output sub-layer ones.
  static ALayerAdaptorPtr make(int batch, int h, int w, int c,
                               int hidden_filters, int output_filters,
                               int groups, int steps, int size, int stride,
                               int dilation, int pad, ACTIVATION activation,
                               int batch_normalize, int xnor, int train);

  /// @brief The resize_crnn_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_CRNN_LAYER_CLASS_H
