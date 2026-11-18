#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_CONNECTED_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_CONNECTED_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/// @brief The [connected] layer, the sub-layer of the recurrent ones too.
class CudaConnectedLayer : public CudaLayer
{
 public:
  void place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
             int index) const override;
  void describe(layer& l, ANetworkAdaptor& net) const override;
  void push(const layer& l) const override;
  void pull(const layer& l) const override;
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_CONNECTED_LAYER_CLASS_H
