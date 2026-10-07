#ifndef DARKNET_CXX_PORT_PROJECT_YOLO_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_YOLO_LAYER_CLASS_H

#include <vector>

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [yolo] layer: the C++ port of the
 * make_yolo_layer() and resize_yolo_layer() functions of the original darknet
 * yolo_layer.c, the computation stays there.
 */
class YoloLayer
{
 public:
  /// @brief The make_yolo_layer(): the empty mask selects the first n anchors.
  /// Reseeds the rand() with the time as the original did.
  static ALayerAdaptorPtr make(int batch, int w, int h, int n, int total,
                               std::vector<int> mask, int classes,
                               int max_boxes);

  /// @brief The resize_yolo_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_YOLO_LAYER_CLASS_H
