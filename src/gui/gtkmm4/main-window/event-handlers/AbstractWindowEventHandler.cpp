#include "src/gui/gtkmm4/main-window/event-handlers/AbstractWindowEventHandler.h"

#include <cassert>

namespace darknetxxGtkmm4::window::event_handlers
{

AbstractWindowEventHandler::AbstractWindowEventHandler(
    std::shared_ptr<MainWindowContext> nmwctx)
    : mwctx{nmwctx}
{
  assert(mwctx != nullptr);
}

}  // namespace darknetxxGtkmm4::window::event_handlers
