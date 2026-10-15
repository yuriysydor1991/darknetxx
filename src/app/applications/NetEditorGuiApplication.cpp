#include "src/app/applications/NetEditorGuiApplication.h"

#include "src/gui/gtkmm4/net-editor-window/NetEditorGtkmmIniter.h"

namespace app
{

std::shared_ptr<gui::IGUIIniter> NetEditorGuiApplication::create_gui_initer()
{
  return std::make_shared<darknetxxGtkmm4::NetEditorGtkmmIniter>();
}

}  // namespace app
