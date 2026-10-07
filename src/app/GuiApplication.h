#ifndef DARKNETXX_PROJECT_GUIAPPLICATION_CLASS_H
#define DARKNETXX_PROJECT_GUIAPPLICATION_CLASS_H

#include <memory>

#include "src/app/Application.h"
#include "src/gui/IGUIIniter.h"

namespace app
{

/**
 * @brief The application of the GUI binary: the window of the src/gui
 * component, which runs the training of the Application.
 */
class GuiApplication : public Application
{
 public:
  virtual ~GuiApplication() = default;
  GuiApplication() = default;

 protected:
  /// @brief Shows the window till the user closes it.
  virtual int perform(AppCtxPtr ctx) override;

  virtual std::shared_ptr<gui::IGUIIniter> create_gui_initer();
};

}  // namespace app

#endif  // DARKNETXX_PROJECT_GUIAPPLICATION_CLASS_H
