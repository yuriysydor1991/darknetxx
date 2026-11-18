#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_IMPLICIT_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_IMPLICIT_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaOutputLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/// @brief The [implicit_*] layer: the trained weights of it's output.
class CudaImplicitLayer : public CudaOutputLayer
{
 public:
  CudaImplicitLayer();

  void place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
             int index) const override;
  void push(const layer& l) const override;
  void pull(const layer& l) const override;
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_IMPLICIT_LAYER_CLASS_H
