#include "src/lib/darknet-adaptor/adaptors/TrainingJournal/TrainingJournal.h"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/log/log.h"

namespace darknet_adaptor::adaptors
{

TrainingJournal::TrainingJournal(loaders::IWeightsFileFormatPtr nformat,
                                 loaders::WeightsDocument nrest,
                                 std::string nproject)
    : mformat{std::move(nformat)},
      mrest{std::move(nrest)},
      mproject{std::move(nproject)},
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

  const bool saved = mformat->create_writer()->save(
      net, stem + mformat->extension(), net.n, ema, mrest);

  return saved && (ema || mproject.empty() || save_project(net));
}

bool TrainingJournal::save_project(network& net) const
{
  const std::string temporary = mproject + ".saving";
  std::error_code error;

  // the project is replaced by the complete file alone
  if (!loaders::WeightsFileFormats::of_file(mproject)->create_writer()->save(
          net, temporary, net.n, false, mrest)) {
    std::filesystem::remove(temporary, error);
    return false;
  }

  std::filesystem::rename(temporary, mproject, error);

  if (error) {
    LOGE("Fail to update the project " << mproject << ": "
                                       << error.message());
    std::filesystem::remove(temporary, error);
    return false;
  }

  return true;
}

const loaders::WeightsDocument& TrainingJournal::rest() const { return mrest; }

}  // namespace darknet_adaptor::adaptors
