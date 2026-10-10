#include "src/gui/gtkmm4/recognition-window/RecognitionWindowLoader.h"

namespace darknetxxGtkmm4::window
{

bool RecognitionWindowLoader::load()
{
  return load_window("/ua/org/kytok/darknetxx/gui/RecognitionWindow.ui") &&
         weights_entry() != nullptr && weights_button() != nullptr &&
         cfg_entry() != nullptr && cfg_button() != nullptr &&
         names_entry() != nullptr && names_button() != nullptr &&
         image_entry() != nullptr && image_button() != nullptr &&
         thresh_spin() != nullptr && recognize_button() != nullptr &&
         status_label() != nullptr && image_picture() != nullptr &&
         objects_label() != nullptr;
}

}  // namespace darknetxxGtkmm4::window
