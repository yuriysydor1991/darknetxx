#ifndef DARKNETXX_PROJECT_NETEDITORQTGUIAPPLICATION_CLASS_H
#define DARKNETXX_PROJECT_NETEDITORQTGUIAPPLICATION_CLASS_H

#include "src/app/applications/GuiApplication.h"

namespace app
{

/**
 * @brief The application of the Qt 6 network editor binary: the window,
 * which creates the network of the dxxwjz3p project or edits the one of the
 * project.
 */
class NetEditorQtGuiApplication : public GuiApplication
{
 protected:
  virtual std::shared_ptr<gui::IGUIIniter> create_gui_initer() override;
};

}  // namespace app

#endif  // DARKNETXX_PROJECT_NETEDITORQTGUIAPPLICATION_CLASS_H
