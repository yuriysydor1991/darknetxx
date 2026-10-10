#include "src/app/TrainGuiApplication.h"

#include "src/gui/gtkmm4/train-window/TrainGtkmmIniter.h"

namespace app
{

std::shared_ptr<gui::IGUIIniter> TrainGuiApplication::create_gui_initer()
{
  return std::make_shared<darknetxxGtkmm4::TrainGtkmmIniter>();
}

}  // namespace app
