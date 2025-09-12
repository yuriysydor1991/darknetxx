#ifndef DARKNET_CXX_PORT_PROJECT_STRUCT_ADAPTOR_PARENT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_STRUCT_ADAPTOR_PARENT_CLASS_H

#include <memory>
#include <vector>

#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The helper class for the structur adaptor classes.
 */
class StructAdaptorHelper
{
 public:
  using networkptr = std::shared_ptr<network>;
  using layerptr = std::shared_ptr<struct layer>;
  using vvfloats = std::vector<std::vector<float>>;
  using vcontrastive_params = std::vector<contrastive_params>;
  using vrfloats = std::vector<float*>;

  virtual ~StructAdaptorHelper() = default;
  StructAdaptorHelper() = default;

  void place_pointers(vrfloats& dst, vvfloats& psrc);
};

using layerptr = StructAdaptorHelper::layerptr;
using vvfloats = StructAdaptorHelper::vvfloats;
using vrfloats = StructAdaptorHelper::vrfloats;
using vcontrastive_params = StructAdaptorHelper::vcontrastive_params;

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_ALAYERADAPTOR_CLASS_H
