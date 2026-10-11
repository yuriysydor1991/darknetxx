#ifndef DARKNET_CXX_PORT_PROJECT_UPSAMPLE_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_UPSAMPLE_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [upsample] layer: the C++ port of the
 * make_upsample_layer() and resize_upsample_layer() functions of the original
 * darknet upsample_layer.c, the computation stays there.
 */
class UpsampleLayer
{
 public:
  /// @brief The make_upsample_layer(), the negative stride downsamples.
  static ALayerAdaptorPtr make(int batch, int w, int h, int c, int stride);

  /// @brief The resize_upsample_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_UPSAMPLE_LAYER_CLASS_H
