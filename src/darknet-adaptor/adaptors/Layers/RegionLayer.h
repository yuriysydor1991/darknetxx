#ifndef DARKNET_CXX_PORT_PROJECT_REGION_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_REGION_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [region] layer: the C++ port of the
 * make_region_layer() and resize_region_layer() functions of the original
 * darknet region_layer.c, the computation stays there.
 */
class RegionLayer
{
 public:
  /// @brief The make_region_layer(), which reseeds the rand() with the time as
  /// the original did.
  static ALayerAdaptorPtr make(int batch, int w, int h, int n, int classes,
                               int coords, int max_boxes);

  /// @brief The resize_region_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_REGION_LAYER_CLASS_H
