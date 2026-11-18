#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_LAYER_CLASS_H

#include <initializer_list>

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/ICudaLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/**
 * @brief The layer kind of no cuDNN descriptors and no weights files arrays,
 * the helpers of the kinds of the sub-layers.
 */
class CudaLayer : public ICudaLayer
{
 public:
  using forward_fn = decltype(layer::forward_gpu);
  using backward_fn = decltype(layer::backward_gpu);
  using update_fn = decltype(layer::update_gpu);

  void describe(layer& l, ANetworkAdaptor& net) const override;
  void push(const layer& l) const override;
  void pull(const layer& l) const override;

 protected:
  /// @brief Places the sub-layers, returns the biggest workspace of them.
  static size_t place_all(std::initializer_list<ALayerAdaptorPtr> sublayers,
                          ANetworkAdaptor& net, int index);
  static void push_all(std::initializer_list<const layer*> sublayers);
  static void pull_all(std::initializer_list<const layer*> sublayers);
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_LAYER_CLASS_H
