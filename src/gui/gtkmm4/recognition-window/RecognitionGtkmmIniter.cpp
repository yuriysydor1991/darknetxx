#include "src/gui/gtkmm4/recognition-window/RecognitionGtkmmIniter.h"

#include "project-global-decls.h"
#include "src/gui/gtkmm4/recognition-window/RecognitionWindowContext.h"

namespace darknetxxGtkmm4
{

RecognitionGtkmmIniter::RecognitionGtkmmIniter()
    : GtkmmIniter{project_decls::PROJECT_RECOGNITION_GUI_NAME,
                  project_decls::PROJECT_RECOGNITION_APPLICATION_ID}
{
}

std::shared_ptr<AWindowContext> RecognitionGtkmmIniter::create_window_context(
    app::AppCtxPtr ctx, darknetxx011::LibraryContextPtr libctx)
{
  return std::make_shared<RecognitionWindowContext>(ctx, libctx);
}

}  // namespace darknetxxGtkmm4
