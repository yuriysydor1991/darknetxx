#include <gtest/gtest.h>

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#include "DarknetNetwork.h"
#include "WeightsNetSpec.h"
#include "WeightsWorkspace.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocumentValidator.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsLayout.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h"
#include "src/log/default-logger/DefaultLogger.h"
#include "version.h"

using namespace testing;

using darknet_adaptor::loaders::WeightsArray;
using darknet_adaptor::loaders::WeightsArrayKind;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsDocumentValidator;
using darknet_adaptor::loaders::WeightsHeader;
using darknet_adaptor::loaders::WeightsLayer;
using darknet_adaptor::loaders::WeightsLayout;
using darknet_adaptor::loaders::WeightsReport;
using darknet_adaptor::loaders::WeightsSchema;
using darknet_adaptor::loaders::WeightsSubLayer;

namespace
{

/// @brief Keeps the info messages of the darknet parser out of the output.
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

const std::vector<WeightsArrayKind>& all_arrays()
{
  static const std::vector<WeightsArrayKind> arrays{
      WeightsArrayKind::Biases, WeightsArrayKind::Scales,
      WeightsArrayKind::RollingMean, WeightsArrayKind::RollingVariance,
      WeightsArrayKind::Weights};
  return arrays;
}

const std::vector<WeightsSubLayer>& all_subs()
{
  static const std::vector<WeightsSubLayer> subs{
      WeightsSubLayer::InputLayer,  WeightsSubLayer::SelfLayer,
      WeightsSubLayer::OutputLayer, WeightsSubLayer::InputZLayer,
      WeightsSubLayer::InputRLayer, WeightsSubLayer::InputHLayer,
      WeightsSubLayer::StateZLayer, WeightsSubLayer::StateRLayer,
      WeightsSubLayer::StateHLayer, WeightsSubLayer::Wf,
      WeightsSubLayer::Wi,          WeightsSubLayer::Wg,
      WeightsSubLayer::Wo,          WeightsSubLayer::Uf,
      WeightsSubLayer::Ui,          WeightsSubLayer::Ug,
      WeightsSubLayer::Uo,          WeightsSubLayer::Vf,
      WeightsSubLayer::Vi,          WeightsSubLayer::Vo};
  return subs;
}

WeightsArray weights_array(const WeightsSubLayer sub,
                           const WeightsArrayKind kind, const size_t count)
{
  WeightsArray result;
  result.sub = sub;
  result.kind = kind;
  result.values.assign(count, 0.5f);
  return result;
}

WeightsLayer weights_layer(const int index, const LAYER_TYPE type,
                           const std::vector<WeightsArray>& arrays = {})
{
  WeightsLayer result;
  result.index = index;
  result.type = type;
  result.arrays = arrays;
  return result;
}

::testing::AssertionResult reports(const WeightsReport& report,
                                   const std::string& error)
{
  if (report.ok()) {
    return ::testing::AssertionFailure()
           << "no errors, \"" << error << "\" expected";
  }

  if (report.first_error().find(error) == std::string::npos) {
    return ::testing::AssertionFailure()
           << "the error \"" << report.first_error() << "\" instead of \""
           << error << "\"";
  }

  return ::testing::AssertionSuccess();
}

}  // namespace

TEST(CTEST_WeightsModel, schema_names_read_back)
{
  for (const WeightsArrayKind kind : all_arrays()) {
    WeightsArrayKind parsed = WeightsArrayKind::Biases;

    ASSERT_TRUE(WeightsSchema::parse(WeightsSchema::name(kind), parsed));
    EXPECT_EQ(parsed, kind);
  }

  for (const WeightsSubLayer sub : all_subs()) {
    WeightsSubLayer parsed = WeightsSubLayer::None;

    ASSERT_TRUE(WeightsSchema::parse(WeightsSchema::name(sub), parsed));
    EXPECT_EQ(parsed, sub);
  }

  // every darknet layer type
  for (int value = CONVOLUTIONAL; value <= IMPLICIT; ++value) {
    const auto type = static_cast<LAYER_TYPE>(value);
    LAYER_TYPE parsed = BLANK;

    ASSERT_NE(WeightsSchema::name(type), "unknown") << value;
    ASSERT_TRUE(WeightsSchema::parse(WeightsSchema::name(type), parsed));
    EXPECT_EQ(parsed, type);
  }

  EXPECT_EQ(WeightsSchema::name(CONVOLUTIONAL), "convolutional");
  EXPECT_EQ(WeightsSchema::name(ACTIVE), "activation");
  EXPECT_EQ(WeightsSchema::name(REORG), "reorg3d");
  EXPECT_EQ(WeightsSchema::name(REORG_OLD), "reorg");
  EXPECT_EQ(WeightsSchema::name(WeightsSubLayer::InputZLayer), "input_z_layer");
  EXPECT_EQ(WeightsSchema::name(WeightsArrayKind::RollingMean), "rolling_mean");
  EXPECT_EQ(WeightsSchema::name(WeightsSubLayer::None), "");
}

TEST(CTEST_WeightsModel, schema_rejects_the_unknown_names)
{
  WeightsArrayKind kind = WeightsArrayKind::Biases;
  WeightsSubLayer sub = WeightsSubLayer::Wf;
  LAYER_TYPE type = CONNECTED;

  for (const std::string& name : std::vector<std::string>{
           "", "bias", "Biases", "biases_ema", "weights ", "input"}) {
    EXPECT_FALSE(WeightsSchema::parse(name, kind)) << name;
    EXPECT_FALSE(WeightsSchema::parse(name, sub)) << name;
    EXPECT_FALSE(WeightsSchema::parse(name, type)) << name;
  }

  EXPECT_EQ(kind, WeightsArrayKind::Biases);
  EXPECT_EQ(sub, WeightsSubLayer::Wf);
  EXPECT_EQ(type, CONNECTED);
}

TEST(CTEST_WeightsModel, schema_ranks_follow_the_file_order)
{
  using K = WeightsArrayKind;
  using S = WeightsSubLayer;

  const auto ranks = [](const LAYER_TYPE type, const S sub) {
    std::vector<int> result;
    for (const K kind : all_arrays()) {
      result.push_back(WeightsSchema::rank(type, sub, kind));
    }
    return result;
  };

  // the biases, scales, rolling mean, rolling variance and weights ranks
  EXPECT_EQ(ranks(CONVOLUTIONAL, S::None), (std::vector<int>{0, 1, 2, 3, 4}));
  EXPECT_EQ(ranks(CONNECTED, S::None), (std::vector<int>{0, 2, 3, 4, 1}));
  EXPECT_EQ(ranks(BATCHNORM, S::None), (std::vector<int>{0, 1, 2, 3, -1}));
  EXPECT_EQ(ranks(SHORTCUT, S::None), (std::vector<int>{-1, -1, -1, -1, 0}));
  EXPECT_EQ(ranks(IMPLICIT, S::None), (std::vector<int>{-1, -1, -1, -1, 0}));
  EXPECT_EQ(ranks(LOCAL, S::None), (std::vector<int>{0, -1, -1, -1, 1}));
  EXPECT_EQ(ranks(MAXPOOL, S::None), (std::vector<int>{-1, -1, -1, -1, -1}));

  // the sub-layers one after another
  EXPECT_EQ(ranks(RNN, S::SelfLayer), (std::vector<int>{5, 7, 8, 9, 6}));
  EXPECT_EQ(ranks(CRNN, S::OutputLayer),
            (std::vector<int>{10, 11, 12, 13, 14}));
  EXPECT_EQ(ranks(GRU, S::StateHLayer), (std::vector<int>{25, 27, 28, 29, 26}));
  EXPECT_EQ(ranks(LSTM, S::Uo), (std::vector<int>{35, 37, 38, 39, 36}));
  EXPECT_EQ(ranks(CONV_LSTM, S::Vf), (std::vector<int>{0, 1, 2, 3, 4}));
  EXPECT_EQ(ranks(CONV_LSTM, S::Uo), (std::vector<int>{50, 51, 52, 53, 54}));

  // the arrays of the other layers and sub-layers
  EXPECT_EQ(ranks(RNN, S::None), (std::vector<int>{-1, -1, -1, -1, -1}));
  EXPECT_EQ(ranks(RNN, S::Wf), (std::vector<int>{-1, -1, -1, -1, -1}));
  EXPECT_EQ(ranks(LSTM, S::Vf), (std::vector<int>{-1, -1, -1, -1, -1}));
  EXPECT_EQ(ranks(CONVOLUTIONAL, S::InputLayer),
            (std::vector<int>{-1, -1, -1, -1, -1}));

  for (const LAYER_TYPE type :
       {CONVOLUTIONAL, CONNECTED, BATCHNORM, SHORTCUT, IMPLICIT, LOCAL, RNN,
        GRU, LSTM, CONV_LSTM, CRNN}) {
    EXPECT_TRUE(WeightsSchema::has_weights(type)) << WeightsSchema::name(type);
  }

  for (const LAYER_TYPE type : {MAXPOOL, ROUTE, YOLO, UPSAMPLE, SOFTMAX, EMPTY,
                                SCALE_CHANNELS, SAM, BLANK}) {
    EXPECT_FALSE(WeightsSchema::has_weights(type)) << WeightsSchema::name(type);
  }
}

TEST(CTEST_WeightsModel, header_tells_the_seen_and_the_transposition)
{
  // the (major * 10 + minor) >= 2 versions keep the 64 bits seen, computed
  // with no overflow of the huge majors
  EXPECT_TRUE((WeightsHeader{0, 2, 5, 0U}.seen64()));
  EXPECT_TRUE((WeightsHeader{1, 0, 0, 0U}.seen64()));
  EXPECT_FALSE((WeightsHeader{0, 1, 9999, 0U}.seen64()));
  EXPECT_FALSE((WeightsHeader{0, 0, 0, 0U}.seen64()));
  EXPECT_TRUE(
      (WeightsHeader{std::numeric_limits<int32_t>::max(), 0, 0, 0U}.seen64()));
  EXPECT_FALSE(
      (WeightsHeader{std::numeric_limits<int32_t>::min(), 0, 0, 0U}.seen64()));

  EXPECT_EQ((WeightsHeader{0, 2, 5, 0U}.size()), 20U);
  EXPECT_EQ((WeightsHeader{0, 1, 0, 0U}.size()), 16U);

  EXPECT_FALSE((WeightsHeader{1000, 1000, 0, 0U}.transposed()));
  EXPECT_TRUE((WeightsHeader{1001, 0, 0, 0U}.transposed()));
  EXPECT_TRUE((WeightsHeader{0, 1001, 0, 0U}.transposed()));

  const WeightsHeader current = WeightsHeader::current(77U);

  EXPECT_EQ(current.major, MAJOR_VERSION);
  EXPECT_EQ(current.minor, MINOR_VERSION);
  EXPECT_EQ(current.revision, PATCH_VERSION);
  EXPECT_EQ(current.seen, 77U);
  EXPECT_EQ(current.version(), "0.2.5");
  EXPECT_EQ(current, (WeightsHeader{0, 2, 5, 77U}));
  EXPECT_NE(current, (WeightsHeader{0, 2, 5, 78U}));
}

TEST(CTEST_WeightsModel, document_normalizes_to_the_file_order)
{
  using K = WeightsArrayKind;
  using S = WeightsSubLayer;

  WeightsDocument document;
  document.header = WeightsHeader{0, 1, 0, 5U};
  document.layers = {
      weights_layer(4, RNN,
                    {weights_array(S::OutputLayer, K::Biases, 1U),
                     weights_array(S::InputLayer, K::Scales, 2U),
                     weights_array(S::InputLayer, K::Biases, 3U)}),
      weights_layer(1, MAXPOOL),
      weights_layer(2, CONNECTED,
                    {weights_array(S::None, K::RollingVariance, 4U),
                     weights_array(S::None, K::Weights, 5U),
                     weights_array(S::None, K::Biases, 6U)})};
  document.trailing = {1U, 2U, 3U};

  document.normalize();

  ASSERT_EQ(document.layers.size(), 3U);
  EXPECT_EQ(document.layers[0].index, 1);
  EXPECT_EQ(document.layers[1].index, 2);
  EXPECT_EQ(document.layers[2].index, 4);

  EXPECT_EQ(document.layers[1].arrays[0].kind, K::Biases);
  EXPECT_EQ(document.layers[1].arrays[1].kind, K::Weights);
  EXPECT_EQ(document.layers[1].arrays[2].kind, K::RollingVariance);

  EXPECT_EQ(document.layers[2].arrays[0].sub, S::InputLayer);
  EXPECT_EQ(document.layers[2].arrays[0].kind, K::Biases);
  EXPECT_EQ(document.layers[2].arrays[1].kind, K::Scales);
  EXPECT_EQ(document.layers[2].arrays[2].sub, S::OutputLayer);

  EXPECT_EQ(document.floats(), 21U);
  EXPECT_EQ(document.file_size(), 16U + 21U * 4U + 3U);
  EXPECT_EQ(document.find(2), &document.layers[1]);
  EXPECT_EQ(document.find(3), nullptr);
  EXPECT_NE(document.layers[2].find(S::InputLayer, K::Scales), nullptr);
  EXPECT_EQ(document.layers[2].find(S::SelfLayer, K::Scales), nullptr);

  EXPECT_TRUE(WeightsDocumentValidator::validate(document).ok());
}

TEST(CTEST_WeightsModel, validator_rejects_the_documents_of_the_errors)
{
  using K = WeightsArrayKind;
  using S = WeightsSubLayer;

  struct Case
  {
    WeightsDocument document;
    std::string error;
  };

  std::vector<Case> cases;

  cases.push_back(
      {WeightsDocument{WeightsHeader{0, 1, 0, 0x100000000ULL}, {}, {}, {}, {}},
       "keeps the 32 bits seen"});
  cases.push_back(
      {WeightsDocument{
           WeightsHeader{}, {weights_layer(-1, MAXPOOL)}, {}, {}, {}},
       "negative"});
  cases.push_back(
      {WeightsDocument{WeightsHeader{},
                       {weights_layer(1, MAXPOOL), weights_layer(1, ROUTE)},
                       {},
                       {},
                       {}},
       "the layer 1 is given twice"});
  cases.push_back(
      {WeightsDocument{WeightsHeader{},
                       {weights_layer(2, MAXPOOL), weights_layer(1, ROUTE)},
                       {},
                       {},
                       {}},
       "out of the order"});
  cases.push_back(
      {WeightsDocument{
           WeightsHeader{},
           {weights_layer(0, MAXPOOL,
                          {weights_array(S::None, K::Weights, 1U)})},
           {},
           {},
           {}},
       "keeps no weights"});
  cases.push_back(
      {WeightsDocument{WeightsHeader{},
                       {weights_layer(0, SHORTCUT,
                                      {weights_array(S::None, K::Biases, 1U)})},
                       {},
                       {},
                       {}},
       "has no biases array"});
  cases.push_back(
      {WeightsDocument{
           WeightsHeader{},
           {weights_layer(0, RNN, {weights_array(S::None, K::Biases, 1U)})},
           {},
           {},
           {}},
       "has no biases array"});
  cases.push_back(
      {WeightsDocument{
           WeightsHeader{},
           {weights_layer(0, LSTM, {weights_array(S::Vf, K::Biases, 1U)})},
           {},
           {},
           {}},
       "has no vf biases array"});
  cases.push_back(
      {WeightsDocument{WeightsHeader{},
                       {weights_layer(0, CONVOLUTIONAL,
                                      {weights_array(S::None, K::Biases, 1U),
                                       weights_array(S::None, K::Biases, 1U)})},
                       {},
                       {},
                       {}},
       "given twice"});
  cases.push_back(
      {WeightsDocument{WeightsHeader{},
                       {weights_layer(0, CONVOLUTIONAL,
                                      {weights_array(S::None, K::Weights, 1U),
                                       weights_array(S::None, K::Biases, 1U)})},
                       {},
                       {},
                       {}},
       "out of the order"});

  for (const Case& c : cases) {
    SCOPED_TRACE(c.error);

    EXPECT_TRUE(
        reports(WeightsDocumentValidator::validate(c.document), c.error));
  }
}

TEST(CTEST_WeightsModel, validator_compares_the_document_with_the_network)
{
  using K = WeightsArrayKind;
  using S = WeightsSubLayer;

  weights_factory::WeightsWorkspace workspace{CTEST_WeightsModel_DATA_DIR};

  weights_factory::NetSpec spec;
  spec.name = "validated";
  spec.width = 6;
  spec.height = 6;
  spec.channels = 2;
  spec.layers = {
      weights_factory::conv(4, 3).with_bn(), weights_factory::maxpool(2, 2),
      weights_factory::conv(3, 1).with_dontload(), weights_factory::rnn(3, 2)};

  weights_factory::DarknetNetwork darknet{workspace.write_cfg(spec),
                                          weights_factory::ParseMode::Train};
  const network& net = darknet.net();

  const auto complete = [&net]() {
    WeightsDocument document;

    for (int i = 0; i < net.n; ++i) {
      document.layers.push_back(weights_layer(i, net.layers[i].type));
    }

    for (const auto& entry : WeightsLayout::of(net)) {
      document.layers[static_cast<size_t>(entry.layer)].arrays.push_back(
          weights_array(entry.sub, entry.kind, entry.count));
    }

    return document;
  };

  // the complete document of the network
  WeightsReport report = WeightsDocumentValidator::validate(complete(), net);

  EXPECT_TRUE(report.ok()) << report.first_error();
  EXPECT_TRUE(report.warnings().empty());

  // the document without the arrays of the dontload layer lacks no arrays
  // the network loads
  WeightsDocument document = complete();
  document.layers[2].arrays.clear();
  report = WeightsDocumentValidator::validate(document, net);

  EXPECT_TRUE(report.ok());
  EXPECT_TRUE(report.warnings().empty());

  // the truncated array and the missing ones
  document = complete();
  document.layers[0].arrays[4].values.resize(5U);
  document.layers[3].arrays.clear();
  report = WeightsDocumentValidator::validate(document, net);

  EXPECT_TRUE(report.ok());
  ASSERT_EQ(report.warnings().size(), 2U);
  EXPECT_NE(report.warnings()[0].find("the layer 0 weights holds 5 of the 72"),
            std::string::npos)
      << report.warnings()[0];
  EXPECT_NE(report.warnings()[1].find("6 arrays of the network aren't in the "
                                      "document (the layer 3 input_layer "
                                      "biases is the first of them)"),
            std::string::npos)
      << report.warnings()[1];

  // the errors of the network
  document = complete();
  document.layers[0].arrays[0].values.resize(5U);
  EXPECT_TRUE(reports(WeightsDocumentValidator::validate(document, net),
                      "the layer 0 biases holds 5 values, the network one "
                      "takes 4"));

  document = complete();
  document.layers[1].type = ROUTE;
  EXPECT_TRUE(
      reports(WeightsDocumentValidator::validate(document, net),
              "the layer 1 (route) is the maxpool layer in the network"));

  document = complete();
  document.layers.push_back(weights_layer(4, MAXPOOL));
  EXPECT_TRUE(reports(WeightsDocumentValidator::validate(document, net),
                      "the layer 4 (maxpool) is beyond the 4 layers"));

  // the rnn sub-layers have no batch normalization
  document = complete();
  document.layers[3].arrays.push_back(
      weights_array(S::OutputLayer, K::Scales, 2U));
  document.normalize();
  EXPECT_TRUE(reports(WeightsDocumentValidator::validate(document, net),
                      "the layer 3 output_layer scales isn't an array of the "
                      "network"));
}
