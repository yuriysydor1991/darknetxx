#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaImplicitLayer.h"

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

CudaImplicitLayer::CudaImplicitLayer()
    : CudaOutputLayer{forward_implicit_layer_gpu, backward_implicit_layer_gpu}
{
}

void CudaImplicitLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                              int index) const
{
  CudaOutputLayer::place(adaptor, net, index);

  layer& l = *adaptor.get();
  const CudaArrays arrays{adaptor};
  const size_t weights = LayersBuffers::count(l.nweights);

  l.update_gpu = update_implicit_layer_gpu;

  arrays.once(l.weights_gpu, l.weights, weights);
  arrays.once(l.weight_updates_gpu, l.weight_updates, weights);
}

void CudaImplicitLayer::push(const layer& l) const { push_implicit_layer(l); }

void CudaImplicitLayer::pull(const layer& l) const { pull_implicit_layer(l); }

}  // namespace darknet_adaptor::adaptors::devices::cuda
