#ifndef DARKNETXX_011_ITRAINER_CLASS_H
#define DARKNETXX_011_ITRAINER_CLASS_H

#include <memory>

#include "LibraryAPI.h"
#include "Yolo4Folder.h"

namespace darknetxx011
{

/**
 * @brief Trains the network of a YOLO4 folder while the caller goes on. The
 * training listener of it's library context hears the training.
 */
class DARKNETXX_LIB_API ITrainer
{
 public:
  virtual ~ITrainer() = default;
  ITrainer() = default;

  /// @brief Starts the training, resumed from the latest weights of the folder.
  virtual bool train(const Yolo4Folder& folder) = 0;

  /// @brief Asks the training to save it's weights and stop after the current
  /// step.
  virtual void stop() = 0;

  virtual bool running() const = 0;
};

using ITrainerPtr = std::shared_ptr<ITrainer>;

}  // namespace darknetxx011

#endif  // DARKNETXX_011_ITRAINER_CLASS_H
