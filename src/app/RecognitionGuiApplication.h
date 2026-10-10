#ifndef DARKNETXX_PROJECT_RECOGNITIONGUIAPPLICATION_CLASS_H
#define DARKNETXX_PROJECT_RECOGNITIONGUIAPPLICATION_CLASS_H

#include "src/app/GuiApplication.h"

namespace app
{

/**
 * @brief The application of the GUI recognition binary: the window of the
 * recognition of the chosen images by the network of the chosen weights file.
 */
class RecognitionGuiApplication : public GuiApplication
{
 protected:
  virtual std::shared_ptr<gui::IGUIIniter> create_gui_initer() override;
};

}  // namespace app

#endif  // DARKNETXX_PROJECT_RECOGNITIONGUIAPPLICATION_CLASS_H
