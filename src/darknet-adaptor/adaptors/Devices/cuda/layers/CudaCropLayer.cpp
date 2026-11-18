#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaCropLayer.h"

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

void CudaCropLayer::place(ALayerAdaptor& adaptor,
                          [[maybe_unused]] ANetworkAdaptor& net,
                          [[maybe_unused]] int index) const
{
  layer& l = *adaptor.get();
  const CudaArrays arrays{adaptor};

  l.forward_gpu = forward_crop_layer_gpu;
  l.backward_gpu = backward_crop_layer_gpu;

  arrays.make(l.output_gpu, l.output, LayersBuffers::count(l.outputs, l.batch));
  arrays.once(l.rand_gpu, LayersBuffers::count(l.batch, 8));
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
