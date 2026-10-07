#include "TrainingProgress.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

namespace darknetxx011
{

TrainingProgress::TrainingProgress(const int niteration, const int niterations,
                                   const float nloss, const float navg_loss,
                                   const float nrate, const double nhours_left)
    : miteration{niteration},
      miterations{niterations},
      mloss{nloss},
      mavg_loss{navg_loss},
      mrate{nrate},
      mhours_left{nhours_left}
{
}

int TrainingProgress::iteration() const { return miteration; }

double TrainingProgress::done_fraction() const
{
  if (miterations <= 0) {
    return 0.0;
  }

  return std::clamp(static_cast<double>(miteration) / miterations, 0.0, 1.0);
}

std::string TrainingProgress::iterations_text() const
{
  return std::to_string(miteration) + " / " + std::to_string(miterations);
}

std::string TrainingProgress::loss_text() const
{
  std::ostringstream text;

  text << std::fixed << std::setprecision(4) << mloss;

  return text.str();
}

std::string TrainingProgress::details_text() const
{
  std::ostringstream text;

  text << std::fixed << std::setprecision(4) << "avg loss " << mavg_loss
       << ", learning rate " << std::setprecision(6) << mrate << ", "
       << std::setprecision(2) << mhours_left << " hours left";

  return text.str();
}

}  // namespace darknetxx011
