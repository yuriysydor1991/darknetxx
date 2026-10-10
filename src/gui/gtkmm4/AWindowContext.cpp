#include "src/gui/gtkmm4/AWindowContext.h"

namespace darknetxxGtkmm4
{

AWindowContext::AWindowContext(app::AppCtxPtr nactx,
                               darknetxx011::LibraryContextPtr nlibctx)
    : actx{std::move(nactx)}, libctx{std::move(nlibctx)}
{
}

bool AWindowContext::load()
{
  if (!loader().load()) {
    LOGE("Fail to load the window");
    return false;
  }

  handlers = create_handlers();

  for (const auto& handler : handlers) {
    if (!handler->subscribe()) {
      LOGE("Fail to connect an event handler to the window");
      return false;
    }
  }

  return true;
}

Gtk::Window* AWindowContext::window() { return loader().window(); }

void AWindowContext::clear() { handlers.clear(); }

}  // namespace darknetxxGtkmm4
