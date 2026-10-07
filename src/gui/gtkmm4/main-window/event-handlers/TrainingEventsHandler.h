#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_TRAININGEVENTSHANDLER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_TRAININGEVENTSHANDLER_CLASS_H

#include <glibmm/dispatcher.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include "ITrainingListener.h"
#include "Yolo4Folder.h"
#include "src/gui/gtkmm4/main-window/event-handlers/AbstractWindowEventHandler.h"

namespace darknetxxGtkmm4::window::event_handlers
{

/**
 * @brief The training of the folder the activated folder entry names, it's
 * stop and resume button and it's progress, which arrives from the training
 * thread and is shown in the window one.
 */
class TrainingEventsHandler
    : public AbstractWindowEventHandler,
      public darknetxx011::ITrainingListener,
      public std::enable_shared_from_this<TrainingEventsHandler>
{
 public:
  using TrainingProgress = darknetxx011::TrainingProgress;

  virtual ~TrainingEventsHandler() = default;
  explicit TrainingEventsHandler(std::shared_ptr<MainWindowContext> nmwctx);

  virtual bool subscribe() override;

  virtual void on_iteration(const TrainingProgress& progress) override;
  virtual void on_finished(bool succeeded) override;

 protected:
  /// @brief Trains the folder of the entry, resuming from it's weights.
  virtual void train();
  virtual void on_train_button();

  virtual void show_started(const darknetxx011::Yolo4Folder& folder);
  virtual void show_progress();
  virtual void show_finished();

 private:
  void set_folder_choice(bool sensitive);
  void load_chart();

  /// @brief Shows the chart the training record of the latest weights of
  /// the folder redraws, false for none.
  bool show_recorded_chart(const darknetxx011::Yolo4Folder& folder);
  std::string finished_text();

  /// @brief The latest progress, which the window has not shown yet.
  std::optional<TrainingProgress> mprogress;
  std::mutex mprogressLock;
  std::atomic_bool mprogressPending{false};
  Glib::Dispatcher mprogressDispatcher;

  std::atomic_bool msucceeded{false};
  Glib::Dispatcher mfinishDispatcher;

  /// @brief The folder trained, the chart of the latest training of which
  /// is shown.
  std::optional<darknetxx011::Yolo4Folder> mfolder;
};

}  // namespace darknetxxGtkmm4::window::event_handlers

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_TRAININGEVENTSHANDLER_CLASS_H
