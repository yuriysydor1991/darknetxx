#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_TRAINWINDOWCONTEXT_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_TRAINWINDOWCONTEXT_CLASS_H

#include <memory>

#include "ITrainer.h"
#include "src/gui/gtkmm4/AWindowContext.h"
#include "src/gui/gtkmm4/train-window/TrainWindowLoader.h"

namespace darknetxxGtkmm4
{

/**
 * @brief The window of the YOLO4 folder training and the trainer of it.
 */
class TrainWindowContext
    : public AWindowContext,
      public std::enable_shared_from_this<TrainWindowContext>
{
 public:
  virtual ~TrainWindowContext() = default;
  TrainWindowContext(app::AppCtxPtr nactx,
                     darknetxx011::LibraryContextPtr nlibctx);

  /// @brief Waits for the stopped training and breaks the references of the
  /// parts to each other.
  virtual void clear() override;

  window::TrainWindowLoader wloader;
  darknetxx011::ITrainerPtr trainer;

 protected:
  virtual window::AWindowLoader& loader() override;
  virtual Handlers create_handlers() override;
};

}  // namespace darknetxxGtkmm4

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_TRAINWINDOWCONTEXT_CLASS_H
