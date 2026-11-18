#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_MAXPOOL_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_MAXPOOL_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/// @brief The [maxpool] and the [local_avgpool] layers.
class CudaMaxpoolLayer : public CudaLayer
{
 public:
  void place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
             int index) const override;
  void describe(layer& l, ANetworkAdaptor& net) const override;
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_MAXPOOL_LAYER_CLASS_H
