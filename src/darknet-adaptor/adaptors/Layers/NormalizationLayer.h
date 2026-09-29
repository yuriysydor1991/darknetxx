#ifndef DARKNET_CXX_PORT_PROJECT_NORMALIZATION_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NORMALIZATION_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [lrn] layer: the C++ port of the
 * make_normalization_layer() and resize_normalization_layer() functions of the
 * original darknet normalization_layer.c, the computation stays there.
 */
class NormalizationLayer
{
 public:
  /// @brief The make_normalization_layer().
  static ALayerAdaptorPtr make(int batch, int w, int h, int c, int size,
                               float alpha, float beta, float kappa);

  /// @brief The resize_normalization_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_NORMALIZATION_LAYER_CLASS_H
