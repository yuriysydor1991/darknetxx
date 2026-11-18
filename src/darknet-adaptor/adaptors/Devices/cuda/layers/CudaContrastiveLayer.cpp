#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaContrastiveLayer.h"

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

CudaContrastiveLayer::CudaContrastiveLayer()
    : CudaOutputLayer{forward_contrastive_layer_gpu,
                      backward_contrastive_layer_gpu}
{
}

void CudaContrastiveLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                                 int index) const
{
  CudaOutputLayer::place(adaptor, net, index);

  layer& l = *adaptor.get();
  const size_t boxes = LayersBuffers::count(l.max_boxes, l.batch);

  CudaArrays{adaptor}.once(l.contrast_p_gpu, boxes * boxes);
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
