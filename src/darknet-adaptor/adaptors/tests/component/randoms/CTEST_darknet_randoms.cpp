#include <gtest/gtest.h>

#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <set>

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

TEST_F(CTEST_darknet_randoms, rand_int_is_in_range)
{
  static constexpr const int min = -5;
  static constexpr const int max = 5;

  std::set<int> seen;

  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const int cr = rand_int(min, max);

    ASSERT_GE(cr, min);
    ASSERT_LE(cr, max);

    seen.insert(cr);
  }

  // every value of the inclusive range comes up
  EXPECT_EQ(seen.size(), static_cast<size_t>(max - min + 1));
}

TEST_F(CTEST_darknet_randoms, rand_int_swaps_the_reversed_range)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const int cr = rand_int(10, 3);

    ASSERT_GE(cr, 3);
    ASSERT_LE(cr, 10);
  }
}

TEST_F(CTEST_darknet_randoms, rand_size_t_covers_every_bit)
{
  size_t bits = 0U;

  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    bits |= rand_size_t();
  }

  EXPECT_EQ(bits, std::numeric_limits<size_t>::max());
}

TEST_F(CTEST_darknet_randoms, rand_uniform_is_in_range)
{
  static constexpr const float min = -2.f;
  static constexpr const float max = 3.f;

  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const float cr = rand_uniform(min, max);

    ASSERT_GE(cr, min);
    ASSERT_LE(cr, max);
  }
}

TEST_F(CTEST_darknet_randoms, rand_scale_is_in_range)
{
  static constexpr const float scale = 1.5f;

  bool below = false;
  bool above = false;

  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const float cr = rand_scale(scale);

    ASSERT_GE(cr, 1.f / scale);
    ASSERT_LE(cr, scale);

    below = below || cr < 1.f;
    above = above || cr > 1.f;
  }

  // both the magnification and the reduction come up
  EXPECT_TRUE(below);
  EXPECT_TRUE(above);
}

TEST_F(CTEST_darknet_randoms, random_gen_fast_covers_every_bit)
{
  unsigned int bits = 0U;
  std::set<unsigned int> seen;

  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const unsigned int cr = random_gen_fast();

    bits |= cr;

    if (seen.size() < 1000U) {
      seen.insert(cr);
    }
  }

  EXPECT_EQ(bits, std::numeric_limits<unsigned int>::max());
  EXPECT_EQ(seen.size(), 1000U);
}

TEST_F(CTEST_darknet_randoms, random_float_fast_is_in_range)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const float cr = random_float_fast();

    ASSERT_GE(cr, 0.f);
    ASSERT_LE(cr, 1.f);
  }
}

TEST_F(CTEST_darknet_randoms, random_gen_is_in_range)
{
  std::set<unsigned int> seen;

  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const unsigned int cr = random_gen();

#if !defined(WIN32) && RAND_MAX >= 65536
    ASSERT_LE(cr, static_cast<unsigned int>(RAND_MAX));
#endif

    if (seen.size() < 1000U) {
      seen.insert(cr);
    }
  }

  // not a constant
  EXPECT_EQ(seen.size(), 1000U);
}

TEST_F(CTEST_darknet_randoms, random_float_is_in_range)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const float cr = random_float();

    ASSERT_GE(cr, 0.f);
    ASSERT_LE(cr, 1.f);
  }
}

TEST_F(CTEST_darknet_randoms, rand_uniform_strong_is_in_range)
{
  static constexpr const float min = 1.f;
  static constexpr const float max = 7.f;

  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const float cr = rand_uniform_strong(min, max);

    ASSERT_GE(cr, min);
    ASSERT_LE(cr, max);
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

TEST_F(CTEST_darknet_randoms, rand_precalc_random_maps_the_random_part)
{
  EXPECT_FLOAT_EQ(rand_precalc_random(2.f, 4.f, 0.f), 2.f);
  EXPECT_FLOAT_EQ(rand_precalc_random(2.f, 4.f, 0.5f), 3.f);
  EXPECT_FLOAT_EQ(rand_precalc_random(4.f, 2.f, 1.f), 4.f);
}

TEST_F(CTEST_darknet_randoms, double_rand_is_in_range)
{
  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const double cr = double_rand();

    ASSERT_GE(cr, 0.0);
    ASSERT_LT(cr, 1.0);
  }
}

TEST_F(CTEST_darknet_randoms, uint_rand_is_in_range)
{
  static constexpr const unsigned int less_than = 10U;

  std::set<unsigned int> seen;

  for (size_t titer = 0U; titer < TEST_REPS; titer++) {
    const unsigned int cr = uint_rand(less_than);

    ASSERT_LT(cr, less_than);

    seen.insert(cr);
  }

  EXPECT_EQ(seen.size(), static_cast<size_t>(less_than));
}
