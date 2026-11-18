#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayer.h"

#include <algorithm>

#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

void CudaLayer::describe([[maybe_unused]] layer& l,
                         [[maybe_unused]] ANetworkAdaptor& net) const
{
}

void CudaLayer::push([[maybe_unused]] const layer& l) const {}

void CudaLayer::pull([[maybe_unused]] const layer& l) const {}

size_t CudaLayer::place_all(std::initializer_list<ALayerAdaptorPtr> sublayers,
                            ANetworkAdaptor& net, int index)
{
  size_t workspace = 0U;

  for (const ALayerAdaptorPtr& sublayer : sublayers) {
    CudaLayers::of(sublayer->type()).place(*sublayer, net, index);
    workspace = std::max(workspace, sublayer->get()->workspace_size);
  }

  return workspace;
}

void CudaLayer::push_all(std::initializer_list<const layer*> sublayers)
{
  for (const layer* sublayer : sublayers) {
    CudaLayers::of(sublayer->type).push(*sublayer);
  }
}

void CudaLayer::pull_all(std::initializer_list<const layer*> sublayers)
{
  for (const layer* sublayer : sublayers) {
    CudaLayers::of(sublayer->type).pull(*sublayer);
  }
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
