#include "src/gui/gtkmm4/net-editor-window/NetEditorWindowLoader.h"

namespace darknetxxGtkmm4::window
{

bool NetEditorWindowLoader::load()
{
  return load_window("/ua/org/kytok/darknetxx/gui/NetEditorWindow.ui") &&
         new_button() != nullptr && open_button() != nullptr &&
         save_button() != nullptr && save_as_button() != nullptr &&
         new_image() != nullptr && open_image() != nullptr &&
         save_image() != nullptr && save_as_image() != nullptr &&
         file_label() != nullptr && net_grid() != nullptr &&
         classes_view() != nullptr && layers_box() != nullptr &&
         layer_type_dropdown() != nullptr && add_layer_button() != nullptr &&
         add_layer_image() != nullptr && layer_type_label() != nullptr &&
         status_label() != nullptr;
}

}  // namespace darknetxxGtkmm4::window
