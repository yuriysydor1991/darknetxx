#include "src/app/applications/Application.h"

#include <fenv.h>
#include <unistd.h>

#include <cassert>
#include <cfenv>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <vector>

#include "LibraryFacade.h"
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

  return perform(ctx);
}

int Application::perform(AppCtxPtr ctx)
{
  assert(ctx != nullptr);

  if (ctx->chart_requested()) {
    return draw_chart(ctx);
  }

  auto libctx = create_and_convert_libctx(ctx);

  if (libctx == nullptr) {
    return INVALID;
  }

  auto lib = darknetxx011::LibraryFacade::create_library(libctx);

  // the library tells the reason
  if (lib == nullptr) {
    return INVALID;
  }

  const bool performed = lib->libcall(libctx);

  // the work was cut short by a stop request, which is not a success
  if (ctx->stop.load()) {
    return stopped_status(ctx);
  }

  if (!performed) {
    LOGE("The library work failed");
    return INVALID;
  }

  return 0;
}

int Application::draw_chart(AppCtxPtr ctx)
{
  const std::vector<unsigned char> png =
      darknetxx011::LibraryFacade::training_chart(ctx->latest_weights);

  if (png.empty()) {
    LOGE("No training chart of the weights file " << ctx->latest_weights);
    return INVALID;
  }

  std::ofstream file{ctx->draw_chart, std::ios::binary};

  file.write(reinterpret_cast<const char*>(png.data()),
             static_cast<std::streamsize>(png.size()));

  if (!file.flush()) {
    LOGE("Fail to write the training chart " << ctx->draw_chart);
    return INVALID;
  }

  LOGI("The training chart of " << ctx->latest_weights << " is drawn into "
                                << ctx->draw_chart);

  return 0;
}

Application::LibraryContextPtr Application::create_and_convert_libctx(
    AppCtxPtr ctx)
{
  auto libctx = darknetxx011::LibraryFacade::create_library_context();
  auto converter = create_ctx_converter();

  assert(libctx != nullptr);
  assert(converter != nullptr);

  if (libctx == nullptr || converter == nullptr ||
      !converter->convert(ctx, libctx)) {
    LOGE("Fail to prepare the library context");
    return {};
  }

  return libctx;
}

Application::CtxConverterPtr Application::create_ctx_converter()
{
  return std::make_shared<converters::ApplicationContext2LibraryContext>();
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

void Application::init_computation_traps()
{
#ifdef ENABLE_FP_TRAPS
  LOGD("Initing computation exceptions");

  std::signal(SIGFPE, fpe_handler);
  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif  // ENABLE_FP_TRAPS
}

}  // namespace app
