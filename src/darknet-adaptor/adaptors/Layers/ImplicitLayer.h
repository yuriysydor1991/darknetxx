#ifndef DARKNET_CXX_PORT_PROJECT_IMPLICIT_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_IMPLICIT_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [implicit] layer: the C++ port of the
 * make_implicit_layer() function of the original darknet
 * representation_layer.c, the computation stays there.
 */
class ImplicitLayer
{
 public:
  /// @brief The make_implicit_layer(), the random weights initialization
  /// included.
  static ALayerAdaptorPtr make(int batch, int index, float mean_init,
                               float std_init, int filters, int atoms);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_IMPLICIT_LAYER_CLASS_H
