#ifndef DARKNET_CXX_PORT_PROJECT_NETWORK_DEVICES_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NETWORK_DEVICES_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/INetworkDevice.h"

namespace darknet_adaptor::adaptors::devices
{

/// @brief The devices of the networks.
class NetworkDevices
{
 public:
  /// @brief The GPU of the GPU build on the machine of one, the host otherwise.
  static const INetworkDevice& current();

  static const INetworkDevice& host();
};

}  // namespace darknet_adaptor::adaptors::devices

#endif  // DARKNET_CXX_PORT_PROJECT_NETWORK_DEVICES_CLASS_H
