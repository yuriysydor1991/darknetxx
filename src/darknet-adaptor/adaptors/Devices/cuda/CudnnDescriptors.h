#ifndef DARKNET_CXX_PORT_PROJECT_CUDNN_DESCRIPTORS_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDNN_DESCRIPTORS_CLASS_H

#include "src/darknet-adaptor/adaptors/ResourcesKeeper.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/// @brief The cuDNN descriptors of the layer structs the network keeps.
class CudnnDescriptors
{
 public:
  /// @brief Keeps every descriptor of the layer, right after they're created.
  static void keep(ResourcesKeeper& keeper, const layer& l);
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDNN_DESCRIPTORS_CLASS_H
