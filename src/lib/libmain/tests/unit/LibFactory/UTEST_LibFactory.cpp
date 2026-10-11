#include <gtest/gtest.h>

#include <memory>

#include "src/lib/libmain/LibFactory.h"
#include "src/lib/libmain/LibMain.h"
#include "src/lib/libmain/WeightsConversionLib.h"
#include "src/recognition/NetworkRecognizer.h"
#include "src/training/BackgroundTrainer.h"

using darknetxx011::LibraryContext;
using darknetxx011::LibraryContextPtr;
using namespace darknetxx_impl;

class UTEST_LibFactory : public testing::Test
{
 public:
  LibFactoryPtr factory{LibFactory::create_factory()};
  LibraryContextPtr ctx{factory->create_default_context()};
};

TEST_F(UTEST_LibFactory, the_default_context_is_given)
{
  EXPECT_NE(ctx, nullptr);
}

TEST_F(UTEST_LibFactory, no_context_gets_no_work)
{
  EXPECT_EQ(factory->create_library({}), nullptr);
}

TEST_F(UTEST_LibFactory, the_context_of_no_work_gets_none)
{
  EXPECT_EQ(factory->create_library(ctx), nullptr);
}

TEST_F(UTEST_LibFactory, the_detection_and_the_training_get_the_darknet_work)
{
  ctx->detect_cfg = "detect.cfg";

  EXPECT_NE(std::dynamic_pointer_cast<LibMain>(factory->create_library(ctx)),
            nullptr);

  ctx->detect_cfg.clear();
  ctx->train_cfg = "train.cfg";

  EXPECT_NE(std::dynamic_pointer_cast<LibMain>(factory->create_library(ctx)),
            nullptr);
}

TEST_F(UTEST_LibFactory, the_weights_file_alone_gets_the_darknet_work)
{
  ctx->latest_weights = "net.dxxwjz2";

  EXPECT_NE(std::dynamic_pointer_cast<LibMain>(factory->create_library(ctx)),
            nullptr);
}

TEST_F(UTEST_LibFactory, the_conversion_goes_before_the_darknet_work)
{
  ctx->train_cfg = "train.cfg";
  ctx->convert_from = "weights";

  EXPECT_NE(std::dynamic_pointer_cast<WeightsConversionLib>(
                factory->create_library(ctx)),
            nullptr);
}

TEST_F(UTEST_LibFactory, the_trainer_runs_the_darknet_work_of_the_context)
{
  auto trainer = std::dynamic_pointer_cast<BackgroundTrainer>(
      factory->create_trainer(ctx));

  ASSERT_NE(trainer, nullptr);
  EXPECT_EQ(trainer->ctx, ctx);
  EXPECT_NE(std::dynamic_pointer_cast<LibMain>(trainer->training), nullptr);
}

TEST_F(UTEST_LibFactory, the_recognizer_recognizes_by_the_networks)
{
  EXPECT_NE(std::dynamic_pointer_cast<NetworkRecognizer>(
                factory->create_recognizer()),
            nullptr);
}
