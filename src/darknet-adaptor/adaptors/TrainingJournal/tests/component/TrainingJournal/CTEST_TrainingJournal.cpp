#include <gtest/gtest.h>

#include <chrono>
#include <filesystem>
#include <string>
#include <thread>
#include <vector>

#include "DarknetNetwork.h"
#include "WeightsFile.h"
#include "WeightsNetSpec.h"
#include "WeightsWorkspace.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Reader.h"
#include "src/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/darknet-adaptor/adaptors/TrainingJournal/TrainingJournal.h"
#include "src/log/default-logger/DefaultLogger.h"

using namespace testing;
using namespace weights_factory;

using darknet_adaptor::adaptors::TrainingJournal;
using darknet_adaptor::loaders::NetworkDescription;
using darknet_adaptor::loaders::OrigWeightsReader;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsFileFormats;
using darknet_adaptor::loaders::dxxwjz2::v2::Dxxwjz2Reader;

namespace
{

class QuietLogEnvironment : public Environment
{
 public:
  void SetUp() override
  {
    default_logger::DefaultLogger::level(
        default_logger::DefaultLogger::LVL_WARNING);
  }
};

[[maybe_unused]] Environment* const quiet_log_environment =
    AddGlobalTestEnvironment(new QuietLogEnvironment);

NetSpec small_net()
{
  NetSpec spec;
  spec.name = "small";
  spec.width = 8;
  spec.height = 8;
  spec.layers = {conv(4, 3).with_bn(), maxpool(2, 2), connected(3)};
  return spec;
}

/// @brief The rest of the training resumed after the 2 iterations.
WeightsDocument resumed(const std::string& cfg)
{
  WeightsDocument rest;
  rest.network = NetworkDescription{cfg, {"a", "b", "c"}};
  rest.training.record(1, 4.f, 4.f);
  rest.training.record(2, 3.f, 3.9f);
  rest.training.record_map(2, 0.25f);
  rest.training.seconds(100.0);
  return rest;
}

}  // namespace

class CTEST_TrainingJournal : public Test
{
 public:
  CTEST_TrainingJournal()
      : spec{small_net()},
        cfg{workspace.write_cfg(spec)},
        darknet{cfg, ParseMode::Train}
  {
  }

  WeightsWorkspace workspace{CTEST_TrainingJournal_DATA_DIR};
  NetSpec spec;
  std::string cfg;
  DarknetNetwork darknet;
};

TEST_F(CTEST_TrainingJournal, iterations_are_recorded_once)
{
  TrainingJournal journal{WeightsFileFormats::named("dxxwjz2"),
                          resumed(spec.cfg())};

  journal.record(3, 2.f, 3.7f);
  journal.record(3, 2.f, 3.7f);
  journal.record(4, 1.f, 3.4f);
  // the iterations are recorded in their order
  journal.record(2, 9.f, 9.f);
  journal.record_map(2, 0.5f);
  journal.record_map(4, 0.75f);
  journal.record_map(4, 0.8f);

  EXPECT_EQ(journal.rest().training.iterations(),
            (std::vector<int>{1, 2, 3, 4}));
  EXPECT_EQ(journal.rest().training.losses(),
            (std::vector<float>{4.f, 3.f, 2.f, 1.f}));
  EXPECT_EQ(journal.rest().training.map_iterations(), (std::vector<int>{2, 4}));
  EXPECT_EQ(journal.rest().training.maps(), (std::vector<float>{0.25f, 0.75f}));
}

TEST_F(CTEST_TrainingJournal, saved_file_keeps_the_resumed_training_too)
{
  TrainingJournal journal{WeightsFileFormats::named("dxxwjz2"),
                          resumed(spec.cfg())};

  journal.record(3, 2.f, 3.7f);

  std::this_thread::sleep_for(std::chrono::milliseconds{1600});

  const std::string stem = workspace.file("small_last");

  ASSERT_TRUE(darknet.fill(spec.layout(), Pattern{11U}));
  ASSERT_TRUE(journal.save(darknet.net(), stem, false));

  DarknetNetwork loaded{cfg, ParseMode::Train};
  WeightsDocument rest;

  ASSERT_TRUE(Dxxwjz2Reader{}.load(loaded.net(), stem + ".dxxwjz2",
                                   loaded.net().n, rest));

  EXPECT_TRUE(loaded.holds(spec.layout(), Pattern{11U}));
  EXPECT_EQ(rest.network.cfg(), spec.cfg());
  EXPECT_EQ(rest.network.classes(), (std::vector<std::string>{"a", "b", "c"}));
  EXPECT_EQ(rest.training.iterations(), (std::vector<int>{1, 2, 3}));
  EXPECT_EQ(rest.training.map_iterations(), (std::vector<int>{2}));
  // the seconds of the resumed training and of this one
  EXPECT_GE(rest.training.seconds(), 102.0);
  EXPECT_LT(rest.training.seconds(), 160.0);
}

TEST_F(CTEST_TrainingJournal, files_are_of_the_format_given)
{
  TrainingJournal journal{WeightsFileFormats::named("weights"),
                          resumed(spec.cfg())};

  const std::string stem = workspace.file("small_final");

  ASSERT_TRUE(darknet.fill(spec.layout(), Pattern{12U}));
  ASSERT_TRUE(journal.save(darknet.net(), stem, false));

  EXPECT_FALSE(std::filesystem::exists(stem + ".dxxwjz2"));

  DarknetNetwork loaded{cfg, ParseMode::Train};

  ASSERT_TRUE(OrigWeightsReader{}.load(loaded.net(), stem + ".weights"));
  EXPECT_TRUE(loaded.holds(spec.layout(), Pattern{12U}));
}
