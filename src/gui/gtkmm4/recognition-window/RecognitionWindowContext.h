#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_RECOGNITIONWINDOWCONTEXT_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_RECOGNITIONWINDOWCONTEXT_CLASS_H

#include <memory>

#include "src/gui/gtkmm4/AWindowContext.h"
#include "src/gui/gtkmm4/recognition-window/RecognitionWindowLoader.h"

namespace darknetxxGtkmm4
{

/**
 * @brief The window of the recognition of the images by the network of a
 * weights file of any format.
 */
class RecognitionWindowContext
    : public AWindowContext,
      public std::enable_shared_from_this<RecognitionWindowContext>
{
 public:
  virtual ~RecognitionWindowContext() = default;
  using AWindowContext::AWindowContext;

  window::RecognitionWindowLoader wloader;

 protected:
  virtual window::AWindowLoader& loader() override;
  virtual Handlers create_handlers() override;
};

}  // namespace darknetxxGtkmm4

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_RECOGNITIONWINDOWCONTEXT_CLASS_H
