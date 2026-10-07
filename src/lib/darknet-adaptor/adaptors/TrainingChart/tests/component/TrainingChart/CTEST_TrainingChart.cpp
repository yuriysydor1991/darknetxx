#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <string>
#include <vector>

#include "image_opencv.h"
#include "src/detector/subs/trainer/draw_train_chart_ctx.h"
#include "src/lib/darknet-adaptor/adaptors/TrainingChart/TrainingChart.h"

using namespace testing;
using darknet_adaptor::adaptors::TrainingChart;
using darknet_adaptor::loaders::NetworkDescription;
using darknet_adaptor::loaders::WeightsDocument;

namespace
{

constexpr const int SIZE = 1000;
constexpr const int MAX_BATCHES = 1000;
constexpr const float MAX_LOSS = 20.f;
// the status line of the live chart, which the redrawn ones have none of
constexpr const int STATUS_ROW = 958;

float loss_of(const int iteration)
{
  return 15.f * 100.f / static_cast<float>(100 + iteration);
}

/// @brief The record of the iterations up to the last, of the mAP of every
/// 50th one.
WeightsDocument rest_of(const int last)
{
  WeightsDocument rest;

  rest.network = NetworkDescription{
      "[net]\nmax_batches=" + std::to_string(MAX_BATCHES) + "\n", {}};

  for (int iteration = 1; iteration <= last; ++iteration) {
    rest.training.record(iteration, loss_of(iteration), loss_of(iteration));

    if (iteration % 50 == 0) {
      rest.training.record_map(iteration, static_cast<float>(iteration) / 1e3f);
    }
  }

  return rest;
}

train_chart_record record_of(const WeightsDocument& rest)
{
  const auto& training = rest.training;

  return train_chart_record{training.iterations().data(),
                            training.avg_losses().data(),
                            static_cast<int>(training.size()),
                            training.map_iterations().data(),
                            training.maps().data(),
                            static_cast<int>(training.map_size())};
}

cv::Mat& image_of(mat_cv* img) { return *reinterpret_cast<cv::Mat*>(img); }

/// @brief The live chart of the iterations, with no save of the chart file.
void draw_live(mat_cv* img, const int from, const int to,
               train_chart_state& state)
{
  char name[] = "chart";
  char accuracy[] = "mAP%";

  state.saved_batch = MAX_BATCHES;

  for (int iteration = from; iteration <= to; ++iteration) {
    draw_train_loss_ex(name, img, SIZE, loss_of(iteration), MAX_LOSS, iteration,
                       MAX_BATCHES, static_cast<float>(iteration) / 1e3f,
                       iteration % 50 == 0, accuracy, -1.f, 1, 0, 0.0, 0,
                       &state);
  }
}

mat_cv* new_chart()
{
  char name[] = "chart";

  return draw_train_chart(name, MAX_LOSS, MAX_BATCHES, 100, SIZE, 1, nullptr);
}

AssertionResult same_rows(const cv::Mat& actual, const cv::Mat& expected,
                          const int rows)
{
  if (actual.size() != expected.size() || actual.type() != expected.type()) {
    return AssertionFailure() << "the images are of the different sizes";
  }

  cv::Mat diff;

  cv::absdiff(actual.rowRange(0, rows), expected.rowRange(0, rows), diff);

  const int differing = cv::countNonZero(diff.reshape(1));

  if (differing > 0) {
    return AssertionFailure() << differing << " bytes differ";
  }

  return AssertionSuccess();
}

}  // namespace

class CTEST_TrainingChart : public Test
{
 public:
  void SetUp() override
  {
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);
    std::filesystem::current_path(dir);
  }

  void TearDown() override
  {
    std::filesystem::current_path(workingDir);
    std::filesystem::remove_all(dir);
  }

  /// @brief The chart image of the rest, of it's PNG.
  static cv::Mat rendered(const WeightsDocument& rest)
  {
    std::vector<unsigned char> png;

    EXPECT_TRUE(TrainingChart::render(rest, png));

    return cv::imdecode(png, cv::IMREAD_COLOR);
  }

  std::filesystem::path dir{
      std::filesystem::path{CTEST_TrainingChart_DATA_DIR} /
      UnitTest::GetInstance()->current_test_info()->name()};
  std::filesystem::path workingDir{std::filesystem::current_path()};
};

TEST_F(CTEST_TrainingChart, redrawn_chart_is_the_live_one)
{
  const WeightsDocument first = rest_of(200);
  const train_chart_record record = record_of(first);
  mat_cv* live = new_chart();
  mat_cv* resumed = new_chart();
  train_chart_state liveState{};
  train_chart_state resumedState{};

  // the training of 300 iterations, the one resumed after the 200th of them
  draw_live(live, 1, 300, liveState);
  draw_train_record(resumed, SIZE, MAX_LOSS, MAX_BATCHES, &record,
                    &resumedState);
  draw_live(resumed, 201, 300, resumedState);

  EXPECT_TRUE(same_rows(image_of(resumed), image_of(live), STATUS_ROW));
  EXPECT_TRUE(same_rows(rendered(rest_of(300)), image_of(live), STATUS_ROW));

  release_mat(&live);
  release_mat(&resumed);

  std::vector<unsigned char> png;
  WeightsDocument unbounded = first;

  unbounded.network = NetworkDescription{"[net]\nbatch=1\n", {}};

  EXPECT_FALSE(TrainingChart::render(unbounded, png));
  EXPECT_FALSE(TrainingChart::render_file("net.weights", png));
}

TEST_F(CTEST_TrainingChart, resumed_training_chart_starts_with_the_record)
{
  const WeightsDocument rest = rest_of(200);
  char base[] = "net";
  char missing[] = "missing.png";
  detector_context ctx{};

  ctx.net.max_batches = MAX_BATCHES;
  ctx.net.max_chart_loss = MAX_LOSS;
  ctx.base = base;
  ctx.dont_show = 1;
  ctx.chart_path = missing;
  ctx.current_iteration = 200;
  ctx.resumed = record_of(rest);

  // the record redraws the chart instead of the chart_path image, which is
  // saved at once
  draw_train_chart_ctx(&ctx);

  EXPECT_EQ(ctx.resumed.points, 0);
  release_mat(&ctx.img);

  const std::string chart = ctx.windows_name;

  ASSERT_TRUE(std::filesystem::exists(chart)) << chart;
  EXPECT_TRUE(same_rows(cv::imread(chart), rendered(rest), SIZE));

  // the chart_path file of no chart image is replaced by a new chart
  char text[] = "not_a.png";
  detector_context fresh{};

  std::ofstream{text} << "no image";
  std::filesystem::remove(chart);

  fresh.net = ctx.net;
  fresh.base = base;
  fresh.dont_show = 1;
  fresh.chart_path = text;

  draw_train_chart_ctx(&fresh);
  release_mat(&fresh.img);

  ASSERT_TRUE(std::filesystem::exists(fresh.windows_name));
  EXPECT_TRUE(
      same_rows(cv::imread(fresh.windows_name), rendered(rest_of(0)), SIZE));
}
