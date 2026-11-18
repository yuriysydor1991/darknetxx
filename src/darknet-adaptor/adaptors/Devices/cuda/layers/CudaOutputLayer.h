#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_OUTPUT_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_OUTPUT_LAYER_CLASS_H

#include <tuple>
#include <vector>

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/// @brief The device array of the output size and the host one it copies
/// (none for the zeroed one).
using CudaMirror = std::tuple<float* layer::*, float* layer::*>;

/**
 * @brief The layer kind of the output and the delta device arrays of the
 * output size, the more arrays of that size it mirrors beside.
 */
class CudaOutputLayer : public CudaLayer
{
 public:
  CudaOutputLayer(forward_fn forward, backward_fn backward,
                  std::vector<CudaMirror> mirrors = {});

  void place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
             int index) const override;

 private:
  forward_fn mforward;
  backward_fn mbackward;
  std::vector<CudaMirror> mmirrors;
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_OUTPUT_LAYER_CLASS_H
