#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaNetworkInputs.h"

#include <algorithm>

namespace darknet_adaptor::adaptors::devices::cuda
{

CudaNetworkInputs::CudaNetworkInputs(network& net)
{
  net.input_gpu = &input;
  net.truth_gpu = &truth;
  net.input16_gpu = &input16;
  net.output16_gpu = &output16;
  net.max_input16_size = &max_input16;
  net.max_output16_size = &max_output16;

  if (!net.cudnn_half) {
    return;
  }

  for (int i = 0; i < net.n; ++i) {
    max_input16 =
        std::max(max_input16, static_cast<size_t>(net.layers[i].inputs));
    max_output16 =
        std::max(max_output16, static_cast<size_t>(net.layers[i].outputs));
  }

  // the half precision elements, as the original parser allocated them
  CHECK_CUDA(cudaMalloc(&input16, max_input16 * sizeof(short)));
  CHECK_CUDA(cudaMalloc(&output16, max_output16 * sizeof(short)));
}

CudaNetworkInputs::~CudaNetworkInputs()
{
  for (float* array : {input, truth, input16, output16}) {
    cudaFree(array);
  }
}

void CudaNetworkInputs::reset(network& net)
{
  for (float** array : {net.input_gpu, net.truth_gpu}) {
    cudaFree(*array);
    *array = nullptr;
  }
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
