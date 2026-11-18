#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_SHORTCUT_LAYER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_SHORTCUT_LAYER_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayer.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/**
 * @brief The [shortcut] layer: the weights of it's inputs and the device
 * arrays of the outputs and the deltas of the layers it adds.
 */
class CudaShortcutLayer : public CudaLayer
{
 public:
  void place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
             int index) const override;
  void push(const layer& l) const override;
  void pull(const layer& l) const override;
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_SHORTCUT_LAYER_CLASS_H
