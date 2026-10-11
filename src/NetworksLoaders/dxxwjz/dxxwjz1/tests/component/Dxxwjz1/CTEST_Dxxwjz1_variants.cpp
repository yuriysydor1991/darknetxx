#include <gtest/gtest.h>

#include <string>

#include "CTEST_Dxxwjz1_provider.h"
#include "Dxxwjz1Checks.h"
#include "WeightsVariants.h"

using namespace testing;
using namespace dxxwjz1_test;

namespace
{

std::string variant_name(const TestParamInfo<WeightsVariant>& info)
{
  return info.param.name;
}

}  // namespace

/**
 * @brief Every weights layer kind with the combinations of its options and
 * the layers without the weights between the weights layers, in both of the
 * parser modes.
 */
class CTEST_Dxxwjz1_layers : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace{CTEST_Dxxwjz1_provider::GENERATED_DIR};
};

TEST_P(CTEST_Dxxwjz1_layers, saves_the_expected_document)
{
  check_save(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_layers, loads_the_expected_arrays)
{
  check_load(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_layers, round_trips_bit_exact)
{
  check_round_trip(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_layers, loads_the_truncated_document)
{
  check_truncated(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_layers, saves_and_loads_up_to_the_cutoff)
{
  check_cutoff(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_layers, saves_the_ema_arrays)
{
  // the inference networks have no EMA arrays to save
  if (GetParam().train()) {
    check_ema(GetParam(), workspace);
  }
}

TEST_P(CTEST_Dxxwjz1_layers, converts_the_weights_file_back_and_forth)
{
  check_conversion(GetParam(), workspace);
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_Dxxwjz1_layers,
                         ValuesIn(variants::layers()), variant_name);

/**
 * @brief The network families from the 2x2 pixels up to the DCI 4K
 * resolution: the resolution independent convolutional weights, the
 * resolution dependent connected, local and recurrent weights.
 */
class CTEST_Dxxwjz1_resolutions : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace{CTEST_Dxxwjz1_provider::GENERATED_DIR};
};

TEST_P(CTEST_Dxxwjz1_resolutions, saves_the_expected_document)
{
  check_save(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_resolutions, loads_the_expected_arrays)
{
  check_load(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_resolutions, round_trips_bit_exact)
{
  check_round_trip(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_resolutions, loads_the_truncated_document)
{
  check_truncated(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_resolutions, converts_the_weights_file_back_and_forth)
{
  check_conversion(GetParam(), workspace);
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_Dxxwjz1_resolutions,
                         ValuesIn(variants::resolutions_sweep()), variant_name);

/**
 * @brief The random networks of the random layers with the random options,
 * see the variants::random function to generate more of them.
 */
class CTEST_Dxxwjz1_random : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace{CTEST_Dxxwjz1_provider::GENERATED_DIR};
};

TEST_P(CTEST_Dxxwjz1_random, saves_the_expected_document)
{
  check_save(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_random, loads_the_expected_arrays)
{
  check_load(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_random, round_trips_bit_exact)
{
  check_round_trip(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_random, loads_the_truncated_document)
{
  check_truncated(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_random, saves_and_loads_up_to_the_cutoff)
{
  check_cutoff(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_random, saves_the_ema_arrays)
{
  if (GetParam().train()) {
    check_ema(GetParam(), workspace);
  }
}

TEST_P(CTEST_Dxxwjz1_random, converts_the_weights_file_back_and_forth)
{
  check_conversion(GetParam(), workspace);
}

INSTANTIATE_TEST_SUITE_P(
    Generated, CTEST_Dxxwjz1_random,
    ValuesIn(variants::random(variants::DEFAULT_RANDOM_SEED,
                              variants::DEFAULT_RANDOM_COUNT)),
    variant_name);

/**
 * @brief The real world sized networks: the weights files of the tens and
 * the hundreds of megabytes.
 */
class CTEST_Dxxwjz1_big : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace{CTEST_Dxxwjz1_provider::GENERATED_DIR};
};

TEST_P(CTEST_Dxxwjz1_big, saves_the_expected_document)
{
  check_save(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_big, round_trips_bit_exact)
{
  check_round_trip(GetParam(), workspace);
}

TEST_P(CTEST_Dxxwjz1_big, converts_the_weights_file_back_and_forth)
{
  check_conversion(GetParam(), workspace);
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_Dxxwjz1_big,
                         ValuesIn(variants::big()), variant_name);
