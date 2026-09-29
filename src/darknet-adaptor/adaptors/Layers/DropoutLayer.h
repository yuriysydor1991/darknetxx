#ifndef DARKNET_CXX_PORT_PROJECT_DROPOUT_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DROPOUT_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [dropout] layer: the C++ port of the
 * make_dropout_layer() and resize_dropout_layer() functions of the original
 * darknet dropout_layer.c, the computation stays there.
 */
class DropoutLayer
{
 public:
  /// @brief The make_dropout_layer(). The output and the delta of the layer are
  /// the previous layer ones, which the network borrows.
  static ALayerAdaptorPtr make(int batch, int inputs, float probability,
                               int dropblock, float dropblock_size_rel,
                               int dropblock_size_abs, int w, int h, int c);

  /// @brief The resize_dropout_layer() to the given inputs.
  static void resize(ALayerAdaptor& adaptor, int inputs);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_DROPOUT_LAYER_CLASS_H
