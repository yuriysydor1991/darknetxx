#include "src/darknet-adaptor/adaptors/Devices/cuda/CudnnDescriptors.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

void CudnnDescriptors::keep([[maybe_unused]] ResourcesKeeper& keeper,
                            [[maybe_unused]] const layer& l)
{
#ifdef CUDNN
  using Resource = ResourcesKeeper::Resource;

  for (cudnnTensorDescriptor_t tensor :
       {l.srcTensorDesc, l.dstTensorDesc, l.srcTensorDesc16, l.dstTensorDesc16,
        l.dsrcTensorDesc, l.ddstTensorDesc, l.dsrcTensorDesc16,
        l.ddstTensorDesc16, l.normTensorDesc, l.normDstTensorDesc,
        l.normDstTensorDescF16}) {
    if (tensor) keeper.keep(Resource(tensor, cudnnDestroyTensorDescriptor));
  }

  for (cudnnFilterDescriptor_t filter :
       {l.weightDesc, l.weightDesc16, l.dweightDesc, l.dweightDesc16}) {
    if (filter) keeper.keep(Resource(filter, cudnnDestroyFilterDescriptor));
  }

  if (l.convDesc) {
    keeper.keep(Resource(l.convDesc, cudnnDestroyConvolutionDescriptor));
  }

  if (l.poolingDesc) {
    keeper.keep(Resource(l.poolingDesc, cudnnDestroyPoolingDescriptor));
  }
#endif  // CUDNN
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
