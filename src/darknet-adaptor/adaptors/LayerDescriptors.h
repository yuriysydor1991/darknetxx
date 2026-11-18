#ifndef DARKNET_CXX_PORT_PROJECT_LAYER_DESCRIPTORS_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_LAYER_DESCRIPTORS_CLASS_H

#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The cuDNN descriptors and algorithms of a layer struct, the batch
 * dependent description of it's computation (the void pointers and the zeros
 * of the builds of no cuDNN).
 */
class LayerDescriptors
{
 public:
  static void copy(const layer& source, layer& target);
};

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_LAYER_DESCRIPTORS_CLASS_H
