#include "src/app/signals-handlers/SignalsHandler.h"

#include <cassert>
#include <memory>
#include <vector>

#include "src/app/ApplicationContext.h"
#include "src/app/signals-handlers/ISignalsHandler.h"
#include "src/app/signals-handlers/handlers/posix/IgnoredSignalsHandler.h"
#include "src/app/signals-handlers/handlers/posix/StopSignalsHandler.h"
#include "src/log/log.h"

namespace app
{

SignalsHandler::~SignalsHandler() { SignalsHandler::uninstall(); }

bool SignalsHandler::install(std::shared_ptr<ApplicationContext> ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid application context provided");
    return false;
  }

  uninstall();

  mhandlers = create_handlers();

  if (mhandlers.empty()) {
    LOGE("No signals handlers created");
    return false;
  }

  bool anyInstalled{false};

  for (const std::shared_ptr<ISignalsHandler>& handler : mhandlers) {
    assert(handler != nullptr);

    if (handler == nullptr) {
      LOGE("An invalid signals handler met");
      continue;
    }

    anyInstalled = handler->install(ctx) || anyInstalled;
  }

  if (!anyInstalled) {
    LOGE("None of the signals handlers covers its signals");
    return false;
  }

  LOGD("Handling the OS signals with " << mhandlers.size() << " handlers");

  return true;
}

void SignalsHandler::uninstall()
{
  for (const std::shared_ptr<ISignalsHandler>& handler : mhandlers) {
    if (handler == nullptr) {
      continue;
    }

    handler->uninstall();
  }

  mhandlers.clear();
}

std::vector<std::shared_ptr<ISignalsHandler>> SignalsHandler::create_handlers()
{
  // Place here a subhandler of every OS signals category the application
  // covers. A handled signal loses its OS default disposition, so only the
  // categories the darknetxx acts on are covered: nothing polls the pause, the
  // reload and the user request flags, so the PauseSignalsHandler,
  // ContinueSignalsHandler, ReloadSignalsHandler and UserSignalsHandler ones
  // are left out on purpose. That keeps the shell job control working (Ctrl+Z
  // suspends, a background terminal access stops the job) and the user
  // signals terminating the process. The SIGHUP is a stop signal instead, see
  // the StopSignalsHandler class.
  return {
      std::make_shared<StopSignalsHandler>(),
      std::make_shared<IgnoredSignalsHandler>(),
  };
}

}  // namespace app
