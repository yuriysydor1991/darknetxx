#include "src/app/signals-handlers/handlers/ASignalsHandler.h"

#include <cassert>
#include <csignal>
#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/log/log.h"

namespace app
{

bool ASignalsHandler::install(std::shared_ptr<ApplicationContext> ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid application context provided");
    return false;
  }

  const SignalDisposition signalDisposition = get_signal_disposition();

  assert(signalDisposition != nullptr);

  if (signalDisposition == nullptr) {
    LOGE("No valid signal disposition provided");
    return false;
  }

  uninstall();

  store_context(ctx.get());

  for (const int signalNumber : get_handled_signals()) {
    // The parent process kept the signal away from this one on purpose.
    if (signalDisposition != SIG_IGN && is_ignored(signalNumber)) {
      LOGD("Keeping the inherited ignore of the signal " << signalNumber);
      continue;
    }

    const SignalDisposition previousDisposition =
        std::signal(signalNumber, signalDisposition);

    if (previousDisposition == SIG_ERR) {
      LOGE("Fail to handle the signal " << signalNumber);
      continue;
    }

    // The same, recognized after the fact where the is_ignored method is
    // unable to query the disposition.
    if (previousDisposition == SIG_IGN && signalDisposition != SIG_IGN) {
      std::signal(signalNumber, SIG_IGN);
      LOGD("Keeping the inherited ignore of the signal " << signalNumber);
      continue;
    }

    minstalled_signals.emplace(signalNumber, previousDisposition);
  }

  if (minstalled_signals.empty()) {
    store_context(nullptr);
    return false;
  }

  LOGD("Handling " << minstalled_signals.size() << " OS signals");

  return true;
}

bool ASignalsHandler::is_ignored([[maybe_unused]] int signalNumber)
{
  return false;
}

void ASignalsHandler::uninstall()
{
  for (const auto& [signalNumber, previousDisposition] : minstalled_signals) {
    std::signal(signalNumber, previousDisposition);
  }

  minstalled_signals.clear();

  store_context(nullptr);
}

}  // namespace app
