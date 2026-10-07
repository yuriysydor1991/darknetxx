#ifndef DARKNET_CXX_PORT_PROJECT_ACTIVATION_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ACTIVATION_LAYER_CLASS_H

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [activation] layer: the C++ port of the
 * make_activation_layer() function of the original darknet activation_layer.c,
 * the computation stays there.
 */
class ActivationLayer
{
 public:
  /// @brief The make_activation_layer().
  static ALayerAdaptorPtr make(int batch, int inputs, ACTIVATION activation);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_ACTIVATION_LAYER_CLASS_H
