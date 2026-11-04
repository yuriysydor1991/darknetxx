#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "CTEST_NetworkWeights_provider.h"
#include "src/NetworksLoaders/DataDriversAliases.h"
#include "src/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/NetworksLoaders/network-weights/NetworkWeights.h"
#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/log/default-logger/DefaultLogger.h"

using namespace testing;

using darknet_adaptor::adaptors::ANetworkAdaptorPtr;
using darknet_adaptor::loaders::NetworkWeights;
using darknet_adaptor::loaders::OrigDefaultLoader;

namespace fs = std::filesystem;

namespace
{

/// @brief The darknet logs the layers of every network it builds, the tests
/// keep the warnings and the errors alone.
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

/// @brief The layouts the readers and the writers ask the networks for.
std::vector<dxxdd::WeightsLayoutOptions> layout_options()
{
  dxxdd::WeightsLayoutOptions saving;

  dxxdd::WeightsLayoutOptions ema;
  ema.ema = true;

  dxxdd::WeightsLayoutOptions loading;
  loading.loading = true;

  dxxdd::WeightsLayoutOptions transposed;
  transposed.loading = true;
  transposed.transposed = true;

  dxxdd::WeightsLayoutOptions cut;
  cut.cutoff = 3;

  return {saving, ema, loading, transposed, cut};
}

/// @brief The entry of the layout of no data, the one the two compare by.
std::string describe(const dxxdd::WeightsLayoutEntry& entry)
{
  std::ostringstream text;

  text << dxxdd::WeightsSchema::describe(entry.layer, entry.sub, entry.kind)
       << " of the " << dxxdd::WeightsSchema::name(entry.type)
       << " layer: " << entry.count << " floats, " << entry.rows << " x "
       << entry.cols << " transposed";

  return text.str();
}

std::vector<std::string> describe(const dxxdd::WeightsLayoutEntries& layout)
{
  std::vector<std::string> entries;

  for (const dxxdd::WeightsLayoutEntry& entry : layout) {
    entries.push_back(describe(entry));
  }

  return entries;
}

/// @brief Every cfg file of the directory and of it's subdirectories.
std::vector<std::string> cfg_files(const std::string& dir)
{
  std::vector<std::string> files;

  for (const auto& entry : fs::recursive_directory_iterator{dir}) {
    if (entry.is_regular_file() && entry.path().extension() == ".cfg") {
      files.push_back(entry.path().string());
    }
  }

  std::sort(files.begin(), files.end());

  return files;
}

/// @brief The cfg files of the networks the project carries.
std::vector<std::string> all_cfg_files()
{
  std::vector<std::string> files =
      cfg_files(CTEST_NetworkWeights_provider::CFG_DIR);
  const std::vector<std::string> data =
      cfg_files(CTEST_NetworkWeights_provider::DATA_DIR);

  files.insert(files.end(), data.begin(), data.end());

  return files;
}

/// @brief The darknet network of the cfg file, nullptr for the cfg the
/// darknet builds no network of (the reason in the message).
ANetworkAdaptorPtr darknet_network(const std::string& cfg, std::string& message)
{
  try {
    return OrigDefaultLoader{}.parse_network_cfg_custom(cfg, 1, 1);
  }
  catch (const std::exception& failure) {
    message = failure.what();
  }

  return nullptr;
}

}  // namespace

TEST(CTEST_NetworkWeights, the_library_layer_types_are_the_darknet_ones)
{
  for (int type = 0; type <= static_cast<int>(IMPLICIT); ++type) {
    const auto darknet = static_cast<LAYER_TYPE>(type);

    EXPECT_EQ(NetworkWeights::type_of(NetworkWeights::type_of(darknet)),
              darknet);
    EXPECT_EQ(static_cast<int>(NetworkWeights::type_of(darknet)), type);
  }

  EXPECT_EQ(NetworkWeights::type_of(dxxdd::LayerType::CONVOLUTIONAL),
            CONVOLUTIONAL);
  EXPECT_EQ(NetworkWeights::type_of(dxxdd::LayerType::IMPLICIT), IMPLICIT);
}

/*
 * The library reads the original weights files by the structure of the cfg it
 * makes itself (the darknetxxConverter of the library knows no darknet
 * network), the darknetxx reads them by the darknet network: the two have to
 * be the layout of the very same arrays, so the files are read and written the
 * very same way. Every cfg file the project carries is the case.
 */
TEST(CTEST_NetworkWeights,
     the_cfg_structure_is_the_layout_of_the_darknet_network)
{
  const std::vector<std::string> files = all_cfg_files();
  std::size_t compared = 0U;
  std::vector<std::string> skipped;

  ASSERT_THAT(files, Not(IsEmpty()));

  for (const std::string& cfg : files) {
    SCOPED_TRACE(cfg);

    std::string message;
    const ANetworkAdaptorPtr net = darknet_network(cfg, message);

    if (net == nullptr) {
      skipped.push_back(cfg + ": " + message);
      continue;
    }

    dxxdd::CfgNetworkStructurePtr structure;

    ASSERT_NO_THROW(structure = dxxdd::CfgNetworkStructure::of_file(cfg));
    ASSERT_NE(structure, nullptr);

    const NetworkWeights view{*net->get()};

    ASSERT_EQ(structure->layers(), view.layers());

    for (int layer = 0; layer < view.layers(); ++layer) {
      EXPECT_EQ(structure->type(layer), view.type(layer)) << "layer " << layer;
    }

    EXPECT_EQ(structure->classes(), view.classes());
    EXPECT_EQ(structure->names(), net->names());

    for (const dxxdd::WeightsLayoutOptions& options : layout_options()) {
      SCOPED_TRACE("ema " + std::to_string(options.ema) + ", loading " +
                   std::to_string(options.loading) + ", transposed " +
                   std::to_string(options.transposed) + ", cutoff " +
                   std::to_string(options.cutoff));

      const dxxdd::WeightsLayoutEntries expected = view.layout(options);
      const dxxdd::WeightsLayoutEntries actual = structure->layout(options);

      EXPECT_THAT(describe(actual), ElementsAreArray(describe(expected)));
      EXPECT_EQ(dxxdd::WeightsLayout::floats(actual),
                dxxdd::WeightsLayout::floats(expected));
    }

    ++compared;
  }

  // the cfg files of the files the darknet needs beside them (the trees, the
  // maps) are the ones it builds no network of from here
  for (const std::string& reason : skipped) {
    std::cout << "[ SKIPPED  ] " << reason << std::endl;
  }

  EXPECT_GT(compared, files.size() / 2U);
}

/*
 * The arrays of the view are the arrays of the darknet network: the readers of
 * the library load the weights files into them and the writers save them.
 */
TEST(CTEST_NetworkWeights, the_view_arrays_are_the_network_ones)
{
  const std::string cfg =
      std::string{CTEST_NetworkWeights_provider::DATA_DIR} + "/1.cfg";
  std::string message;
  const ANetworkAdaptorPtr net = darknet_network(cfg, message);

  ASSERT_NE(net, nullptr) << message;

  network& darknet = *net->get();
  NetworkWeights view{darknet};

  for (const dxxdd::WeightsLayoutEntry& entry :
       view.layout(dxxdd::WeightsLayoutOptions{})) {
    const layer& l = darknet.layers[entry.layer];

    if (entry.kind == dxxdd::WeightsArrayKind::Biases) {
      EXPECT_EQ(entry.data, l.biases);
    } else if (entry.kind == dxxdd::WeightsArrayKind::Weights) {
      EXPECT_EQ(entry.data, l.weights);
    }
  }

  view.loaded_seen(64000U);

  EXPECT_EQ(view.seen(), 64000U);
  EXPECT_EQ(*darknet.seen, 64000U);
  EXPECT_EQ(*darknet.cur_iteration, get_current_batch(darknet));

  // the save keeps the images of the current iteration, the way the original
  // save_weights_upto() did
  const uint64_t saved = view.saved_seen();

  EXPECT_EQ(saved, static_cast<uint64_t>(get_current_iteration(darknet) *
                                         darknet.batch * darknet.subdivisions));
  EXPECT_EQ(*darknet.seen, saved);
}

/*
 * The library writes the files of the network and reads them back into the
 * very same network: the arrays, the header and the seen.
 */
TEST(CTEST_NetworkWeights, every_format_round_trips_the_network_arrays)
{
  const std::string cfg =
      std::string{CTEST_NetworkWeights_provider::DATA_DIR} + "/1.cfg";
  const std::string weights =
      std::string{CTEST_NetworkWeights_provider::DATA_DIR} + "/1.weights";
  const fs::path dir =
      fs::temp_directory_path() / "CTEST_NetworkWeights_round_trip";
  std::string message;

  fs::create_directories(dir);

  const ANetworkAdaptorPtr source = darknet_network(cfg, message);

  ASSERT_NE(source, nullptr) << message;
  ASSERT_TRUE(
      dxxdd::WeightsFileFormats::named("weights")->create_reader()->load(
          NetworkWeights{*source->get()}, weights));

  for (const char* const format : {"weights", "dxxwjz1"}) {
    SCOPED_TRACE(format);

    const dxxdd::IWeightsFileFormatPtr files =
        dxxdd::WeightsFileFormats::named(format);
    const std::string file = (dir / ("1" + files->extension())).string();

    ASSERT_NE(files, nullptr);
    ASSERT_TRUE(
        files->create_writer()->save(NetworkWeights{*source->get()}, file));

    const ANetworkAdaptorPtr loaded = darknet_network(cfg, message);

    ASSERT_NE(loaded, nullptr) << message;
    ASSERT_TRUE(
        files->create_reader()->load(NetworkWeights{*loaded->get()}, file));

    const dxxdd::WeightsLayoutEntries expected =
        NetworkWeights::layout_of(*source->get());
    const dxxdd::WeightsLayoutEntries actual =
        NetworkWeights::layout_of(*loaded->get());

    ASSERT_EQ(actual.size(), expected.size());

    for (std::size_t i = 0U; i < expected.size(); ++i) {
      ASSERT_EQ(actual[i].count, expected[i].count);
      EXPECT_TRUE(std::equal(expected[i].data,
                             expected[i].data + expected[i].count,
                             actual[i].data))
          << describe(expected[i]);
    }

    EXPECT_EQ(*loaded->get()->seen, *source->get()->seen);
  }

  std::error_code error;
  fs::remove_all(dir, error);
}
