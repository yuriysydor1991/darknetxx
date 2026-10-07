#include "src/lib/darknet-adaptor/adaptors/TrainingJournal/TrainingJournal.h"

#include <cassert>
#include <chrono>
#include <string>
#include <utility>
#include <vector>

namespace darknet_adaptor::adaptors
{

TrainingJournal::TrainingJournal(loaders::IWeightsFileFormatPtr nformat,
                                 loaders::WeightsDocument nrest)
    : mformat{std::move(nformat)},
      mrest{std::move(nrest)},
      mresumed{mrest.training.seconds()},
      mstart{std::chrono::steady_clock::now()}
{
  assert(mformat != nullptr);
}

void TrainingJournal::record(const int iteration, const float loss,
                             const float avg_loss)
{
  const std::vector<int>& iterations = mrest.training.iterations();

  // the iteration saved before its report is recorded once, in the order
  if (iterations.empty() || iterations.back() < iteration) {
    mrest.training.record(iteration, loss, avg_loss);
  }
}

void TrainingJournal::record_map(const int iteration, const float map)
{
  const std::vector<int>& iterations = mrest.training.map_iterations();

  if (iterations.empty() || iterations.back() < iteration) {
    mrest.training.record_map(iteration, map);
  }
}

bool TrainingJournal::save(network& net, const std::string& stem,
                           const bool ema)
{
  const std::chrono::duration<double> trained =
      std::chrono::steady_clock::now() - mstart;

  mrest.training.seconds(mresumed + trained.count());

  return mformat->create_writer()->save(net, stem + mformat->extension(), net.n,
                                        ema, mrest);
}

const loaders::WeightsDocument& TrainingJournal::rest() const { return mrest; }

}  // namespace darknet_adaptor::adaptors
