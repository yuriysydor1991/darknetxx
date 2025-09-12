#include <gtest/gtest.h>

#include <cmath>
#include <limits>

#include "src/randoms.h"

using namespace testing;

class CTEST_darknet_randoms : public Test
{
 public:
  static constexpr const size_t TEST_REPS = 1000000;

  ~CTEST_darknet_randoms() = default;
  CTEST_darknet_randoms() { srand(2222222); }
};

TEST_F(CTEST_darknet_randoms, nan_to_float_is_not_finite)
{
  EXPECT_FALSE(std::isfinite(std::numeric_limits<float>::quiet_NaN()));
}

TEST_F(CTEST_darknet_randoms, rand_normal_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr = rand_normal();

    EXPECT_TRUE(std::isfinite(cr));
  }
}

TEST_F(CTEST_darknet_randoms, rand_int_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr = rand_int(0, static_cast<int>(TEST_REPS));

    EXPECT_TRUE(std::isfinite(cr));
  }
}

TEST_F(CTEST_darknet_randoms, rand_size_t_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr = rand_size_t();

    EXPECT_TRUE(std::isfinite(cr));
  }
}

TEST_F(CTEST_darknet_randoms, rand_uniform_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr = rand_uniform(0.f, static_cast<float>(TEST_REPS));

    EXPECT_TRUE(std::isfinite(cr));
  }
}

TEST_F(CTEST_darknet_randoms, rand_scale_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr = rand_scale(static_cast<float>(TEST_REPS));

    EXPECT_TRUE(std::isfinite(cr));
  }
}

TEST_F(CTEST_darknet_randoms, random_gen_fast_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr = random_gen_fast();

    EXPECT_TRUE(std::isfinite(cr));
  }
}

TEST_F(CTEST_darknet_randoms, random_float_fast_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr = random_float_fast();

    EXPECT_TRUE(std::isfinite(cr));
  }
}

TEST_F(CTEST_darknet_randoms, random_gen_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr = random_gen();

    EXPECT_TRUE(std::isfinite(cr));
  }
}

TEST_F(CTEST_darknet_randoms, random_float_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr = random_float();

    EXPECT_TRUE(std::isfinite(cr));
  }
}

TEST_F(CTEST_darknet_randoms, rand_uniform_strong_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr = rand_uniform_strong(0.f, static_cast<float>(TEST_REPS));

    EXPECT_TRUE(std::isfinite(cr));
  }
}

// watch the real cases in the code
TEST_F(CTEST_darknet_randoms, rand_precalc_random_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr =
        rand_precalc_random(0.f, static_cast<float>(TEST_REPS), 123);

    EXPECT_TRUE(std::isfinite(cr));
  }
}

TEST_F(CTEST_darknet_randoms, double_rand_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr = double_rand();

    EXPECT_TRUE(std::isfinite(cr));
  }
}

TEST_F(CTEST_darknet_randoms, uint_rand_is_finite)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const auto cr = uint_rand(static_cast<unsigned int>(TEST_REPS));

    EXPECT_TRUE(std::isfinite(cr));
  }
}
