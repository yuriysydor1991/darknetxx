#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_NETWORK_INPUTS_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_NETWORK_INPUTS_CLASS_H

#include <cstddef>

#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/**
 * @brief The slots of the device arrays the darknet GPU code allocates itself
 * (the training input and truth, the half precision input and output of the
 * convolutions), which the network fields point at.
 */
class CudaNetworkInputs
{
 public:
  /// @brief Points the network at the slots, the half precision arrays
  /// allocated for the biggest layer of the cudnn_half network.
  explicit CudaNetworkInputs(network& net);
  ~CudaNetworkInputs();
  CudaNetworkInputs(const CudaNetworkInputs&) = delete;
  CudaNetworkInputs& operator=(const CudaNetworkInputs&) = delete;

  /// @brief Frees the training input and truth of the previous network size,
  /// the next training batch allocates them anew.
  static void reset(network& net);

 private:
  float* input{nullptr};
  float* truth{nullptr};
  float* input16{nullptr};
  float* output16{nullptr};
  size_t max_input16{0U};
  size_t max_output16{0U};
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_NETWORK_INPUTS_CLASS_H
