#include "src/app/Application.h"

#include <fenv.h>
#include <unistd.h>

#include <cassert>
#include <cfenv>
#include <csignal>
#include <cstdlib>
#include <memory>

#include "src/OMP/OMPController.h"
#include "src/darknet-adaptor/DarknetController.h"
#include "src/log/log.h"

namespace app
{

#ifdef ENABLE_FP_TRAPS

namespace
{

/**
 * @brief Reports the trapped floating point exception and terminates the
 * process. Runs in the signal context of whichever thread computed, so it uses
 * the async signal safe calls only: no logging and no exit handlers.
 */
void fpe_handler(int)
{
  static const char message[] = "Floating point exception caught!\n";

  // nothing to do about a failed write of a dying process
  [[maybe_unused]] const ssize_t written =
      write(STDERR_FILENO, message, sizeof(message) - 1);

  _exit(EXIT_FAILURE);
}

}  // namespace

#endif  // ENABLE_FP_TRAPS

Application::Application() {}

int Application::run(AppCtxPtr ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid context pointer provided");
    return INVALID;
  }

  actx = ctx;

  init_computation_traps();

  if (!deal_omp(ctx)) {
    LOGE("Adjust OpenMP values failure");
    return INVALID;
  }

  auto darknet = darknet_adaptor::DarknetController::create();

  assert(darknet != nullptr);

  if (!darknet->perform(ctx)) {
    if (ctx->stop.load()) {
      return stopped_status(ctx);
    }

    LOGE("Fail to init the darknet");
    return INVALID;
  }

  // the work was cut short by a stop request, which is not a success
  if (ctx->stop.load()) {
    return stopped_status(ctx);
  }

  return 0;
}

int Application::stopped_status(AppCtxPtr ctx)
{
  // the exit status a shell reports for a process the signal killed
  static constexpr const int SIGNAL_EXIT_STATUS_BASE = 128;

  assert(ctx != nullptr);

  const int signalNumber = ctx->stop_signal.load();

  LOGW("The work is stopped by the signal " << signalNumber);

  return signalNumber > 0 ? SIGNAL_EXIT_STATUS_BASE + signalNumber : INVALID;
}

bool Application::deal_omp(AppCtxPtr ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid context pointer provided");
    return false;
  }

  auto omp = ompi::OMPController::create();

  assert(omp != nullptr);

  if (!omp->adjust(ctx)) {
    LOGE("Failure durng the OpenMP adjust calls");
    return false;
  }

  return true;
}

void Application::init_computation_traps()
{
#ifdef ENABLE_FP_TRAPS
  LOGD("Initing computation exceptions");

  std::signal(SIGFPE, fpe_handler);
  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif  // ENABLE_FP_TRAPS
}

}  // namespace app
