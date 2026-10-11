#include <gtest/gtest.h>

#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "DarknetNetwork.h"
#include "NetworkChecks.h"
#include "WeightsNetSpec.h"
#include "WeightsVariants.h"
#include "WeightsWorkspace.h"
#include "src/NetworksLoaders/weights-model/WeightsLayout.h"
#include "src/NetworksLoaders/weights-model/WeightsSchema.h"

using namespace testing;
using namespace weights_factory;

using darknet_adaptor::loaders::WeightsArrayKind;
using darknet_adaptor::loaders::WeightsLayout;
using darknet_adaptor::loaders::WeightsLayoutEntries;
using darknet_adaptor::loaders::WeightsLayoutEntry;
using darknet_adaptor::loaders::WeightsLayoutOptions;
using darknet_adaptor::loaders::WeightsSchema;
using darknet_adaptor::loaders::WeightsSubLayer;

namespace
{

std::string variant_name(const TestParamInfo<WeightsVariant>& info)
{
  return info.param.name;
}

WeightsSubLayer sub_of(const SubLayer sub)
{
  switch (sub) {
    case SubLayer::None:
      return WeightsSubLayer::None;
    case SubLayer::Input:
      return WeightsSubLayer::InputLayer;
    case SubLayer::Self:
      return WeightsSubLayer::SelfLayer;
    case SubLayer::Output:
      return WeightsSubLayer::OutputLayer;
    case SubLayer::InputZ:
      return WeightsSubLayer::InputZLayer;
    case SubLayer::InputR:
      return WeightsSubLayer::InputRLayer;
    case SubLayer::InputH:
      return WeightsSubLayer::InputHLayer;
    case SubLayer::StateZ:
      return WeightsSubLayer::StateZLayer;
    case SubLayer::StateR:
      return WeightsSubLayer::StateRLayer;
    case SubLayer::StateH:
      return WeightsSubLayer::StateHLayer;
    case SubLayer::Uf:
      return WeightsSubLayer::Uf;
    case SubLayer::Ui:
      return WeightsSubLayer::Ui;
    case SubLayer::Ug:
      return WeightsSubLayer::Ug;
    case SubLayer::Uo:
      return WeightsSubLayer::Uo;
    case SubLayer::Wf:
      return WeightsSubLayer::Wf;
    case SubLayer::Wi:
      return WeightsSubLayer::Wi;
    case SubLayer::Wg:
      return WeightsSubLayer::Wg;
    case SubLayer::Wo:
      return WeightsSubLayer::Wo;
    case SubLayer::Vf:
      return WeightsSubLayer::Vf;
    case SubLayer::Vi:
      return WeightsSubLayer::Vi;
    case SubLayer::Vo:
      return WeightsSubLayer::Vo;
  }

  return WeightsSubLayer::None;
}

/// @brief The place of the array in the file: the EMA arrays are kept in the
/// places of the regular ones.
WeightsArrayKind kind_of(const ArrayKind array)
{
  switch (array) {
    case ArrayKind::Biases:
    case ArrayKind::BiasesEma:
      return WeightsArrayKind::Biases;
    case ArrayKind::Scales:
    case ArrayKind::ScalesEma:
      return WeightsArrayKind::Scales;
    case ArrayKind::RollingMean:
      return WeightsArrayKind::RollingMean;
    case ArrayKind::RollingVariance:
      return WeightsArrayKind::RollingVariance;
    case ArrayKind::Weights:
    case ArrayKind::WeightsEma:
      return WeightsArrayKind::Weights;
  }

  return WeightsArrayKind::Weights;
}

std::string describe(const WeightsLayoutEntry& entry)
{
  std::ostringstream text;
  text << WeightsSchema::describe(entry.layer, entry.sub, entry.kind) << " ("
       << WeightsSchema::name(entry.type) << ") of " << entry.count
       << " floats, " << entry.rows << "x" << entry.cols;
  return text.str();
}

/**
 * @brief Checks the layout of the network is the model one: the arrays in
 * the order of the model, their layer types, their counts, their
 * transpositions and their memory (the arrays the model maps the blocks to).
 */
::testing::AssertionResult layout_matches(DarknetNetwork& darknet,
                                          const NetSpec& spec,
                                          const WeightsLayoutEntries& layout,
                                          const Layout& model)
{
  if (layout.size() != model.size()) {
    return ::testing::AssertionFailure()
           << layout.size() << " arrays instead of the " << model.size()
           << " of the model";
  }

  for (size_t index = 0U; index < layout.size(); ++index) {
    const WeightsLayoutEntry& entry = layout[index];
    const Block& block = model[index];

    const bool same =
        entry.layer == block.layer &&
        entry.type ==
            layer_type(spec.layers[static_cast<size_t>(block.layer)].kind) &&
        entry.sub == sub_of(block.sub) && entry.kind == kind_of(block.array) &&
        entry.count == block.count &&
        static_cast<uint64_t>(entry.rows) == block.rows &&
        static_cast<uint64_t>(entry.cols) == block.cols &&
        entry.data == darknet.array(block);

    if (!same) {
      return ::testing::AssertionFailure()
             << "the array " << index << " is " << describe(entry)
             << " instead of " << weights_factory::describe(block);
    }
  }

  return ::testing::AssertionSuccess();
}

/**
 * @brief The layouts of every variant: saved, loaded (the transposing one
 * included), the EMA one of the training networks, up to the cutoffs.
 */
void check_layouts(const WeightsVariant& variant)
{
  WeightsWorkspace workspace{CTEST_WeightsModel_DATA_DIR};

  const NetSpec& spec = variant.spec;
  const int n = static_cast<int>(spec.layers.size());

  DarknetNetwork darknet{workspace.write_cfg(spec), variant.mode};

  ASSERT_TRUE(matches_spec(darknet.net(), spec, variant.mode));

  for (const int cutoff :
       std::set<int>{std::numeric_limits<int>::min(), -1, 0, 1, n / 2, n - 1, n,
                     std::numeric_limits<int>::max()}) {
    for (const bool ema : {false, true}) {
      for (const bool loading : {false, true}) {
        for (const bool transposed : {false, true}) {
          // the EMA arrays are saved alone, the inference networks have
          // none of them
          if ((ema && (loading || transposed || !variant.train())) ||
              (transposed && !loading)) {
            continue;
          }

          SCOPED_TRACE("the cutoff " + std::to_string(cutoff) + ", ema " +
                       std::to_string(ema) + ", loading " +
                       std::to_string(loading) + ", transposed " +
                       std::to_string(transposed));

          WeightsLayoutOptions options;
          options.cutoff = cutoff;
          options.ema = ema;
          options.loading = loading;
          options.transposed = transposed;

          LayoutOptions model;
          model.cutoff = cutoff;
          model.ema = ema;
          model.loading = loading;
          model.transposed = transposed;

          const WeightsLayoutEntries layout =
              WeightsLayout::of(darknet.net(), options);

          ASSERT_TRUE(
              layout_matches(darknet, spec, layout, spec.layout(model)));
          EXPECT_EQ(WeightsLayout::floats(layout),
                    count_floats(spec.layout(model)));
        }
      }
    }
  }
}

}  // namespace

/**
 * @brief The layout of the network of every generated variant is the one the
 * test models independently of the darknetxx code.
 */
class CTEST_WeightsModel_layers : public TestWithParam<WeightsVariant>
{
};

TEST_P(CTEST_WeightsModel_layers, layouts_match_the_model)
{
  check_layouts(GetParam());
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_WeightsModel_layers,
                         ValuesIn(variants::layers()), variant_name);

class CTEST_WeightsModel_resolutions : public TestWithParam<WeightsVariant>
{
};

TEST_P(CTEST_WeightsModel_resolutions, layouts_match_the_model)
{
  check_layouts(GetParam());
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_WeightsModel_resolutions,
                         ValuesIn(variants::resolutions_sweep()), variant_name);

class CTEST_WeightsModel_random : public TestWithParam<WeightsVariant>
{
};

TEST_P(CTEST_WeightsModel_random, layouts_match_the_model)
{
  check_layouts(GetParam());
}

INSTANTIATE_TEST_SUITE_P(
    Generated, CTEST_WeightsModel_random,
    ValuesIn(variants::random(variants::DEFAULT_RANDOM_SEED,
                              variants::DEFAULT_RANDOM_COUNT)),
    variant_name);

class CTEST_WeightsModel_big : public TestWithParam<WeightsVariant>
{
};

TEST_P(CTEST_WeightsModel_big, layouts_match_the_model)
{
  check_layouts(GetParam());
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_WeightsModel_big,
                         ValuesIn(variants::big()), variant_name);
