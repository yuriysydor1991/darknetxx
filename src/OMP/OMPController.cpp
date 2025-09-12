#include "src/OMP/OMPController.h"

#include <cassert>
#include <memory>

#ifdef OPENMP
#include <omp.h>
#endif  // OPENMP

#include "src/app/ApplicationContext.h"
#include "src/log/log.h"

namespace ompi
{

bool OMPController::adjust(AppCtxPtr ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("Invalid context pointer provided");
    return false;
  }

  if (!adjust_threads(ctx->threads)) {
    LOGE("Failure during the threads set");
    return false;
  }

  return true;
}

OMPControllerPtr OMPController::create()
{
  return std::make_shared<OMPController>();
}

bool OMPController::adjust_threads(const unsigned int& maxThreads)
{
#ifdef OPENMP
  if (maxThreads == 0U) {
    LOGD("No valid threads value provided, skipping");
    return true;
  }

  omp_set_num_threads(maxThreads);

  LOGD("Adjusted threads count to " << maxThreads);
#else   // OPENMP
  LOGD("No compiled in support for the OpenMP");
#endif  // OPENMP

  return true;
}

}  // namespace ompi
