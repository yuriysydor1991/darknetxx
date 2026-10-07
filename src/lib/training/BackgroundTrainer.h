#ifndef DARKNETXX_PROJECT_BACKGROUNDTRAINER_CLASS_H
#define DARKNETXX_PROJECT_BACKGROUNDTRAINER_CLASS_H

#include <atomic>
#include <thread>

#include "ILib.h"
#include "ITrainer.h"
#include "LibraryContext.h"

namespace darknetxx_impl
{

/**
 * @brief Runs the training work in it's own thread. The training listener of
 * the context hears the iterations and the end of it.
 */
class BackgroundTrainer : public darknetxx011::ITrainer
{
 public:
  using LibraryContextPtr = darknetxx011::LibraryContextPtr;
  using ILibPtr = darknetxx011::ILibPtr;

  /// @brief Stops the running training and waits for it.
  virtual ~BackgroundTrainer();
  BackgroundTrainer(LibraryContextPtr nctx, ILibPtr ntraining);

  virtual bool train(const darknetxx011::Yolo4Folder& folder) override;
  virtual void stop() override;
  virtual bool running() const override;

 protected:
  /// @brief The thread body: the training and the report of it's end.
  virtual void work();

 private:
  void join();

  LibraryContextPtr ctx;
  ILibPtr training;
  std::thread mworker;
  std::atomic_bool mrunning{false};
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_BACKGROUNDTRAINER_CLASS_H
