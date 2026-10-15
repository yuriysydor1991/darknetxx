#include "src/gui/gtkmm4/net-editor-window/NetEditorGtkmmIniter.h"

#include "project-global-decls.h"
#include "src/gui/gtkmm4/net-editor-window/NetEditorWindowContext.h"

namespace darknetxxGtkmm4
{

NetEditorGtkmmIniter::NetEditorGtkmmIniter()
    : GtkmmIniter{project_decls::PROJECT_NET_EDITOR_GUI_NAME,
                  project_decls::PROJECT_NET_EDITOR_APPLICATION_ID}
{
}

std::shared_ptr<AWindowContext> NetEditorGtkmmIniter::create_window_context(
    app::AppCtxPtr ctx, darknetxx011::LibraryContextPtr libctx)
{
  return std::make_shared<NetEditorWindowContext>(ctx, libctx);
}

}  // namespace darknetxxGtkmm4
