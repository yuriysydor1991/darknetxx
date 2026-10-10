#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_TRAINGTKMMINITER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_TRAINGTKMMINITER_CLASS_H

#include "src/gui/gtkmm4/GtkmmIniter.h"

namespace darknetxxGtkmm4
{

/**
 * @brief The GTKmm 4 GUI of the window of the YOLO4 folder training.
 */
class TrainGtkmmIniter : public GtkmmIniter
{
 public:
  virtual ~TrainGtkmmIniter() = default;
  TrainGtkmmIniter();

 protected:
  virtual std::shared_ptr<AWindowContext> create_window_context(
      app::AppCtxPtr ctx, darknetxx011::LibraryContextPtr libctx) override;
};

}  // namespace darknetxxGtkmm4

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_TRAINGTKMMINITER_CLASS_H
