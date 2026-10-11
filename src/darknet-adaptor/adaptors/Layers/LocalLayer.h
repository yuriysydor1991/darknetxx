#ifndef DARKNET_CXX_PORT_PROJECT_LOCAL_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_LOCAL_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [local] layer: the C++ port of the
 * make_local_layer() function of the original darknet local_layer.c, the
 * computation stays there.
 */
class LocalLayer
{
 public:
  /// @brief The make_local_layer(), the random weights initialization included.
  static ALayerAdaptorPtr make(int batch, int h, int w, int c, int n, int size,
                               int stride, int pad, ACTIVATION activation);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_LOCAL_LAYER_CLASS_H
