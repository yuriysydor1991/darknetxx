#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_CONV_LSTM_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_CONV_LSTM_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/**
 * @brief The [conv_lstm] layer: the gates of it's convolutional sub-layers,
 * the peephole and the bottleneck ones of the layer of them alone.
 */
class CudaConvLstmLayer : public CudaLayer
{
 public:
  void place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
             int index) const override;
  void push(const layer& l) const override;
  void pull(const layer& l) const override;
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_CONV_LSTM_LAYER_CLASS_H
