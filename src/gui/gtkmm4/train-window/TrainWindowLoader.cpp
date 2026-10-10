#include "src/gui/gtkmm4/train-window/TrainWindowLoader.h"

namespace darknetxxGtkmm4::window
{

bool TrainWindowLoader::load()
{
  return load_window("/ua/org/kytok/darknetxx/gui/TrainWindow.ui") &&
         project_entry() != nullptr && folder_button() != nullptr &&
         project_button() != nullptr && status_label() != nullptr &&
         training_box() != nullptr && progress_bar() != nullptr &&
         loss_label() != nullptr && details_label() != nullptr &&
         train_button() != nullptr && chart_picture() != nullptr;
}

}  // namespace darknetxxGtkmm4::window
