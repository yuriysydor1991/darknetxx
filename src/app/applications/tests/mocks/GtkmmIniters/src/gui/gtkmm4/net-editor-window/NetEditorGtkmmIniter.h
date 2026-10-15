#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_NETEDITORGTKMMINITER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_NETEDITORGTKMMINITER_CLASS_H

#include "src/gui/IGUIIniter.h"

namespace darknetxxGtkmm4
{

class NetEditorGtkmmIniter : public gui::IGUIIniter
{
 public:
  bool run(app::AppCtxPtr, darknetxx011::LibraryContextPtr) override
  {
    return true;
  }

  void deinit() override {}
};

}  // namespace darknetxxGtkmm4

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_NETEDITORGTKMMINITER_CLASS_H
