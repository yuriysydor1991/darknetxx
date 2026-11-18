#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_NETWORK_DEVICE_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_NETWORK_DEVICE_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/INetworkDevice.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/**
 * @brief The NVIDIA GPU: the device memory of a network follows the host one
 * the way the original darknet allocated it beside the host memory.
 */
class CudaNetworkDevice : public INetworkDevice
{
 public:
  /// @brief Starts the computation on the GPU of the gpu_index.
  CudaNetworkDevice();

  /// @brief Tells if the GPU of the gpu_index is present, turning the
  /// computation to the CPU (the gpu_index of -1) otherwise.
  static bool available();

  void select(int gpu) const override;
  void place(ANetworkAdaptor& net) const override;
  void push(const network& net) const override;
  void pull(const network& net) const override;

 private:
  /// @brief The half precision of the GPU of the network.
  static void describe(network& net);
  static void place_layers(ANetworkAdaptor& net);
  static void place_inputs(ANetworkAdaptor& net);
  static void place_workspace(ANetworkAdaptor& net);
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_NETWORK_DEVICE_CLASS_H
