#ifndef DARKNETXX_011_ITRAINER_CLASS_H
#define DARKNETXX_011_ITRAINER_CLASS_H

#include <memory>

#include "ATrainingProject.h"
#include "LibraryAPI.h"

namespace darknetxx011
{

/**
 * @brief Trains the network of a training project (the YOLO4 folder, the
 * dxxwjz2p project file) while the caller goes on. The training listener of
 * it's library context hears the training.
 */
class DARKNETXX_LIB_API ITrainer
{
 public:
  virtual ~ITrainer() = default;
  ITrainer() = default;

  /// @brief Starts the training, resumed from the latest weights of the
  /// project.
  virtual bool train(const ATrainingProject& project) = 0;

  /// @brief Asks the training to save it's weights and stop after the current
  /// step.
  virtual void stop() = 0;

  virtual bool running() const = 0;
};

using ITrainerPtr = std::shared_ptr<ITrainer>;

}  // namespace darknetxx011

#endif  // DARKNETXX_011_ITRAINER_CLASS_H
