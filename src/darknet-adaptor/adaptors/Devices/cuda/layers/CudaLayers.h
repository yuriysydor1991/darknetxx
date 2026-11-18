#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_LAYERS_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_LAYERS_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/ICudaLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/// @brief The GPU memory of the layer kinds.
class CudaLayers
{
 public:
  /// @brief The kind of the layer type, throws NetworkError for no such one.
  static const ICudaLayer& of(LAYER_TYPE type);
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_LAYERS_CLASS_H
