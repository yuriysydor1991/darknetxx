#ifndef DARKNET_CXX_PORT_PROJECT_ICUDA_LAYER_INTERFACE_H
#define DARKNET_CXX_PORT_PROJECT_ICUDA_LAYER_INTERFACE_H

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/**
 * @brief The GPU memory of a layer kind: the port of the GPU parts of the
 * original make_*_layer(), resize_*_layer() and the weights files functions.
 */
class ICudaLayer
{
 public:
  virtual ~ICudaLayer() = default;

  /**
   * @brief Places the device memory of the made or resized layer the adaptor
   * keeps (the index one of the network): the arrays of the layer size are
   * made anew, the ones of the weights once.
   */
  virtual void place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                     int index) const = 0;

  /// @brief The cuDNN descriptors of it's own the layer of another network
  /// computes it's batch by.
  virtual void describe(layer& l, ANetworkAdaptor& net) const = 0;

  /// @brief Copies the weights of the host arrays to the device.
  virtual void push(const layer& l) const = 0;

  /// @brief Copies the weights of the device to the host arrays.
  virtual void pull(const layer& l) const = 0;
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_ICUDA_LAYER_INTERFACE_H
