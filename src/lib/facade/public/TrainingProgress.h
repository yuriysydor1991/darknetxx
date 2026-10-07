#ifndef DARKNETXX_011_TRAININGPROGRESS_CLASS_H
#define DARKNETXX_011_TRAININGPROGRESS_CLASS_H

#include <string>

#include "LibraryAPI.h"

namespace darknetxx011
{

/**
 * @brief The state of a training right after one of it's iterations.
 */
class DARKNETXX_LIB_API TrainingProgress
{
 public:
  TrainingProgress(int niteration, int niterations, float nloss,
                   float navg_loss, float nrate, double nhours_left);

  int iteration() const;

  /// @brief The done part of the training iterations, from 0 to 1.
  double done_fraction() const;

  /// @brief The done and all the iterations, e.g. "1200 / 6000".
  std::string iterations_text() const;

  std::string loss_text() const;

  /// @brief The average loss, the learning rate and the hours left.
  std::string details_text() const;

 private:
  int miteration;
  int miterations;
  float mloss;
  float mavg_loss;
  float mrate;
  double mhours_left;
};

}  // namespace darknetxx011

#endif  // DARKNETXX_011_TRAININGPROGRESS_CLASS_H
