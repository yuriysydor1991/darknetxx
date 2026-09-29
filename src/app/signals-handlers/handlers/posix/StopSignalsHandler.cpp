#include "src/app/signals-handlers/handlers/posix/StopSignalsHandler.h"

#include <time.h>

#include <atomic>
#include <csignal>
#include <cstdint>
#include <set>

#include "src/app/ApplicationContext.h"
#include "src/app/signals-handlers/handlers/posix/PosixSignalsHandler.h"

namespace app
{

namespace
{

/**
 * @brief Provides the CLOCK_MONOTONIC time in milliseconds, which is never a
 * zero one, since the zero value stands for no stop request. The clock_gettime
 * call is an async signal safe one.
 */
std::int64_t monotonic_milliseconds()
{
  static constexpr const std::int64_t msInSecond = 1000;
  static constexpr const std::int64_t nsInMs = 1000000;

  struct timespec now{};

  if (clock_gettime(CLOCK_MONOTONIC, &now) != 0) {
    return 1;
  }

  const std::int64_t milliseconds =
      static_cast<std::int64_t>(now.tv_sec) * msInSecond +
      static_cast<std::int64_t>(now.tv_nsec) / nsInMs;

  return milliseconds > 0 ? milliseconds : 1;
}

}  // namespace

StopSignalsHandler::~StopSignalsHandler() { StopSignalsHandler::uninstall(); }

const std::set<int>& StopSignalsHandler::get_handled_signals()
{
  // Place here the OS signals that are asking the application to stop. The
  // ones the C++ standard does not declare are guarded with the ifdef. The
  // darknetxx has no configuration to reload, so the SIGHUP of a closed
  // terminal stops it gracefully (saving the trained weights) instead of the
  // ReloadSignalsHandler category one.
  static const std::set<int> stopSignals{
      SIGINT,
      SIGTERM,
#ifdef SIGQUIT
      SIGQUIT,
#endif  // SIGQUIT
#ifdef SIGHUP
      SIGHUP,
#endif  // SIGHUP
  };

  return stopSignals;
}

SignalDisposition StopSignalsHandler::get_signal_disposition()
{
  return &StopSignalsHandler::stop_context;
}

void StopSignalsHandler::store_context(ApplicationContext* ctx)
{
  // every install and uninstall starts over with no stop request
  mfirst_request_time.store(0);
  mhandled_context.store(ctx);
}

void StopSignalsHandler::stop_context(int signalNumber)
{
  const std::int64_t now = monotonic_milliseconds();
  std::int64_t firstRequestTime = 0;

  // The first arrival is the graceful stop request.
  if (mfirst_request_time.compare_exchange_strong(firstRequestTime, now)) {
    ApplicationContext* const ctx = mhandled_context.load();

    if (ctx != nullptr) {
      ctx->stop_signal.store(signalNumber);
      ctx->stop.store(true);
    }

    return;
  }

  // A single request often arrives more than once - the timeout tool signals
  // the process and then its whole process group, a closed terminal sends the
  // SIGHUP twice - so only a repeat after the grace period terminates the
  // process right away, with the OS default disposition of the signal. Both
  // calls are allowed in the signal context, and the raised signal is
  // delivered as soon as the current routine returns.
  if (now - firstRequestTime < GRACE_PERIOD_MS) {
    return;
  }

  std::signal(signalNumber, SIG_DFL);
  std::raise(signalNumber);
}

}  // namespace app
