#include "src/gui/gtkmm4/net-editor-window/NetEditorWindowContext.h"

#include "src/gui/gtkmm4/StopSignalEventsHandler.h"
#include "src/gui/gtkmm4/net-editor-window/NetworkEventsHandler.h"
#include "src/gui/gtkmm4/net-editor-window/ProjectEventsHandler.h"

namespace darknetxxGtkmm4
{

void NetEditorWindowContext::clear()
{
  network.reset();
  file.reset();
  AWindowContext::clear();
}

void NetEditorWindowContext::tell(const std::string& message,
                                  const bool failure)
{
  auto* status = wloader.status_label();

  status->set_text(message);

  if (failure) {
    status->add_css_class("error");
  } else {
    status->remove_css_class("error");
  }
}

window::AWindowLoader& NetEditorWindowContext::loader() { return wloader; }

NetEditorWindowContext::Handlers NetEditorWindowContext::create_handlers()
{
  using namespace window::event_handlers;

  network = std::make_shared<NetworkEventsHandler>(shared_from_this());
  file = std::make_shared<ProjectEventsHandler>(shared_from_this());

  // the views first, the project shows the network of the command line then
  return {network, file,
          std::make_shared<StopSignalEventsHandler>(actx, *wloader.window())};
}

}  // namespace darknetxxGtkmm4
