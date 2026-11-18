#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaMaxpoolLayer.h"

#include <algorithm>

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/CudnnDescriptors.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

void CudaMaxpoolLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                             int index) const
{
  layer& l = *adaptor.get();
  const CudaArrays arrays{adaptor};
  const size_t size = LayersBuffers::count(l.outputs, l.batch);

  l.forward_gpu =
      l.avgpool ? forward_local_avgpool_layer_gpu : forward_maxpool_layer_gpu;
  l.backward_gpu =
      l.avgpool ? backward_local_avgpool_layer_gpu : backward_maxpool_layer_gpu;

  if (l.indexes) arrays.make(l.indexes_gpu, size);
  if (l.delta) arrays.make(l.delta_gpu, l.delta, size);

  arrays.make(l.output_gpu, l.output, size);

  describe(l, net);

  // the blur sub-layer of the antialiasing computes in it's own arrays
  if (l.antialiasing) {
    arrays.make(l.input_antialiasing_gpu, size);
    l.workspace_size = std::max(l.workspace_size,
                                place_all({adaptor.input_layer()}, net, index));
  }
}

void CudaMaxpoolLayer::describe(layer& l, ANetworkAdaptor& net) const
{
  if (!l.poolingDesc) {
    create_maxpool_cudnn_tensors(&l);
    CudnnDescriptors::keep(net, l);
  }

  if (l.avgpool) {
    cudnn_local_avgpool_setup(&l);
  } else {
    cudnn_maxpool_setup(&l);
  }
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
