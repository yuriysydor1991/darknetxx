#ifndef DARKNET_CXX_PORT_PROJECT_CROP_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CROP_LAYER_CLASS_H

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [crop] layer: the C++ port of the
 * make_crop_layer() and resize_crop_layer() functions of the original darknet
 * crop_layer.c, the computation stays there.
 */
class CropLayer
{
 public:
  /// @brief The make_crop_layer().
  static ALayerAdaptorPtr make(int batch, int h, int w, int c, int crop_height,
                               int crop_width, int flip, float angle,
                               float saturation, float exposure);

  /// @brief The resize_crop_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_CROP_LAYER_CLASS_H
