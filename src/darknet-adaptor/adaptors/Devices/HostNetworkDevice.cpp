#include "src/darknet-adaptor/adaptors/Devices/HostNetworkDevice.h"

#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"

namespace darknet_adaptor::adaptors::devices
{

void HostNetworkDevice::select([[maybe_unused]] int gpu) const {}

void HostNetworkDevice::place(ANetworkAdaptor& net) const
{
  net.workspace_bytes(net.layers_workspace());
}

void HostNetworkDevice::push([[maybe_unused]] const network& net) const {}

void HostNetworkDevice::pull([[maybe_unused]] const network& net) const {}

}  // namespace darknet_adaptor::adaptors::devices
