#ifndef DARKNETXX_PROJECT_TRAINGUIAPPLICATION_CLASS_H
#define DARKNETXX_PROJECT_TRAINGUIAPPLICATION_CLASS_H

#include "src/app/applications/GuiApplication.h"

namespace app
{

/**
 * @brief The application of the GUI training binary: the window of the
 * training of the chosen YOLO4 folder.
 */
class TrainGuiApplication : public GuiApplication
{
 protected:
  virtual std::shared_ptr<gui::IGUIIniter> create_gui_initer() override;
};

}  // namespace app

#endif  // DARKNETXX_PROJECT_TRAINGUIAPPLICATION_CLASS_H
