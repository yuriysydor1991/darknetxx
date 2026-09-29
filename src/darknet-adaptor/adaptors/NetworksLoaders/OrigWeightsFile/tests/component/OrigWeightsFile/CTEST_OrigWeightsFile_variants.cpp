#include <gtest/gtest.h>

#include <string>

#include "WeightsChecks.h"
#include "WeightsVariants.h"

using namespace testing;
using namespace weights_test;

namespace
{

std::string variant_name(const TestParamInfo<WeightsVariant>& info)
{
  return info.param.name;
}

}  // namespace

/**
 * @brief Every weights layer kind with the combinations of it's options and
 * the layers without the weights between the weights layers, in both of the
 * parser modes.
 */
class CTEST_OrigWeightsFile_layers : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace;
};

TEST_P(CTEST_OrigWeightsFile_layers, saves_the_expected_layout)
{
  check_save(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_layers, loads_the_expected_layout)
{
  check_load(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_layers, round_trips_bit_exact)
{
  check_round_trip(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_layers, loads_the_truncated_file_prefix)
{
  check_truncated(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_layers, saves_and_loads_up_to_the_cutoff)
{
  check_cutoff(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_layers, saves_the_ema_arrays)
{
  // the inference networks have no EMA arrays to save
  if (GetParam().train()) {
    check_ema(GetParam(), workspace);
  }
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_OrigWeightsFile_layers,
                         ValuesIn(variants::layers()), variant_name);

/**
 * @brief The network families from the 2x2 pixels up to the DCI 4K
 * resolution: the resolution independent convolutional weights, the
 * resolution dependent connected, local and recurrent weights.
 */
class CTEST_OrigWeightsFile_resolutions : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace;
};

TEST_P(CTEST_OrigWeightsFile_resolutions, saves_the_expected_layout)
{
  check_save(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_resolutions, loads_the_expected_layout)
{
  check_load(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_resolutions, round_trips_bit_exact)
{
  check_round_trip(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_resolutions, loads_the_truncated_file_prefix)
{
  check_truncated(GetParam(), workspace);
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_OrigWeightsFile_resolutions,
                         ValuesIn(variants::resolutions_sweep()), variant_name);

/**
 * @brief The random networks of the random layers with the random options,
 * see the variants::random function to generate more of them.
 */
class CTEST_OrigWeightsFile_random : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace;
};

TEST_P(CTEST_OrigWeightsFile_random, saves_the_expected_layout)
{
  check_save(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_random, loads_the_expected_layout)
{
  check_load(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_random, round_trips_bit_exact)
{
  check_round_trip(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_random, loads_the_truncated_file_prefix)
{
  check_truncated(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_random, saves_and_loads_up_to_the_cutoff)
{
  check_cutoff(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_random, saves_the_ema_arrays)
{
  if (GetParam().train()) {
    check_ema(GetParam(), workspace);
  }
}

INSTANTIATE_TEST_SUITE_P(
    Generated, CTEST_OrigWeightsFile_random,
    ValuesIn(variants::random(variants::DEFAULT_RANDOM_SEED,
                              variants::DEFAULT_RANDOM_COUNT)),
    variant_name);

/**
 * @brief The real world sized networks: the weights files of the tens and
 * the hundreds of megabytes.
 */
class CTEST_OrigWeightsFile_big : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace;
};

TEST_P(CTEST_OrigWeightsFile_big, saves_the_expected_layout)
{
  check_save(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_big, loads_the_expected_layout)
{
  check_load(GetParam(), workspace);
}

TEST_P(CTEST_OrigWeightsFile_big, round_trips_bit_exact)
{
  check_round_trip(GetParam(), workspace);
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_OrigWeightsFile_big,
                         ValuesIn(variants::big()), variant_name);
