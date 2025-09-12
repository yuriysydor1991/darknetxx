#include "src/darknet-adaptor/adaptors/StructAdaptorHelper.h"

#include <memory>
#include <vector>

#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors
{

void StructAdaptorHelper::place_pointers(vrfloats& dst, vvfloats& psrc)
{
  dst.clear();
  dst.resize(psrc.size(), NULL);
  auto vriter = dst.begin();

  for (auto& vv : psrc) {
    *(vriter++) = vv.data();
  }
}

}  // namespace darknet_adaptor::adaptors
