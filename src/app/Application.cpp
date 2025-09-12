#include "src/app/Application.h"

#include <fenv.h>

#include <cassert>
#include <cfenv>
#include <csignal>
#include <cstring>
#include <iostream>
#include <memory>

#include "src/OMP/OMPController.h"
#include "src/darknet-adaptor/DarknetController.h"
#include "src/log/log.h"

namespace app
{

namespace
{

void fpe_handler(int)
{
  LOGE("Floating point exception caught!");
  std::exit(1);
}

}  // namespace

Application::Application() {}

int Application::run(AppCtxPtr ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid context pointer provided");
    return INVALID;
  }

  if (apt == nullptr) {
    apt = shared_from_this();

    if (!init_signals()) {
      LOGE("Failure during signals init");
      return INVALID;
    }
  } else {
    LOGW("Application instance is already created");
  }

  actx = ctx;

  if (!deal_omp(ctx)) {
    LOGE("Adjust OpenMP values failure");
    return INVALID;
  }

  auto darknet = darknet_adaptor::DarknetController::create();

  assert(darknet != nullptr);

  if (!darknet->perform(ctx)) {
    LOGE("Fail to init the darknet");
    return INVALID;
  }

  return 0;
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

bool Application::stop_catch()
{
  if (actx == nullptr) {
    LOGW("No application context available");
    return false;
  }

  LOGD("Setting up stop flag");
  actx->stop.store(true);

  for (auto& sub : actx->stop_subscribers) {
    if (sub != nullptr) {
      auto f = *sub;
      if (f != nullptr) {
        f();
      }
    }
  }

  return true;
}

bool Application::init_signals()
{
  static std::shared_ptr<struct sigaction> sa;

  if (sa != nullptr) {
    LOGT("Signals already initialized");
    return true;
  }

  sa = std::make_shared<struct sigaction>();

  sa->sa_handler = &Application::handle_sigint;
  sigemptyset(&sa->sa_mask);
  sa->sa_flags = 0;

  if (sigaction(SIGINT, sa.get(), nullptr) == -1) {
    const auto err = errno;
    LOGE("Failure during sigaction: " << std::strerror(err));
    return false;
  }

  if (sigaction(SIGTERM, sa.get(), nullptr) == -1) {
    const auto err = errno;
    LOGE("Failure during sigaction: " << std::strerror(err));
    return false;
  }

#ifdef DEBUG
  LOGD("Initing computation exceptions");

  std::signal(SIGFPE, fpe_handler);
  feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif  // DEBUG

  return true;
}

void Application::handle_sigint([[maybe_unused]] int sig)
{
  if (apt == nullptr) {
    LOGE("No application instance declared");
    return;
  }

  apt->stop_catch();
}

}  // namespace app
