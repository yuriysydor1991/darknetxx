#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_EMPTY_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_EMPTY_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/// @brief The [empty] layer: the output and the delta of the previous layer.
class CudaEmptyLayer : public CudaLayer
{
 public:
  void place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
             int index) const override;
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_EMPTY_LAYER_CLASS_H
