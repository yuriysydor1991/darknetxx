#include <gtest/gtest.h>

#include <string>

#include "ConverterRuns.h"
#include "ITEST_converter_provider.h"
#include "WeightsVariants.h"

using namespace testing;
using namespace converter_test;

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
class ITEST_converter_layers : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace{ITEST_converter_provider::GENERATED_DIR};
};

TEST_P(ITEST_converter_layers, converts_back_and_forth)
{
  check_variant(GetParam(), workspace);
}

INSTANTIATE_TEST_SUITE_P(Generated, ITEST_converter_layers,
                         ValuesIn(variants::layers()), variant_name);

/**
 * @brief The network families from the 2x2 pixels up to the DCI 4K
 * resolution.
 */
class ITEST_converter_resolutions : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace{ITEST_converter_provider::GENERATED_DIR};
};

TEST_P(ITEST_converter_resolutions, converts_back_and_forth)
{
  check_variant(GetParam(), workspace);
}

INSTANTIATE_TEST_SUITE_P(Generated, ITEST_converter_resolutions,
                         ValuesIn(variants::resolutions_sweep()), variant_name);

/**
 * @brief The random networks of the random layers with the random options.
 */
class ITEST_converter_random : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace{ITEST_converter_provider::GENERATED_DIR};
};

TEST_P(ITEST_converter_random, converts_back_and_forth)
{
  check_variant(GetParam(), workspace);
}

INSTANTIATE_TEST_SUITE_P(
    Generated, ITEST_converter_random,
    ValuesIn(variants::random(variants::DEFAULT_RANDOM_SEED,
                              variants::DEFAULT_RANDOM_COUNT)),
    variant_name);

/**
 * @brief The real world sized networks: the weights files of the tens and
 * the hundreds of megabytes.
 */
class ITEST_converter_big : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace{ITEST_converter_provider::GENERATED_DIR};
};

TEST_P(ITEST_converter_big, converts_back_and_forth)
{
  check_variant(GetParam(), workspace);
}

INSTANTIATE_TEST_SUITE_P(Generated, ITEST_converter_big,
                         ValuesIn(variants::big()), variant_name);
