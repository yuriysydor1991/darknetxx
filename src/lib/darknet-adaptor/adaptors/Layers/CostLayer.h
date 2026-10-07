#ifndef DARKNET_CXX_PORT_PROJECT_COST_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_COST_LAYER_CLASS_H

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [cost] layer: the C++ port of the
 * make_cost_layer() and resize_cost_layer() functions of the original darknet
 * cost_layer.c, the computation stays there.
 */
class CostLayer
{
 public:
  /// @brief The make_cost_layer().
  static ALayerAdaptorPtr make(int batch, int inputs, COST_TYPE cost_type,
                               float scale);

  /// @brief The resize_cost_layer() to the given inputs.
  static void resize(ALayerAdaptor& adaptor, int inputs);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_COST_LAYER_CLASS_H
