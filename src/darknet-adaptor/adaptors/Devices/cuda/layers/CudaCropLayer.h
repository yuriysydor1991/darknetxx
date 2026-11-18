#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_CROP_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_CROP_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/// @brief The [crop] layer: the output and the random numbers of the batch.
class CudaCropLayer : public CudaLayer
{
 public:
  void place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
             int index) const override;
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_CROP_LAYER_CLASS_H
