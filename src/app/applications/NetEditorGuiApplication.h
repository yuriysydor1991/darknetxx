#ifndef DARKNETXX_PROJECT_NETEDITORGUIAPPLICATION_CLASS_H
#define DARKNETXX_PROJECT_NETEDITORGUIAPPLICATION_CLASS_H

#include "src/app/applications/GuiApplication.h"

namespace app
{

/**
 * @brief The application of the GTKmm 4 network editor binary: the window,
 * which creates the network of the dxxwjz3p project or edits the one of the
 * project.
 */
class NetEditorGuiApplication : public GuiApplication
{
 protected:
  virtual std::shared_ptr<gui::IGUIIniter> create_gui_initer() override;
};

}  // namespace app

#endif  // DARKNETXX_PROJECT_NETEDITORGUIAPPLICATION_CLASS_H
