#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaOutputLayer.h"

#include <utility>

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

CudaOutputLayer::CudaOutputLayer(forward_fn forward, backward_fn backward,
                                 std::vector<CudaMirror> mirrors)
    : mforward{forward}, mbackward{backward}, mmirrors{std::move(mirrors)}
{
}

void CudaOutputLayer::place(ALayerAdaptor& adaptor,
                            [[maybe_unused]] ANetworkAdaptor& net,
                            [[maybe_unused]] int index) const
{
  layer& l = *adaptor.get();
  const CudaArrays arrays{adaptor};
  const size_t size = LayersBuffers::count(l.outputs, l.batch);

  l.forward_gpu = mforward;
  l.backward_gpu = mbackward;

  arrays.make(l.output_gpu, l.output, size);
  arrays.make(l.delta_gpu, l.delta, size);

  for (const auto& [device, host] : mmirrors) {
    arrays.make(l.*device, host ? l.*host : nullptr, size);
  }
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
