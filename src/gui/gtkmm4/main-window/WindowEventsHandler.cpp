#include "src/gui/gtkmm4/main-window/WindowEventsHandler.h"

#include "src/gui/gtkmm4/main-window/event-handlers/FolderEventsHandler.h"
#include "src/gui/gtkmm4/main-window/event-handlers/TrainingEventsHandler.h"
#include "src/gui/gtkmm4/main-window/event-handlers/WindowSystemEventsHandler.h"
#include "src/log/log.h"

namespace darknetxxGtkmm4::window
{

bool WindowEventsHandler::init(std::shared_ptr<MainWindowContext> nmwctx)
{
  handlers = create_handlers(nmwctx);

  for (const auto& handler : handlers) {
    if (!handler->subscribe()) {
      LOGE("Fail to connect an event handler to the window");
      return false;
    }
  }

  return true;
}

WindowEventsHandler::Handlers WindowEventsHandler::create_handlers(
    std::shared_ptr<MainWindowContext> nmwctx)
{
  return {std::make_shared<event_handlers::TrainingEventsHandler>(nmwctx),
          std::make_shared<event_handlers::FolderEventsHandler>(nmwctx),
          std::make_shared<event_handlers::WindowSystemEventsHandler>(nmwctx)};
}

}  // namespace darknetxxGtkmm4::window
