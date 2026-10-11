#include "src/lib/libmain/LibMain.h"

#include <cassert>

#include "src/OMP/OMPController.h"
#include "src/darknet-adaptor/DarknetController.h"
#include "src/log/log.h"

namespace darknetxx_impl
{

bool LibMain::libcall(LibraryContextPtr ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid library context provided");
    return false;
  }

  if (!adjust_threads(ctx)) {
    LOGE("Fail to adjust the OpenMP threads");
    return false;
  }

  return darknet_adaptor::DarknetController::create()->perform(ctx);
}

bool LibMain::adjust_threads(LibraryContextPtr ctx)
{
  return ompi::OMPController::create()->adjust(ctx);
}

}  // namespace darknetxx_impl
