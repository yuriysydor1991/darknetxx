#ifndef DARKNET_CXX_PORT_PROJECT_TRAINING_CHART_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_TRAINING_CHART_CLASS_H

#include <string>
#include <vector>

#include "src/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The PNG image of the chart of a training record, drawn the way the
 * training draws its chart: the average loss of every recorded iteration and
 * the mAP line over the grid of the max_batches and the max_chart_loss of the
 * [net] section of the network cfg. It draws on a chart of its own alone, so
 * it's safe to call while a training draws.
 */
class TrainingChart
{
 public:
  virtual ~TrainingChart() = default;
  TrainingChart() = default;

  /// @brief The chart of the network description and the training record
  /// of the rest, false (logged) for the cfg of no max_batches.
  static bool render(const loaders::WeightsDocument& rest,
                     std::vector<unsigned char>& png);

  /// @brief The render() of the weights file which keeps the record (the
  /// dxxwjz2 and the dxxwjz3 ones), false for the rest of the files.
  static bool render_file(const std::string& weights,
                          std::vector<unsigned char>& png);
};

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_TRAINING_CHART_CLASS_H
