#include "src/app/RecognitionGuiApplication.h"

#include "src/gui/gtkmm4/recognition-window/RecognitionGtkmmIniter.h"

namespace app
{

std::shared_ptr<gui::IGUIIniter> RecognitionGuiApplication::create_gui_initer()
{
  return std::make_shared<darknetxxGtkmm4::RecognitionGtkmmIniter>();
}

}  // namespace app
