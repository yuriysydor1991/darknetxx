#ifndef DARKNET_CXX_PORT_PROJECT_TRAINING_JOURNAL_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_TRAINING_JOURNAL_CLASS_H

#include <chrono>
#include <memory>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsFileFormat.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The journal of the training of a network: the network description,
 * the losses of the iterations and the time of the training (the ones of the
 * resumed training included), which the weights files it saves keep.
 */
class TrainingJournal
{
 public:
  using TrainingJournalPtr = std::shared_ptr<TrainingJournal>;

  virtual ~TrainingJournal() = default;

  /**
   * @brief Starts the clock of the training.
   *
   * @param nformat The format of the weights files the training saves.
   * @param nrest The network description and the training record resumed.
   * @param nproject The project file of the training (the dxxwjz2p or the
   * dxxwjz3p one), which every
   * save of the weights but the EMA ones updates; empty for none.
   */
  TrainingJournal(loaders::IWeightsFileFormatPtr nformat,
                  loaders::WeightsDocument nrest, std::string nproject = {});

  /// @brief Records the losses of the iteration after the recorded ones.
  void record(int iteration, float loss, float avg_loss);

  /// @brief Records the mAP of the iteration after the recorded ones.
  void record_map(int iteration, float map);

  /**
   * @brief Saves the weights of the network (the EMA ones for the ema) into
   * the file of the stem path and the extension of the format with the rest
   * of the training, the training ones into the project file as well.
   */
  bool save(network& net, const std::string& stem, bool ema);

  /// @brief The network description and the training record so far.
  const loaders::WeightsDocument& rest() const;

 private:
  /// @brief Replaces the project file with the one of the weights.
  bool save_project(network& net) const;

  loaders::IWeightsFileFormatPtr mformat;
  loaders::WeightsDocument mrest;
  std::string mproject;
  /// the seconds of the resumed training
  double mresumed;
  std::chrono::steady_clock::time_point mstart;
};

using TrainingJournalPtr = TrainingJournal::TrainingJournalPtr;

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_TRAINING_JOURNAL_CLASS_H
