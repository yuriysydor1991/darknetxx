#ifndef DARKNET_CXX_PORT_PROJECT_TRAINING_RECORD_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_TRAINING_RECORD_CLASS_H

#include <cstddef>
#include <vector>

namespace darknet_adaptor::loaders
{

/**
 * @brief The record of the training of the weights: the time it took, the
 * loss of every iteration with its average and the mAP of every mAP
 * calculation, in the order they were computed.
 */
class TrainingRecord
{
 public:
  virtual ~TrainingRecord() = default;
  TrainingRecord() = default;

  double seconds() const;
  void seconds(double nseconds);

  void record(int iteration, float loss, float avg_loss);

  /// @brief False for the columns of the different sizes, which change
  /// nothing.
  bool assign(std::vector<int> niterations, std::vector<float> nlosses,
              std::vector<float> navg_losses);

  const std::vector<int>& iterations() const;
  const std::vector<float>& losses() const;
  const std::vector<float>& avg_losses() const;

  std::size_t size() const;

  void record_map(int iteration, float map);

  /// @brief False for the columns of the different sizes, which change
  /// nothing.
  bool assign_maps(std::vector<int> niterations, std::vector<float> nmaps);

  const std::vector<int>& map_iterations() const;
  const std::vector<float>& maps() const;

  std::size_t map_size() const;

  /// @brief Drops the losses and the mAPs of the iterations after the one
  /// given, the ones the weights trained further than they are.
  void truncate_after(int iteration);

  /// @brief Drops the losses, the mAPs and the time.
  void clear();

 private:
  double mseconds{0.0};
  std::vector<int> miterations;
  std::vector<float> mlosses;
  std::vector<float> mavg_losses;
  std::vector<int> mmap_iterations;
  std::vector<float> mmaps;
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_TRAINING_RECORD_CLASS_H
