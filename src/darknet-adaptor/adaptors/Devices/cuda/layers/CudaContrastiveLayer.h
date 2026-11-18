#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_CONTRASTIVE_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_CONTRASTIVE_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaOutputLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/// @brief The [contrastive] layer: the params of every pair of it's boxes.
class CudaContrastiveLayer : public CudaOutputLayer
{
 public:
  CudaContrastiveLayer();

  void place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
             int index) const override;
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_CONTRASTIVE_LAYER_CLASS_H
