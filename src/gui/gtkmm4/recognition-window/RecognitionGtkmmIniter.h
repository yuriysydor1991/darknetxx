#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_RECOGNITIONGTKMMINITER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_RECOGNITIONGTKMMINITER_CLASS_H

#include "src/gui/gtkmm4/GtkmmIniter.h"

namespace darknetxxGtkmm4
{

/**
 * @brief The GTKmm 4 GUI of the window of the recognition of the images by
 * the network of a weights file.
 */
class RecognitionGtkmmIniter : public GtkmmIniter
{
 public:
  virtual ~RecognitionGtkmmIniter() = default;
  RecognitionGtkmmIniter();

 protected:
  virtual std::shared_ptr<AWindowContext> create_window_context(
      app::AppCtxPtr ctx, darknetxx011::LibraryContextPtr libctx) override;
};

}  // namespace darknetxxGtkmm4

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_RECOGNITIONGTKMMINITER_CLASS_H
