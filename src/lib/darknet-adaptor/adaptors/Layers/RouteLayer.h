#ifndef DARKNET_CXX_PORT_PROJECT_ROUTE_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ROUTE_LAYER_CLASS_H

#include <vector>

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [route] layer: the C++ port of the
 * make_route_layer() and resize_route_layer() functions of the original darknet
 * route_layer.c, the computation stays there.
 */
class RouteLayer
{
 public:
  /// @brief The make_route_layer() of the given layers of the given outputs.
  static ALayerAdaptorPtr make(int batch, std::vector<int> input_layers,
                               std::vector<int> input_sizes, int groups,
                               int group_id);

  /// @brief The resize_route_layer() to the outputs of the resized network
  /// layers.
  static void resize(ALayerAdaptor& adaptor, const network& net);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_ROUTE_LAYER_CLASS_H
