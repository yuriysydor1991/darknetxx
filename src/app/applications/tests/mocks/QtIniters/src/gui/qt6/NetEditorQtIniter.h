#ifndef DARKNETXX_PROJECT_GUI_QT6_NETEDITORQTINITER_CLASS_H
#define DARKNETXX_PROJECT_GUI_QT6_NETEDITORQTINITER_CLASS_H

#include "src/gui/IGUIIniter.h"

namespace darknetxxQt6
{

class NetEditorQtIniter : public gui::IGUIIniter
{
 public:
  bool run(app::AppCtxPtr, darknetxx011::LibraryContextPtr) override
  {
    return true;
  }

  void deinit() override {}
};

}  // namespace darknetxxQt6

#endif  // DARKNETXX_PROJECT_GUI_QT6_NETEDITORQTINITER_CLASS_H
