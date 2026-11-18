#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_RNN_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_RNN_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/**
 * @brief The [rnn] and the [crnn] layers: the state of every step and the
 * input, the self and the output sub-layers, which output the layer passes on.
 */
class CudaRnnLayer : public CudaLayer
{
 public:
  CudaRnnLayer(forward_fn forward, backward_fn backward, update_fn update);

  void place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
             int index) const override;
  void push(const layer& l) const override;
  void pull(const layer& l) const override;

 private:
  forward_fn mforward;
  backward_fn mbackward;
  update_fn mupdate;
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_RNN_LAYER_CLASS_H
