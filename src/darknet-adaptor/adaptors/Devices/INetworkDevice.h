#ifndef DARKNET_CXX_PORT_PROJECT_INETWORK_DEVICE_INTERFACE_H
#define DARKNET_CXX_PORT_PROJECT_INETWORK_DEVICE_INTERFACE_H

#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors
{
class ANetworkAdaptor;
}

namespace darknet_adaptor::adaptors::devices
{

/**
 * @brief The device the networks compute on. The host one keeps the memory
 * of a network on the host alone, the GPU one keeps the device copy of it,
 * which follows the memory the adaptors make, resize and fill.
 */
class INetworkDevice
{
 public:
  virtual ~INetworkDevice() = default;

  /// @brief Makes the GPU of the index the one the next networks are placed on.
  virtual void select(int gpu) const = 0;

  /// @brief Places the made, resized or rebatched memory of the network: the
  /// one of it's layers and the workspace.
  virtual void place(ANetworkAdaptor& net) const = 0;

  /// @brief Copies the weights of the host arrays to the device.
  virtual void push(const network& net) const = 0;

  /// @brief Copies the weights of the device to the host arrays.
  virtual void pull(const network& net) const = 0;
};

}  // namespace darknet_adaptor::adaptors::devices

#endif  // DARKNET_CXX_PORT_PROJECT_INETWORK_DEVICE_INTERFACE_H
