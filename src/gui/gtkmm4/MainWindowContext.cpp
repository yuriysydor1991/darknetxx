#include "src/gui/gtkmm4/MainWindowContext.h"

#include "LibraryFacade.h"

namespace darknetxxGtkmm4
{

std::shared_ptr<MainWindowContext> MainWindowContext::build_context(
    app::AppCtxPtr nactx, darknetxx011::LibraryContextPtr nlibctx)
{
  auto mwctx = std::make_shared<MainWindowContext>();

  mwctx->actx = nactx;
  mwctx->libctx = nlibctx;
  mwctx->wloader = std::make_shared<window::WindowLoader>();
  mwctx->weHandler = std::make_shared<window::WindowEventsHandler>();
  mwctx->trainer = darknetxx011::LibraryFacade::create_trainer(nlibctx);

  return mwctx;
}

void MainWindowContext::clear()
{
  // the training reports to the handlers till it's thread ends
  trainer.reset();

  if (libctx != nullptr) {
    libctx->training_listener.reset();
  }

  weHandler.reset();
  wloader.reset();
  libctx.reset();
  actx.reset();
}

}  // namespace darknetxxGtkmm4
