#ifndef DARKNET_CXX_PORT_PROJECT_DETECTION_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DETECTION_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [detection] layer: the C++ port of the
 * make_detection_layer() function of the original darknet detection_layer.c,
 * the computation stays there.
 */
class DetectionLayer
{
 public:
  /// @brief The make_detection_layer(), which reseeds the rand() with the time
  /// as the original did.
  static ALayerAdaptorPtr make(int batch, int inputs, int n, int side,
                               int classes, int coords, int rescore);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_DETECTION_LAYER_CLASS_H
