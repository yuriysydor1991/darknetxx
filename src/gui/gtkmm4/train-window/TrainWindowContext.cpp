#include "src/gui/gtkmm4/train-window/TrainWindowContext.h"

#include "LibraryFacade.h"
#include "src/gui/gtkmm4/ChooserEventsHandler.h"
#include "src/gui/gtkmm4/StopSignalEventsHandler.h"
#include "src/gui/gtkmm4/train-window/TrainingEventsHandler.h"
#include "src/gui/gtkmm4/train-window/WindowSystemEventsHandler.h"

namespace darknetxxGtkmm4
{

TrainWindowContext::TrainWindowContext(app::AppCtxPtr nactx,
                                       darknetxx011::LibraryContextPtr nlibctx)
    : AWindowContext{std::move(nactx), nlibctx},
      trainer{darknetxx011::LibraryFacade::create_trainer(nlibctx)}
{
}

void TrainWindowContext::clear()
{
  // the training reports to the handlers till it's thread ends
  trainer.reset();

  if (libctx != nullptr) {
    libctx->training_listener.reset();
  }

  AWindowContext::clear();
}

window::AWindowLoader& TrainWindowContext::loader() { return wloader; }

TrainWindowContext::Handlers TrainWindowContext::create_handlers()
{
  using namespace window::event_handlers;

  auto projects = Gtk::FileFilter::create();

  projects->set_name("dxxwjz2p and dxxwjz3p project files");
  projects->add_suffix("dxxwjz2p");
  projects->add_suffix("dxxwjz3p");

  return {
      std::make_shared<TrainingEventsHandler>(shared_from_this()),
      std::make_shared<ChooserEventsHandler>(
          *wloader.window(), *wloader.project_entry(), *wloader.folder_button(),
          "Choose the YOLO4 folder", Gtk::FileChooser::Action::SELECT_FOLDER),
      std::make_shared<ChooserEventsHandler>(
          *wloader.window(), *wloader.project_entry(),
          *wloader.project_button(),
          "Choose the dxxwjz2p or dxxwjz3p project file",
          Gtk::FileChooser::Action::OPEN, projects),
      std::make_shared<WindowSystemEventsHandler>(shared_from_this()),
      std::make_shared<StopSignalEventsHandler>(actx, *wloader.window())};
}

}  // namespace darknetxxGtkmm4
