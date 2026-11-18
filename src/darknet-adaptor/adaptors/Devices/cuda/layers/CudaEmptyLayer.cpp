#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaEmptyLayer.h"

extern "C" {

static void empty_gpu(layer l, network_state state)
{
  (void)l;
  (void)state;
}
}

namespace darknet_adaptor::adaptors::devices::cuda
{

void CudaEmptyLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                           int index) const
{
  layer& l = *adaptor.get();
  const layer& previous = net.get()->layers[index - 1];

  l.forward_gpu = empty_gpu;
  l.backward_gpu = empty_gpu;
  l.output_gpu = previous.output_gpu;
  l.delta_gpu = previous.delta_gpu;
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
