#include "src/darknet-adaptor/adaptors/TrainingChart/TrainingChart.h"

#include <exception>
#include <opencv2/imgcodecs.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "image_opencv.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/CfgSection.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/NetworkSource.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/log/log.h"

namespace darknet_adaptor::adaptors
{

bool TrainingChart::render(const loaders::WeightsDocument& rest,
                           std::vector<unsigned char>& png)
{
  // the grid and the scale of the chart of the training
  static constexpr const int SIZE = 1000;
  static constexpr const int LINES = 100;
  int max_batches = 0;
  float max_loss = 20.f;

  try {
    std::istringstream cfg{rest.network.cfg()};
    loaders::CfgSections sections =
        loaders::CfgSection::read_cfg(cfg, "the network cfg of the chart");

    if (!sections.empty()) {
      max_batches = sections.front().find_int_quiet("max_batches", 0);
      max_loss = sections.front().find_float_quiet("max_chart_loss", max_loss);
    }
  }
  catch (const std::exception& e) {
    LOGE("Fail to read the network cfg of the chart: " << e.what());
    return false;
  }

  if (max_batches <= 0) {
    LOGE("The network cfg of the chart has no max_batches");
    return false;
  }

  const loaders::TrainingRecord& training = rest.training;
  const train_chart_record record{training.iterations().data(),
                                  training.avg_losses().data(),
                                  static_cast<int>(training.size()),
                                  training.map_iterations().data(),
                                  training.maps().data(),
                                  static_cast<int>(training.map_size())};
  train_chart_state state{};
  char name[] = "chart";
  mat_cv* chart =
      draw_train_chart(name, max_loss, max_batches, LINES, SIZE, 1, nullptr);
  bool encoded = false;

  draw_train_record(chart, SIZE, max_loss, max_batches, &record, &state);

  try {
    encoded = cv::imencode(".png", *reinterpret_cast<cv::Mat*>(chart), png);
  }
  catch (const cv::Exception& e) {
    LOGE("Fail to encode the training chart: " << e.what());
  }

  release_mat(&chart);

  return encoded;
}

bool TrainingChart::render_file(const std::string& weights,
                                std::vector<unsigned char>& png)
{
  loaders::WeightsDocument rest;
  bool held = false;

  if (weights.empty() || !loaders::NetworkSource::describes(weights)) {
    LOGD("The weights file " << weights << " keeps no training record");
    return false;
  }

  return loaders::WeightsFileFormats::of_file(weights)->create_reader()->peek(
             weights, rest, held) &&
         render(rest, png);
}

}  // namespace darknet_adaptor::adaptors
