#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_GTKMMINITER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_GTKMMINITER_CLASS_H

#include <gmock/gmock.h>

#include <functional>

#include "src/gui/IGUIIniter.h"

namespace darknetxxGtkmm4
{

class GtkmmIniter : public gui::IGUIIniter
{
 public:
  virtual ~GtkmmIniter() = default;
  GtkmmIniter()
  {
    if (onMockCreate) {
      onMockCreate(*this);
    }
  }

  inline static std::function<void(GtkmmIniter&)> onMockCreate;

  MOCK_METHOD(bool, run,
              (app::AppCtxPtr ctx, darknetxx011::LibraryContextPtr libctx),
              (override));
  MOCK_METHOD(void, deinit, (), (override));
};

}  // namespace darknetxxGtkmm4

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_GTKMMINITER_CLASS_H
