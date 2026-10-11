#include <gtest/gtest.h>

#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <numeric>
#include <vector>

#include "ITrainingListener.h"
#include "LibraryFacade.h"
#include "Yolo4Folder.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pReader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Reader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkFiles.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/TrainingData/TrainingDataRegistry.h"

using darknet_adaptor::adaptors::TrainingDataRegistry;
using darknet_adaptor::loaders::NetworkFiles;
using darknet_adaptor::loaders::WeightsConversion;
using darknet_adaptor::loaders::WeightsConverter;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsFileFormats;
using darknet_adaptor::loaders::dxxwjz2p::v1::Dxxwjz2pReader;
using darknet_adaptor::loaders::dxxwjz3::v3::Dxxwjz3Reader;
using darknetxx011::ITrainer;
using darknetxx011::LibraryContext;
using darknetxx011::LibraryFacade;
using darknetxx011::Yolo4Folder;

namespace fs = std::filesystem;

namespace
{

/// @brief Records the iterations and the end of the training it listens to.
class Listener : public darknetxx011::ITrainingListener
{
 public:
  void on_iteration(const darknetxx011::TrainingProgress& progress) override
  {
    std::lock_guard<std::mutex> lock{mutex};

    if (iterations == 0) {
      first = progress.iteration();
    }

    last = progress.iteration();
    ++iterations;
    changed.notify_all();
  }

  void on_finished(const bool nsucceeded) override
  {
    std::lock_guard<std::mutex> lock{mutex};

    finished = true;
    succeeded = nsucceeded;
    changed.notify_all();
  }

  bool wait_iterations(const int count)
  {
    std::unique_lock<std::mutex> lock{mutex};

    return changed.wait_for(lock, timeout, [&]() {
      return iterations >= count || finished;
    }) && iterations >= count;
  }

  bool wait_finished()
  {
    std::unique_lock<std::mutex> lock{mutex};

    return changed.wait_for(lock, timeout, [&]() { return finished; });
  }

  void reset()
  {
    std::lock_guard<std::mutex> lock{mutex};

    iterations = 0;
    finished = false;
    succeeded = false;
  }

  inline static constexpr const std::chrono::minutes timeout{10};

  std::mutex mutex;
  std::condition_variable changed;
  int iterations{0};
  int first{0};
  int last{0};
  bool finished{false};
  bool succeeded{false};
};

}  // namespace

class CTEST_BackgroundTrainer : public testing::Test
{
 public:
  CTEST_BackgroundTrainer()
      : folder{fs::path{CTEST_BackgroundTrainer_DATA_DIR} /
               testing::UnitTest::GetInstance()->current_test_info()->name()},
        workingDir{fs::current_path()},
        ctx{LibraryFacade::create_library_context()},
        listener{std::make_shared<Listener>()}
  {
    fs::remove_all(folder);
    fs::copy(CTEST_BackgroundTrainer_YOLO4_DIR, folder,
             fs::copy_options::recursive);

    // the way the GUI application trains
    ctx->dont_show = true;
    ctx->dont_save_charts_every_iter = false;
    ctx->training_listener = listener;

    trainer = LibraryFacade::create_trainer(ctx);
  }

  ~CTEST_BackgroundTrainer() override
  {
    trainer.reset();
    ctx->training_listener.reset();
    fs::current_path(workingDir);
    fs::remove_all(folder);
  }

  /// @brief Trains a few iterations of the folder and stops the training.
  void train_a_few_iterations()
  {
    const Yolo4Folder yolo4{folder.string()};

    ASSERT_TRUE(trainer->train(yolo4));
    EXPECT_TRUE(trainer->running());
    ASSERT_TRUE(listener->wait_iterations(3));

    trainer->stop();

    ASSERT_TRUE(listener->wait_finished());
    EXPECT_TRUE(listener->succeeded);
    EXPECT_FALSE(trainer->running());
  }

  /// @brief The rest of the last weights the training saved.
  WeightsDocument saved_rest() const
  {
    WeightsDocument rest;
    bool weights = false;

    EXPECT_TRUE(Dxxwjz3Reader{}.peek(
        (folder / "backup" / "digits_last.dxxwjz3").string(), rest, weights));
    EXPECT_TRUE(weights);

    return rest;
  }

  fs::path folder;
  fs::path workingDir;
  std::shared_ptr<LibraryContext> ctx;
  std::shared_ptr<Listener> listener;
  std::shared_ptr<ITrainer> trainer;
};

TEST_F(CTEST_BackgroundTrainer, stops_saving_the_weights_and_the_chart)
{
  ASSERT_TRUE(Yolo4Folder{folder.string()}.latest_weights().empty());

  train_a_few_iterations();

  EXPECT_EQ(listener->first, 1);
  EXPECT_EQ(Yolo4Folder{folder.string()}.latest_weights(),
            (folder / "backup" / "digits_last.dxxwjz3").string());

  // the single chart of the training start
  const std::string chart = Yolo4Folder{folder.string()}.chart();

  EXPECT_EQ(fs::path{chart}.filename().string().rfind("chart_digits_", 0), 0U)
      << chart;
  EXPECT_TRUE(fs::exists(chart));
  EXPECT_FALSE(fs::exists(folder / "chart.png"));
}

TEST_F(CTEST_BackgroundTrainer, resumes_from_the_saved_weights)
{
  train_a_few_iterations();

  const int stoppedAt = listener->last;
  const WeightsDocument first = saved_rest();

  listener->reset();

  train_a_few_iterations();

  EXPECT_EQ(listener->first, stoppedAt + 1);

  // the weights of the resumed training keep the record of both of them
  const WeightsDocument resumed = saved_rest();
  std::vector<int> iterations(static_cast<size_t>(listener->last));

  std::iota(iterations.begin(), iterations.end(), 1);

  EXPECT_EQ(first.training.size(), static_cast<size_t>(stoppedAt));
  EXPECT_EQ(resumed.training.iterations(), iterations);
  EXPECT_GE(resumed.training.seconds(), first.training.seconds());
  EXPECT_EQ(resumed.network.classes().size(), 10U);
  EXPECT_FALSE(resumed.network.cfg().empty());
}

TEST_F(CTEST_BackgroundTrainer, no_second_training_while_one_runs)
{
  const Yolo4Folder yolo4{folder.string()};

  ASSERT_TRUE(trainer->train(yolo4));
  EXPECT_FALSE(trainer->train(yolo4));

  trainer->stop();

  ASSERT_TRUE(listener->wait_finished());
}

TEST_F(CTEST_BackgroundTrainer, trains_the_dxxwjz2p_project_of_the_folder)
{
  const fs::path project = folder / "project" / "digits.dxxwjz2p";
  WeightsConversion conversion;
  WeightsDocument rest;
  bool weights = false;
  std::string cfg;

  // three iterations of the training, the mAP of the validation images at
  // the end of it
  ASSERT_TRUE(
      NetworkFiles::read_text((folder / "cfg" / "digits.cfg").string(), cfg));
  cfg.replace(cfg.find("max_batches=400"), 15, "max_batches=3");
  cfg.replace(cfg.find("train=data/train.txt"), 20,
              "train=data/train.txt\nvalid=data/valid.txt");
  std::ofstream{folder / "cfg" / "digits.cfg"} << cfg;
  std::ofstream{folder / "data" / "valid.txt"}
      << "data/obj/digit_0_0.png\ndata/obj/digit_1_0.png\n";

  conversion.from =
      WeightsFileFormats::named("yolo4", WeightsFileFormats::convertible());
  conversion.src = folder.string();
  conversion.to = WeightsFileFormats::named("dxxwjz2p");
  conversion.dst = project.string();

  ASSERT_TRUE(WeightsConverter{}.convert(conversion));

  // the project keeps the lists and the labels the training reads
  fs::remove(folder / "data" / "train.txt");
  fs::remove(folder / "data" / "valid.txt");

  for (const auto& entry : fs::directory_iterator{folder / "data" / "obj"}) {
    if (entry.path().extension() == ".txt") {
      fs::remove(entry.path());
    }
  }

  ctx->map = true;

  const darknetxx011::ATrainingProjectPtr opened =
      LibraryFacade::open_training_project(project.string());

  ASSERT_TRUE(opened->valid());
  EXPECT_EQ(opened->path(), folder / "project");
  EXPECT_TRUE(opened->latest_weights().empty());

  ASSERT_TRUE(trainer->train(*opened));
  ASSERT_TRUE(listener->wait_finished());
  EXPECT_TRUE(listener->succeeded);
  EXPECT_EQ(listener->last, 3);

  // the project keeps the latest weights and the record of them, the backup
  // folder beside it the rest
  ASSERT_TRUE(Dxxwjz2pReader{}.peek(project.string(), rest, weights));
  EXPECT_TRUE(weights);
  EXPECT_EQ(rest.annotations.images.size(), 82U);
  EXPECT_EQ(rest.annotations.validation_paths().size(), 2U);
  EXPECT_EQ(rest.training.size(), 3U);
  EXPECT_EQ(rest.training.map_size(), 1U);
  EXPECT_FALSE(fs::is_empty(folder / "project" / "backup"));
  EXPECT_EQ(
      LibraryFacade::open_training_project(project.string())->latest_weights(),
      project.string());
  EXPECT_FALSE(TrainingDataRegistry::has(TrainingDataRegistry::TRAIN_LIST));
}

TEST_F(CTEST_BackgroundTrainer, projects_of_the_paths)
{
  const darknetxx011::ATrainingProjectPtr yolo4 =
      LibraryFacade::open_training_project(folder.string());
  const darknetxx011::ATrainingProjectPtr none =
      LibraryFacade::open_training_project((folder / "none.dxxwjz2p").string());

  EXPECT_TRUE(yolo4->valid());
  EXPECT_EQ(yolo4->cfg(), (folder / "cfg" / "digits.cfg").string());
  EXPECT_FALSE(none->valid());
  EXPECT_EQ(none->path(), folder);
  EXPECT_FALSE(trainer->train(*none));
  EXPECT_FALSE(trainer->running());
}
