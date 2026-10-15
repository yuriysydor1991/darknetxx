#ifndef DARKNETXX_PROJECT_GUI_QT6_NETEDITORQTINITER_CLASS_H
#define DARKNETXX_PROJECT_GUI_QT6_NETEDITORQTINITER_CLASS_H

#include "src/gui/IGUIIniter.h"

namespace darknetxxQt6
{

/**
 * @brief The Qt 6 GUI of the window of the network editor, which creates the
 * network of the dxxwjz3p project or edits the one of the project (the one
 * of the --train-cfg command line parameter at first).
 */
class NetEditorQtIniter : public gui::IGUIIniter
{
 public:
  virtual ~NetEditorQtIniter() = default;
  NetEditorQtIniter() = default;

  virtual bool run(app::AppCtxPtr ctx,
                   darknetxx011::LibraryContextPtr libctx) override;
  virtual void deinit() override;

 private:
  /// @brief The count of the command line arguments of the toolkit, which
  /// lives as long as the application does.
  int margc{1};
};

}  // namespace darknetxxQt6

#endif  // DARKNETXX_PROJECT_GUI_QT6_NETEDITORQTINITER_CLASS_H
