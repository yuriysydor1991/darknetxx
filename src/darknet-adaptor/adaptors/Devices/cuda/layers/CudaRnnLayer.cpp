#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaRnnLayer.h"

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

CudaRnnLayer::CudaRnnLayer(forward_fn forward, backward_fn backward,
                           update_fn update)
    : mforward{forward}, mbackward{backward}, mupdate{update}
{
}

void CudaRnnLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                         int index) const
{
  layer& l = *adaptor.get();

  l.forward_gpu = mforward;
  l.backward_gpu = mbackward;
  l.update_gpu = mupdate;

  CudaArrays{adaptor}.make(
      l.state_gpu, l.state,
      LayersBuffers::count(l.batch, l.hidden, l.steps + 1));

  l.workspace_size = place_all(
      {adaptor.input_layer(), adaptor.self_layer(), adaptor.output_layer()},
      net, index);

  l.output_gpu = l.output_layer->output_gpu;
  l.delta_gpu = l.output_layer->delta_gpu;
}

void CudaRnnLayer::push(const layer& l) const
{
  push_all({l.input_layer, l.self_layer, l.output_layer});
}

void CudaRnnLayer::pull(const layer& l) const
{
  pull_all({l.input_layer, l.self_layer, l.output_layer});
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
