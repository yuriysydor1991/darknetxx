#include "src/gui/gtkmm4/main-window/event-handlers/TrainingEventsHandler.h"

#include <gdkmm/texture.h>
#include <giomm/file.h>
#include <glibmm/bytes.h>

#include <vector>

#include "LibraryFacade.h"
#include "src/gui/gtkmm4/MainWindowContext.h"
#include "src/log/log.h"

namespace darknetxxGtkmm4::window::event_handlers
{

TrainingEventsHandler::TrainingEventsHandler(
    std::shared_ptr<MainWindowContext> nmwctx)
    : AbstractWindowEventHandler{nmwctx}
{
}

bool TrainingEventsHandler::subscribe()
{
  auto* loader = mwctx->wloader.get();

  mwctx->libctx->training_listener = shared_from_this();

  loader->folder_entry()->signal_activate().connect(
      sigc::mem_fun(*this, &TrainingEventsHandler::train));
  loader->train_button()->signal_clicked().connect(
      sigc::mem_fun(*this, &TrainingEventsHandler::on_train_button));

  mprogressDispatcher.connect(
      sigc::mem_fun(*this, &TrainingEventsHandler::show_progress));
  mfinishDispatcher.connect(
      sigc::mem_fun(*this, &TrainingEventsHandler::show_finished));

  return true;
}

void TrainingEventsHandler::on_iteration(const TrainingProgress& progress)
{
  {
    std::lock_guard<std::mutex> lock{mprogressLock};
    mprogress = progress;
  }

  // the window shows the latest progress, however many arrive meanwhile
  if (!mprogressPending.exchange(true)) {
    mprogressDispatcher.emit();
  }
}

void TrainingEventsHandler::on_finished(const bool succeeded)
{
  msucceeded.store(succeeded);
  mfinishDispatcher.emit();
}

void TrainingEventsHandler::train()
{
  auto* status = mwctx->wloader->status_label();
  const darknetxx011::Yolo4Folder folder{
      mwctx->wloader->folder_entry()->get_text().raw()};

  if (!folder.valid()) {
    status->set_text("No cfg/*.cfg network file found in the " +
                     folder.path().string() + " folder");
    return;
  }

  if (!mwctx->trainer->train(folder)) {
    status->set_text("Fail to start the training, see the log");
    return;
  }

  show_started(folder);
}

void TrainingEventsHandler::on_train_button()
{
  if (!mwctx->trainer->running()) {
    train();
    return;
  }

  mwctx->trainer->stop();

  mwctx->wloader->train_button()->set_sensitive(false);
  mwctx->wloader->status_label()->set_text(
      "Stopping after the current step, the weights are saved");
}

void TrainingEventsHandler::show_started(
    const darknetxx011::Yolo4Folder& folder)
{
  auto* loader = mwctx->wloader.get();

  {
    std::lock_guard<std::mutex> lock{mprogressLock};
    mprogress.reset();
  }

  mfolder = folder;

  set_folder_choice(false);

  loader->training_box()->set_visible(true);
  loader->train_button()->set_label("_Stop");
  loader->train_button()->set_sensitive(true);
  loader->status_label()->set_text(
      folder.latest_weights().empty()
          ? "Training the " + folder.cfg() + " network from scratch"
          : "Resuming the training from " + folder.latest_weights());

  // the chart of the record at once, the training saves it's own later
  if (!show_recorded_chart(folder)) {
    load_chart();
  }
}

void TrainingEventsHandler::show_progress()
{
  auto* loader = mwctx->wloader.get();
  std::optional<TrainingProgress> progress;

  mprogressPending.store(false);

  {
    std::lock_guard<std::mutex> lock{mprogressLock};
    progress = mprogress;
  }

  if (!progress) {
    return;
  }

  loader->progress_bar()->set_fraction(progress->done_fraction());
  loader->progress_bar()->set_text(progress->iterations_text() + " iterations");
  loader->loss_label()->set_text("Loss " + progress->loss_text());
  loader->details_label()->set_text(progress->details_text());

  load_chart();
}

void TrainingEventsHandler::show_finished()
{
  auto* loader = mwctx->wloader.get();

  set_folder_choice(true);

  loader->train_button()->set_label("_Resume");
  loader->train_button()->set_sensitive(true);
  loader->status_label()->set_text(finished_text());
}

void TrainingEventsHandler::set_folder_choice(const bool sensitive)
{
  mwctx->wloader->folder_entry()->set_sensitive(sensitive);
  mwctx->wloader->folder_button()->set_sensitive(sensitive);
}

void TrainingEventsHandler::load_chart()
{
  if (!mfolder) {
    return;
  }

  try {
    mwctx->wloader->chart_picture()->set_paintable(
        Gdk::Texture::create_from_file(
            Gio::File::create_for_path(mfolder->chart())));
  }
  catch (const Glib::Error& e) {
    // none yet, or the training is writing it
    LOGT("No training chart to show: " << e.what());
  }
}

bool TrainingEventsHandler::show_recorded_chart(
    const darknetxx011::Yolo4Folder& folder)
{
  const std::vector<unsigned char> png =
      darknetxx011::LibraryFacade::training_chart(folder.latest_weights());

  if (png.empty()) {
    return false;
  }

  try {
    mwctx->wloader->chart_picture()->set_paintable(
        Gdk::Texture::create_from_bytes(
            Glib::Bytes::create(png.data(), png.size())));
  }
  catch (const Glib::Error& e) {
    LOGE("Fail to show the training chart: " << e.what());
    return false;
  }

  return true;
}

std::string TrainingEventsHandler::finished_text()
{
  if (!msucceeded.load()) {
    return "The training failed, see the log";
  }

  std::lock_guard<std::mutex> lock{mprogressLock};

  if (mprogress && mprogress->done_fraction() >= 1.0) {
    return "The training is done, the final weights are in the backup folder";
  }

  return "The training is stopped, resume it from the saved weights";
}

}  // namespace darknetxxGtkmm4::window::event_handlers
