#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_CONVOLUTIONAL_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_CONVOLUTIONAL_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/// @brief The [convolutional] layer, the sub-layer of the other ones too.
class CudaConvolutionalLayer : public CudaLayer
{
 public:
  void place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
             int index) const override;
  void describe(layer& l, ANetworkAdaptor& net) const override;
  void push(const layer& l) const override;
  void pull(const layer& l) const override;

 private:
  /// @brief The own weights, or the ones of the layer they're shared with.
  static void place_weights(const CudaArrays& arrays, layer& l);
  static void place_batchnorm(const CudaArrays& arrays, layer& l, size_t size);
  /// @brief The aligned bit weights of the calculate_binary_weights().
  static void place_bits(const CudaArrays& arrays, layer& l);
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_CONVOLUTIONAL_LAYER_CLASS_H
