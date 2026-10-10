#include "src/gui/gtkmm4/train-window/TrainGtkmmIniter.h"

#include "project-global-decls.h"
#include "src/gui/gtkmm4/train-window/TrainWindowContext.h"

namespace darknetxxGtkmm4
{

TrainGtkmmIniter::TrainGtkmmIniter()
    : GtkmmIniter{project_decls::PROJECT_TRAIN_GUI_NAME,
                  project_decls::PROJECT_APPLICATION_ID}
{
}

std::shared_ptr<AWindowContext> TrainGtkmmIniter::create_window_context(
    app::AppCtxPtr ctx, darknetxx011::LibraryContextPtr libctx)
{
  return std::make_shared<TrainWindowContext>(ctx, libctx);
}

}  // namespace darknetxxGtkmm4
