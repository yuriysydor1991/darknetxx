#include "src/darknet-adaptor/adaptors/LayerDescriptors.h"

namespace darknet_adaptor::adaptors
{

void LayerDescriptors::copy(const layer& source, layer& target)
{
  target.srcTensorDesc = source.srcTensorDesc;
  target.dstTensorDesc = source.dstTensorDesc;
  target.srcTensorDesc16 = source.srcTensorDesc16;
  target.dstTensorDesc16 = source.dstTensorDesc16;
  target.dsrcTensorDesc = source.dsrcTensorDesc;
  target.ddstTensorDesc = source.ddstTensorDesc;
  target.dsrcTensorDesc16 = source.dsrcTensorDesc16;
  target.ddstTensorDesc16 = source.ddstTensorDesc16;
  target.normTensorDesc = source.normTensorDesc;
  target.normDstTensorDesc = source.normDstTensorDesc;
  target.normDstTensorDescF16 = source.normDstTensorDescF16;
  target.weightDesc = source.weightDesc;
  target.weightDesc16 = source.weightDesc16;
  target.dweightDesc = source.dweightDesc;
  target.dweightDesc16 = source.dweightDesc16;
  target.convDesc = source.convDesc;
  target.poolingDesc = source.poolingDesc;
  target.fw_algo = source.fw_algo;
  target.fw_algo16 = source.fw_algo16;
  target.bd_algo = source.bd_algo;
  target.bd_algo16 = source.bd_algo16;
  target.bf_algo = source.bf_algo;
  target.bf_algo16 = source.bf_algo16;
}

}  // namespace darknet_adaptor::adaptors
