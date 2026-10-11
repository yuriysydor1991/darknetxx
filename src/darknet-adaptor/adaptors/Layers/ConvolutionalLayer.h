#ifndef DARKNET_CXX_PORT_PROJECT_CONVOLUTIONAL_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CONVOLUTIONAL_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The memory of the [convolutional] layer: the C++ port of the
 * make_convolutional_layer(), resize_convolutional_layer(),
 * free_convolutional_batchnorm() and binary_align_weights() functions of the
 * original darknet convolutional_layer.c, the computation stays there.
 */
class ConvolutionalLayer
{
 public:
  /**
   * @brief Makes the layer the way make_convolutional_layer() did: the
   * same parameters, fields, buffers sizes and the random weights
   * initialization (the rand_uniform() calls in the same order).
   *
   * The share_layer is the layer to share the weights and the batch
   * normalization buffers with (borrowed), none for the own ones. Throws
   * NetworkError where the original darknet exited with an error.
   */
  static ALayerAdaptorPtr make(int batch, int steps, int h, int w, int c, int n,
                               int groups, int size, int stride_x, int stride_y,
                               int dilation, int padding, ACTIVATION activation,
                               int batch_normalize, int binary, int xnor,
                               int adam, int use_bin_output, int index,
                               int antialiasing,
                               const ALayerAdaptorPtr& share_layer,
                               int assisted_excitation, int deform, int train);

  /// @brief The resize_convolutional_layer() to the w x h input.
  static void resize(ALayerAdaptor& adaptor, int w, int h);

  /**
   * @brief The free_convolutional_batchnorm(): releases the batch
   * normalization buffers of the layer, which fuse_conv_batchnorm() merged
   * into the weights. The layer sharing the buffers of another one drops
   * it's borrowed pointers instead.
   */
  static void free_batchnorm(ALayerAdaptor& adaptor);

  /**
   * @brief The binary_align_weights() of the XNOR layer: computes the
   * aligned bit weights buffer (allocating it) and the mean array.
   */
  static void binary_align_weights(ALayerAdaptor& adaptor);
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_CONVOLUTIONAL_LAYER_CLASS_H
