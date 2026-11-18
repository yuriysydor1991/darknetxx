#ifndef DARKNET_CXX_PORT_PROJECT_HOST_NETWORK_DEVICE_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_HOST_NETWORK_DEVICE_CLASS_H

#include "src/darknet-adaptor/adaptors/Devices/INetworkDevice.h"

namespace darknet_adaptor::adaptors::devices
{

/// @brief The CPU: the network memory stays on the host alone.
class HostNetworkDevice : public INetworkDevice
{
 public:
  void select(int gpu) const override;
  void place(ANetworkAdaptor& net) const override;
  void push(const network& net) const override;
  void pull(const network& net) const override;
};

}  // namespace darknet_adaptor::adaptors::devices

#endif  // DARKNET_CXX_PORT_PROJECT_HOST_NETWORK_DEVICE_CLASS_H
