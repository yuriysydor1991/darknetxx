#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLocalLayer.h"

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

void CudaLocalLayer::place(ALayerAdaptor& adaptor,
                           [[maybe_unused]] ANetworkAdaptor& net,
                           [[maybe_unused]] int index) const
{
  layer& l = *adaptor.get();
  const CudaArrays arrays{adaptor};
  const size_t locations = LayersBuffers::count(l.out_h, l.out_w);
  const size_t weights =
      LayersBuffers::count(l.c, l.n, l.size, l.size) * locations;
  const size_t size = LayersBuffers::count(l.batch, l.out_h, l.out_w, l.n);

  l.forward_gpu = forward_local_layer_gpu;
  l.backward_gpu = backward_local_layer_gpu;
  l.update_gpu = update_local_layer_gpu;

  arrays.once(l.weights_gpu, l.weights, weights);
  arrays.once(l.weight_updates_gpu, l.weight_updates, weights);
  arrays.once(l.biases_gpu, l.biases, LayersBuffers::count(l.outputs));
  arrays.once(l.bias_updates_gpu, l.bias_updates,
              LayersBuffers::count(l.outputs));
  arrays.make(l.col_image_gpu, l.col_image,
              LayersBuffers::count(l.size, l.size, l.c) * locations);
  arrays.make(l.delta_gpu, l.delta, size);
  arrays.make(l.output_gpu, l.output, size);
}

void CudaLocalLayer::push(const layer& l) const { push_local_layer(l); }

void CudaLocalLayer::pull(const layer& l) const { pull_local_layer(l); }

}  // namespace darknet_adaptor::adaptors::devices::cuda
