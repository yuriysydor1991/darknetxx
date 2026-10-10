#include "src/lib/training/BackgroundTrainer.h"

#include <cassert>

#include "src/log/log.h"

namespace darknetxx_impl
{

BackgroundTrainer::~BackgroundTrainer()
{
  stop();
  join();
}

BackgroundTrainer::BackgroundTrainer(LibraryContextPtr nctx, ILibPtr ntraining)
    : ctx{nctx}, training{ntraining}
{
  assert(ctx != nullptr);
  assert(training != nullptr);
}

bool BackgroundTrainer::train(const darknetxx011::ATrainingProject& project)
{
  if (running()) {
    LOGE("The training is running already");
    return false;
  }

  join();

  ctx->withdraw_stop();

  // the stop flag of the context user, the OS stop signal one, for example
  if (ctx->stop_requested()) {
    return false;
  }

  if (!project.valid() || !project.enter(*ctx)) {
    LOGE("No network to train found in the project " << project.path());
    return false;
  }

  mrunning.store(true);

  mworker = std::thread{[this]() { work(); }};

  return true;
}

void BackgroundTrainer::stop() { ctx->request_stop(); }

bool BackgroundTrainer::running() const { return mrunning.load(); }

void BackgroundTrainer::work()
{
  const bool succeeded = training->libcall(ctx);

  mrunning.store(false);

  if (ctx->training_listener != nullptr) {
    ctx->training_listener->on_finished(succeeded);
  }
}

void BackgroundTrainer::join()
{
  if (mworker.joinable()) {
    mworker.join();
  }
}

}  // namespace darknetxx_impl
