#ifndef DARKNET_CXX_PORT_PROJECT_BATCHNORM_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_BATCHNORM_LAYER_CLASS_H

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [batchnorm] layer: the C++ port of the
 * make_batchnorm_layer() and resize_batchnorm_layer() functions of the original
 * darknet batchnorm_layer.c, the computation stays there.
 */
class BatchnormLayer
{
 public:
  /// @brief The make_batchnorm_layer().
  static ALayerAdaptorPtr make(int batch, int w, int h, int c, int train);

  /// @brief The resize_batchnorm_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_BATCHNORM_LAYER_CLASS_H
