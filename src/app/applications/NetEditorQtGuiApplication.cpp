#include "src/app/applications/NetEditorQtGuiApplication.h"

#include "src/gui/qt6/NetEditorQtIniter.h"

namespace app
{

std::shared_ptr<gui::IGUIIniter> NetEditorQtGuiApplication::create_gui_initer()
{
  return std::make_shared<darknetxxQt6::NetEditorQtIniter>();
}

}  // namespace app
