#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_STOPSIGNALSHANDLER_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_STOPSIGNALSHANDLER_CLASS_H

#include <atomic>
#include <cstdint>
#include <set>

#include "src/app/ApplicationContext.h"
#include "src/app/signals-handlers/handlers/posix/PosixSignalsHandler.h"

namespace app
{

/**
 * @brief The handler of the OS signals which are asking the process to
 * terminate, like the SIGINT one of the Ctrl+C keys combination.
 *
 * Every covered signal raises the ApplicationContext::stop flag of the
 * application context and keeps its number in the
 * ApplicationContext::stop_signal one, so the application is expected to poll
 * the flag and to return from its IApplication::run implementation as soon as
 * the flag is raised.
 *
 * The first arrival of a signal is the graceful stop request only. The copies
 * of the signal arriving within the StopSignalsHandler::GRACE_PERIOD_MS
 * period belong to the very same request (the timeout tool, for example,
 * signals the process and then its whole process group), while a repeated
 * signal arriving later (the second Ctrl+C, for example) terminates the
 * process right away with the OS default disposition of the signal.
 *
 * The darknet_adaptor::DarknetContext polls the very same flag in it's own
 * stop watcher and mirrors it into the original Darknet computation context,
 * which is how the C training and detection loops learn about the request. See
 * the DarknetContext::watch_the_app_stop_flag method.
 *
 * The std::signal dispositions are the process wide ones, so a single alive
 * instance is expected and the context of the latest ISignalsHandler::install
 * call is the one that gets stopped.
 */
class StopSignalsHandler : public PosixSignalsHandler
{
 public:
  /// @brief The period after the first stop request, in milliseconds, which
  /// the repeated stop signals are treated as the copies of that request
  /// within.
  static constexpr const std::int64_t GRACE_PERIOD_MS{1000};

  /// @brief Releases the covered signals with the inherited
  /// ASignalsHandler::uninstall method.
  ~StopSignalsHandler() override;
  StopSignalsHandler() = default;

 protected:
  /// @brief Provides the OS signals numbers that are asking the application to
  /// stop. See the ASignalsHandler class.
  virtual const std::set<int>& get_handled_signals() override;

  /// @brief Provides the StopSignalsHandler::stop_context routine. See the
  /// ASignalsHandler class.
  virtual SignalDisposition get_signal_disposition() override;

  /// @brief Keeps the given context in the
  /// StopSignalsHandler::mhandled_context field and forgets the time of the
  /// previous stop request. See the ASignalsHandler class.
  virtual void store_context(ApplicationContext* ctx) override;

  /**
   * @brief The routine the OS calls in the signal context, so it reads the
   * monotonic clock, touches the lock free atomics and, for a repeated signal
   * after the grace period, restores the OS default disposition and raises the
   * signal again only. It does no logging, since nothing else is async signal
   * safe.
   *
   * @param signalNumber The arrived signal number given by the OS.
   */
  static void stop_context(int signalNumber);

 private:
  /// @brief The context to stop, kept as a plain pointer, since neither the
  /// std::signal handler accepts a user data argument nor the std::shared_ptr
  /// is async signal safe.
  inline static std::atomic<ApplicationContext*> mhandled_context{nullptr};

  /// @brief The CLOCK_MONOTONIC time of the first stop request arrival in
  /// milliseconds, or zero while there is no request.
  inline static std::atomic<std::int64_t> mfirst_request_time{0};

  static_assert(std::atomic<std::int64_t>::is_always_lock_free,
                "The signal handler may touch the lock free atomics only");
};

}  // namespace app

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_STOPSIGNALSHANDLER_CLASS_H
