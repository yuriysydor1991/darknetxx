#ifndef DARKNET_CXX_PORT_PROJECT_EMPTY_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_EMPTY_LAYER_CLASS_H

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [empty] layer: the C++ port of the empty layer the
 * original parse_network_cfg_custom() made, the computation-free layer.
 */
class EmptyLayer
{
 public:
  /// @brief The [empty] (the [silence]) layer passing the previous layer output
  /// and delta, which it borrows.
  static ALayerAdaptorPtr make(int batch, int w, int h, int c, int inputs,
                               float* output, float* delta);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_EMPTY_LAYER_CLASS_H
