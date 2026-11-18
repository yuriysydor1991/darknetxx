#include "src/darknet-adaptor/adaptors/Devices/NetworkDevices.h"

#include "src/darknet-adaptor/adaptors/Devices/HostNetworkDevice.h"

#ifdef GPU
#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaNetworkDevice.h"
#endif

namespace darknet_adaptor::adaptors::devices
{

const INetworkDevice& NetworkDevices::current()
{
#ifdef GPU
  if (cuda::CudaNetworkDevice::available()) {
    static const cuda::CudaNetworkDevice gpu;
    return gpu;
  }
#endif

  return host();
}

const INetworkDevice& NetworkDevices::host()
{
  static const HostNetworkDevice device;
  return device;
}

}  // namespace darknet_adaptor::adaptors::devices
