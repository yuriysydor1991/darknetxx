#include "src/app/signals-handlers/handlers/posix/PosixSignalsHandler.h"

#include <signal.h>

namespace app
{

// The out of line destructor pins the class vtable to the current translation
// unit instead of emitting a copy of it into every including one.
PosixSignalsHandler::~PosixSignalsHandler() = default;

bool PosixSignalsHandler::is_ignored(int signalNumber)
{
  struct sigaction current{};

  // a null new action queries the disposition without changing it
  if (sigaction(signalNumber, nullptr, &current) != 0) {
    return false;
  }

  return (current.sa_flags & SA_SIGINFO) == 0 && current.sa_handler == SIG_IGN;
}

}  // namespace app
