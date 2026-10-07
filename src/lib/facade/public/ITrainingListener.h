#ifndef DARKNETXX_011_ITRAININGLISTENER_CLASS_H
#define DARKNETXX_011_ITRAININGLISTENER_CLASS_H

#include <memory>

#include "LibraryAPI.h"
#include "TrainingProgress.h"

namespace darknetxx011
{

/**
 * @brief Listens to a training. Called from the thread the training runs in.
 */
class DARKNETXX_LIB_API ITrainingListener
{
 public:
  virtual ~ITrainingListener() = default;
  ITrainingListener() = default;

  virtual void on_iteration(const TrainingProgress& progress) = 0;

  /// @brief The training ended, done or stopped (succeeded) or failed.
  virtual void on_finished(bool succeeded) = 0;
};

using ITrainingListenerPtr = std::shared_ptr<ITrainingListener>;

}  // namespace darknetxx011

#endif  // DARKNETXX_011_ITRAININGLISTENER_CLASS_H
