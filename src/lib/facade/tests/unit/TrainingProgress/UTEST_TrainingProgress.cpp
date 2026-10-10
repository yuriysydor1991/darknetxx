#include <gtest/gtest.h>

#include "TrainingProgress.h"

using darknetxx011::TrainingProgress;

TEST(UTEST_TrainingProgress, done_fraction_is_the_done_iterations_part)
{
  EXPECT_DOUBLE_EQ(TrainingProgress(150, 600, 1, 1, 1, 1).done_fraction(),
                   0.25);
}

TEST(UTEST_TrainingProgress, done_fraction_stays_within_0_and_1)
{
  EXPECT_DOUBLE_EQ(TrainingProgress(700, 600, 1, 1, 1, 1).done_fraction(), 1);
  EXPECT_DOUBLE_EQ(TrainingProgress(-1, 600, 1, 1, 1, 1).done_fraction(), 0);
  EXPECT_DOUBLE_EQ(TrainingProgress(10, 0, 1, 1, 1, 1).done_fraction(), 0);
}

TEST(UTEST_TrainingProgress, texts)
{
  const TrainingProgress progress{1200, 6000, 2.5f, 3.25f, 0.001f, 1.5};

  EXPECT_EQ(progress.iterations_text(), "1200 / 6000 iterations (20.0%)");
  EXPECT_EQ(progress.loss_text(), "2.5000");
  EXPECT_EQ(progress.details_text(),
            "avg loss 3.2500, learning rate 0.001000, 1.50 hours left");
}

TEST(UTEST_TrainingProgress, iteration)
{
  EXPECT_EQ(TrainingProgress(150, 600, 1, 1, 1, 1).iteration(), 150);
}
