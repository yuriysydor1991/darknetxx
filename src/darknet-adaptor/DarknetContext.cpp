#include "src/darknet-adaptor/DarknetContext.h"

#include <cassert>
#include <memory>

#include "src/app/ApplicationContext.h"

namespace darknet_adaptor
{

DarknetContext::DarknetContext(AppCtxPtr nactx) : actx{nactx}
{
  assert(actx != nullptr);
}

}  // namespace darknet_adaptor
