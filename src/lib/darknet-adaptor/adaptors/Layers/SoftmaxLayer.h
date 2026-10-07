#ifndef DARKNET_CXX_PORT_PROJECT_SOFTMAX_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_SOFTMAX_LAYER_CLASS_H

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [softmax] and the [contrastive] layer: the C++
 * port of the make_softmax_layer() and make_contrastive_layer() functions of
 * the original darknet softmax_layer.c, the computation stays there.
 */
class SoftmaxLayer
{
 public:
  /// @brief The make_softmax_layer().
  static ALayerAdaptorPtr make(int batch, int inputs, int groups);

  /// @brief The make_contrastive_layer(), which borrows the labels and the
  /// class_ids of the given YOLO layer, none for the own ones.
  static ALayerAdaptorPtr make_contrastive(int batch, int w, int h, int c,
                                           int classes, int inputs,
                                           layer* yolo_layer);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_SOFTMAX_LAYER_CLASS_H
