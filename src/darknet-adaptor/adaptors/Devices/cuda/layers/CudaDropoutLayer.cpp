#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaDropoutLayer.h"

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

void CudaDropoutLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                             int index) const
{
  layer& l = *adaptor.get();
  const layer& previous = net.get()->layers[index - 1];
  const CudaArrays arrays{adaptor};

  l.forward_gpu = forward_dropout_layer_gpu;
  l.backward_gpu = backward_dropout_layer_gpu;
  l.output_gpu = previous.output_gpu;
  l.delta_gpu = previous.delta_gpu;

  arrays.make(l.rand_gpu, l.rand, LayersBuffers::count(l.inputs, l.batch));

  if (l.dropblock) {
    arrays.pinned(l.drop_blocks_scale, LayersBuffers::count(l.batch));
    arrays.make(l.drop_blocks_scale_gpu, LayersBuffers::count(l.batch));
  }
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
