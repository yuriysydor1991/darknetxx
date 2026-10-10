#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_TRAININGEVENTSHANDLER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_TRAININGEVENTSHANDLER_CLASS_H

#include <glibmm/dispatcher.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "ATrainingProject.h"
#include "ITrainingListener.h"
#include "src/gui/gtkmm4/IWindowEventHandler.h"

namespace darknetxxGtkmm4
{
class TrainWindowContext;
}  // namespace darknetxxGtkmm4

namespace darknetxxGtkmm4::window::event_handlers
{

/**
 * @brief The training of the project the activated project entry names (the
 * YOLO4 folder, the working directory at first, or the dxxwjz2p or dxxwjz3p
 * project file), it's stop and resume button and it's progress, which
 * arrives from the training thread and is shown in the window one.
 */
class TrainingEventsHandler
    : public IWindowEventHandler,
      public darknetxx011::ITrainingListener,
      public std::enable_shared_from_this<TrainingEventsHandler>
{
 public:
  using TrainingProgress = darknetxx011::TrainingProgress;

  virtual ~TrainingEventsHandler() = default;
  explicit TrainingEventsHandler(std::shared_ptr<TrainWindowContext> nmwctx);

  virtual bool subscribe() override;

  virtual void on_iteration(const TrainingProgress& progress) override;
  virtual void on_finished(bool succeeded) override;

 protected:
  /// @brief Trains the project of the entry, resuming from it's weights.
  virtual void train();
  virtual void on_train_button();

  virtual void show_started(const darknetxx011::ATrainingProjectPtr& project);
  virtual void show_progress();
  virtual void show_finished();

 private:
  void set_project_choice(bool sensitive);
  void load_chart();

  /// @brief Shows the chart the training record of the latest weights of
  /// the project redraws, false for none.
  bool show_recorded_chart(const darknetxx011::ATrainingProject& project);
  std::string finished_text();

  std::shared_ptr<TrainWindowContext> mwctx;

  /// @brief The latest progress, which the window has not shown yet.
  std::optional<TrainingProgress> mprogress;
  std::mutex mprogressLock;
  std::atomic_bool mprogressPending{false};
  Glib::Dispatcher mprogressDispatcher;

  std::atomic_bool msucceeded{false};
  Glib::Dispatcher mfinishDispatcher;

  /// @brief The project trained, the chart of the latest training of which
  /// is shown.
  darknetxx011::ATrainingProjectPtr mproject;
};

}  // namespace darknetxxGtkmm4::window::event_handlers

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_TRAININGEVENTSHANDLER_CLASS_H
