#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/TrainingRecord.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace darknet_adaptor::loaders
{

double TrainingRecord::seconds() const { return mseconds; }

void TrainingRecord::seconds(const double nseconds) { mseconds = nseconds; }

void TrainingRecord::record(const int iteration, const float loss,
                            const float avg_loss)
{
  miterations.push_back(iteration);
  mlosses.push_back(loss);
  mavg_losses.push_back(avg_loss);
}

bool TrainingRecord::assign(std::vector<int> niterations,
                            std::vector<float> nlosses,
                            std::vector<float> navg_losses)
{
  if (niterations.size() != nlosses.size() ||
      niterations.size() != navg_losses.size()) {
    return false;
  }

  miterations = std::move(niterations);
  mlosses = std::move(nlosses);
  mavg_losses = std::move(navg_losses);

  return true;
}

const std::vector<int>& TrainingRecord::iterations() const
{
  return miterations;
}

const std::vector<float>& TrainingRecord::losses() const { return mlosses; }

const std::vector<float>& TrainingRecord::avg_losses() const
{
  return mavg_losses;
}

std::size_t TrainingRecord::size() const { return miterations.size(); }

void TrainingRecord::record_map(const int iteration, const float map)
{
  mmap_iterations.push_back(iteration);
  mmaps.push_back(map);
}

bool TrainingRecord::assign_maps(std::vector<int> niterations,
                                 std::vector<float> nmaps)
{
  if (niterations.size() != nmaps.size()) {
    return false;
  }

  mmap_iterations = std::move(niterations);
  mmaps = std::move(nmaps);

  return true;
}

const std::vector<int>& TrainingRecord::map_iterations() const
{
  return mmap_iterations;
}

const std::vector<float>& TrainingRecord::maps() const { return mmaps; }

std::size_t TrainingRecord::map_size() const { return mmap_iterations.size(); }

void TrainingRecord::truncate_after(const int iteration)
{
  while (!miterations.empty() && miterations.back() > iteration) {
    miterations.pop_back();
    mlosses.pop_back();
    mavg_losses.pop_back();
  }

  while (!mmap_iterations.empty() && mmap_iterations.back() > iteration) {
    mmap_iterations.pop_back();
    mmaps.pop_back();
  }
}

void TrainingRecord::clear()
{
  mseconds = 0.0;
  miterations.clear();
  mlosses.clear();
  mavg_losses.clear();
  mmap_iterations.clear();
  mmaps.clear();
}

}  // namespace darknet_adaptor::loaders
