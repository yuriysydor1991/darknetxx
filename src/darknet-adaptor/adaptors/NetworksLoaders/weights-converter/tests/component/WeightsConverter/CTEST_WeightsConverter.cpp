#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "DarknetNetwork.h"
#include "NetworkChecks.h"
#include "WeightsFile.h"
#include "WeightsNetSpec.h"
#include "WeightsVariants.h"
#include "WeightsWorkspace.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Reader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Writer.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h"
#include "src/log/default-logger/DefaultLogger.h"
#include "src/zlib/ZlibController.h"

using namespace testing;
using namespace weights_factory;

using darknet_adaptor::loaders::OrigWeightsReader;
using darknet_adaptor::loaders::OrigWeightsWriter;
using darknet_adaptor::loaders::WeightsConversion;
using darknet_adaptor::loaders::WeightsConverter;
using darknet_adaptor::loaders::WeightsFormat;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Reader;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Writer;

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

constexpr const uint64_t SEED = 0xC0417E27U;

std::string variant_name(const TestParamInfo<WeightsVariant>& info)
{
  return info.param.name;
}

NetSpec small_net()
{
  NetSpec spec;
  spec.name = "small";
  spec.width = 6;
  spec.height = 6;
  spec.channels = 2;
  spec.layers = {conv(4, 3).with_bn(), maxpool(2, 2), connected(3).with_bn(),
                 rnn(3, 2)};
  return spec;
}

WeightsConversion conversion(const WeightsFormat from, const std::string& src,
                             const WeightsFormat to, const std::string& dst,
                             const std::string& cfg)
{
  WeightsConversion result;
  result.from = from;
  result.src = src;
  result.to = to;
  result.dst = dst;
  result.cfg = cfg;
  return result;
}

/// @brief The files of the directory, the ones the conversion left behind.
std::vector<std::string> files_of(const std::string& dir)
{
  std::vector<std::string> files;

  for (const auto& entry : std::filesystem::directory_iterator{dir}) {
    files.push_back(entry.path().filename().string());
  }

  std::sort(files.begin(), files.end());

  return files;
}

/**
 * @brief Converts the original weights file to the dxxwjz1 one and back, both
 * ways and each format into itself, expecting the same bytes.
 */
void check_back_and_forth(const std::string& weights, const std::string& cfg,
                          const std::string& dir)
{
  WeightsConverter converter;

  const std::string dxxwjz1 = dir + "/converted.dxxwjz1";
  const std::string again = dir + "/again.dxxwjz1";
  const std::string back = dir + "/back.weights";
  const std::string validated = dir + "/validated.weights";
  const std::string copied = dir + "/copied.weights";

  ASSERT_TRUE(converter.convert(conversion(
      WeightsFormat::Original, weights, WeightsFormat::Dxxwjz1, dxxwjz1, cfg)));
  EXPECT_TRUE(zlibi::ZlibController::is_gzip_file(dxxwjz1));

  // the dxxwjz1 file needs no network to convert back
  ASSERT_TRUE(converter.convert(conversion(WeightsFormat::Dxxwjz1, dxxwjz1,
                                           WeightsFormat::Original, back, "")));
  EXPECT_TRUE(files_equal(weights, back));

  // and converts back validated against the network the same way
  ASSERT_TRUE(
      converter.convert(conversion(WeightsFormat::Dxxwjz1, dxxwjz1,
                                   WeightsFormat::Original, validated, cfg)));
  EXPECT_TRUE(files_equal(weights, validated));

  // each format converts into itself
  ASSERT_TRUE(converter.convert(conversion(
      WeightsFormat::Dxxwjz1, dxxwjz1, WeightsFormat::Dxxwjz1, again, cfg)));
  EXPECT_TRUE(files_equal(dxxwjz1, again));

  ASSERT_TRUE(converter.convert(conversion(
      WeightsFormat::Original, weights, WeightsFormat::Original, copied, cfg)));
  EXPECT_TRUE(files_equal(weights, copied));
}

}  // namespace

class CTEST_WeightsConverter : public Test
{
 public:
  WeightsWorkspace workspace{CTEST_WeightsConverter_DATA_DIR};

  /// @brief The weights file of the pattern filled network and its cfg.
  std::string saved(const NetSpec& spec, std::string& cfg)
  {
    cfg = workspace.write_cfg(spec);

    DarknetNetwork darknet{cfg, ParseMode::Train};

    EXPECT_TRUE(darknet.fill(spec.layout(), Pattern{SEED}));

    *darknet.net().cur_iteration = 5;

    const std::string weights = workspace.file(spec.name + ".weights");

    EXPECT_TRUE(OrigWeightsWriter{}.save(darknet.net(), weights));

    return weights;
  }
};

TEST_F(CTEST_WeightsConverter, formats_are_named)
{
  EXPECT_EQ(WeightsConverter::format_names(),
            (std::vector<std::string>{"weights", "dxxwjz1"}));
  EXPECT_EQ(WeightsConverter::format_name(WeightsFormat::Original), "weights");
  EXPECT_EQ(WeightsConverter::format_name(WeightsFormat::Dxxwjz1), "dxxwjz1");

  WeightsFormat format = WeightsFormat::Original;

  ASSERT_TRUE(WeightsConverter::parse_format("dxxwjz1", format));
  EXPECT_EQ(format, WeightsFormat::Dxxwjz1);
  ASSERT_TRUE(WeightsConverter::parse_format("weights", format));
  EXPECT_EQ(format, WeightsFormat::Original);

  for (const std::string& name :
       std::vector<std::string>{"", "Weights", "dxxwjz2", "json", "darknet"}) {
    EXPECT_FALSE(WeightsConverter::parse_format(name, format)) << name;
  }
}

TEST_F(CTEST_WeightsConverter, reference_weights_convert_back_and_forth)
{
  for (const std::string& name : {std::string{"1"}, std::string{"2"}}) {
    SCOPED_TRACE(name);

    check_back_and_forth(
        std::string{CTEST_WeightsConverter_REFERENCE_DIR} + "/" + name +
            ".weights",
        std::string{CTEST_WeightsConverter_REFERENCE_DIR} + "/" + name + ".cfg",
        workspace.dir());
  }
}

TEST_F(CTEST_WeightsConverter, destination_directories_are_made)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const std::string dst = workspace.file("made/deep/dirs/small.dxxwjz1");

  ASSERT_TRUE(WeightsConverter{}.convert(conversion(
      WeightsFormat::Original, weights, WeightsFormat::Dxxwjz1, dst, cfg)));
  EXPECT_TRUE(std::filesystem::is_regular_file(dst));
}

TEST_F(CTEST_WeightsConverter, destinations_are_replaced_and_sources_kept)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const std::string dst = workspace.file("replaced.dxxwjz1");
  const std::string copy = workspace.file("copy.weights");

  {
    std::ofstream old{dst};
    old << "the previous file";
  }

  ASSERT_TRUE(WeightsConverter{}.convert(conversion(
      WeightsFormat::Original, weights, WeightsFormat::Dxxwjz1, dst, cfg)));

  // the conversion of the file into itself
  std::filesystem::copy_file(weights, copy);

  ASSERT_TRUE(WeightsConverter{}.convert(conversion(
      WeightsFormat::Original, copy, WeightsFormat::Original, copy, cfg)));
  EXPECT_TRUE(files_equal(weights, copy));

  ASSERT_TRUE(WeightsConverter{}.convert(conversion(
      WeightsFormat::Dxxwjz1, dst, WeightsFormat::Dxxwjz1, dst, cfg)));

  WeightsConversion back = conversion(WeightsFormat::Dxxwjz1, dst,
                                      WeightsFormat::Original, copy, "");

  ASSERT_TRUE(WeightsConverter{}.convert(back));
  EXPECT_TRUE(files_equal(weights, copy));

  // no temporary files are left behind
  EXPECT_EQ(files_of(workspace.dir()),
            (std::vector<std::string>{"copy.weights", "replaced.dxxwjz1",
                                      "small.cfg", "small.weights"}));
}

TEST_F(CTEST_WeightsConverter, failed_conversions_leave_no_destination)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const std::string dxxwjz1 = workspace.file("small.dxxwjz1");

  ASSERT_TRUE(WeightsConverter{}.convert(conversion(
      WeightsFormat::Original, weights, WeightsFormat::Dxxwjz1, dxxwjz1, cfg)));

  // the network of another cfg and a broken cfg
  NetSpec other = small_net();
  other.name = "other";
  other.layers[0] = connected(4);

  const std::string other_cfg = workspace.write_cfg(other);
  const std::string broken_cfg = workspace.file("broken.cfg");

  {
    std::ofstream broken{broken_cfg};
    broken << "[net]\nwidth=6\nheight=6\nchannels=2\n[convolutional]\n"
              "filters=2\nsize=3\n[route]\nlayers=-7\n";
  }

  // the corrupted dxxwjz1 file
  const std::string corrupted = workspace.file("corrupted.dxxwjz1");

  std::filesystem::copy_file(dxxwjz1, corrupted);
  std::filesystem::resize_file(corrupted,
                               std::filesystem::file_size(corrupted) / 2U);

  const std::string dst = workspace.file("out/never.file");

  const std::vector<std::pair<std::string, WeightsConversion>> failures{
      {"the original weights without the cfg",
       conversion(WeightsFormat::Original, weights, WeightsFormat::Dxxwjz1, dst,
                  "")},
      {"the missing source",
       conversion(WeightsFormat::Original, workspace.file("missing.weights"),
                  WeightsFormat::Dxxwjz1, dst, cfg)},
      {"the directory source",
       conversion(WeightsFormat::Dxxwjz1, workspace.dir(),
                  WeightsFormat::Original, dst, "")},
      {"the missing cfg",
       conversion(WeightsFormat::Original, weights, WeightsFormat::Dxxwjz1, dst,
                  workspace.file("missing.cfg"))},
      {"the broken cfg", conversion(WeightsFormat::Original, weights,
                                    WeightsFormat::Dxxwjz1, dst, broken_cfg)},
      {"the dxxwjz1 file of another network",
       conversion(WeightsFormat::Dxxwjz1, dxxwjz1, WeightsFormat::Original, dst,
                  other_cfg)},
      {"the corrupted dxxwjz1 file",
       conversion(WeightsFormat::Dxxwjz1, corrupted, WeightsFormat::Original,
                  dst, "")},
      {"the original weights file read as the dxxwjz1 one",
       conversion(WeightsFormat::Dxxwjz1, weights, WeightsFormat::Original, dst,
                  "")},
      {"the dxxwjz1 file read as the original weights one",
       conversion(WeightsFormat::Original, dxxwjz1, WeightsFormat::Dxxwjz1, dst,
                  cfg)},
      {"the empty destination", conversion(WeightsFormat::Original, weights,
                                           WeightsFormat::Dxxwjz1, "", cfg)},
      {"the destination of the file parent",
       conversion(WeightsFormat::Original, weights, WeightsFormat::Dxxwjz1,
                  weights + "/x.dxxwjz1", cfg)},
      {"the destination directory",
       conversion(WeightsFormat::Original, weights, WeightsFormat::Dxxwjz1,
                  workspace.dir(), cfg)}};

  for (const auto& [name, failure] : failures) {
    SCOPED_TRACE(name);

    EXPECT_FALSE(WeightsConverter{}.convert(failure));
    EXPECT_FALSE(std::filesystem::exists(dst));
  }

  // no temporary files are left behind
  EXPECT_EQ(files_of(workspace.dir()),
            (std::vector<std::string>{"broken.cfg", "corrupted.dxxwjz1",
                                      "other.cfg", "small.cfg", "small.dxxwjz1",
                                      "small.weights"}));
}

TEST_F(CTEST_WeightsConverter, array_gaps_fail_the_original_weights_of_the_cfg)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const std::string dxxwjz1 = workspace.file("small.dxxwjz1");

  ASSERT_TRUE(WeightsConverter{}.convert(conversion(
      WeightsFormat::Original, weights, WeightsFormat::Dxxwjz1, dxxwjz1, cfg)));

  // the document edited by hand: the scales of the first layer removed, the
  // arrays after them are there
  darknet_adaptor::loaders::WeightsDocument document;

  ASSERT_TRUE(Dxxwjz1Reader{}.read(dxxwjz1, nullptr, document));
  ASSERT_GT(document.layers[0].arrays.size(), 2U);

  document.layers[0].arrays.erase(document.layers[0].arrays.begin() + 1);

  const std::string gapped = workspace.file("gapped.dxxwjz1");

  ASSERT_TRUE(Dxxwjz1Writer{}.write(document, gapped));

  // the original weights file keeps no names of its arrays, so the arrays
  // after the gap would load into the wrong ones of the network
  const std::string dst = workspace.file("gapped.weights");

  EXPECT_FALSE(WeightsConverter{}.convert(conversion(
      WeightsFormat::Dxxwjz1, gapped, WeightsFormat::Original, dst, cfg)));
  EXPECT_FALSE(std::filesystem::exists(dst));

  // the dxxwjz1 file keeps the names, the network keeps the values of the
  // missing array
  const std::string again = workspace.file("again.dxxwjz1");

  EXPECT_TRUE(WeightsConverter{}.convert(conversion(
      WeightsFormat::Dxxwjz1, gapped, WeightsFormat::Dxxwjz1, again, cfg)));

  // with no network to check against, the document is written as it is
  EXPECT_TRUE(WeightsConverter{}.convert(conversion(
      WeightsFormat::Dxxwjz1, gapped, WeightsFormat::Original, dst, "")));
  EXPECT_EQ(file_size(dst), document.file_size());
}

TEST_F(CTEST_WeightsConverter, stop_requests_stop_the_conversion)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const std::string dst = workspace.file("stopped.dxxwjz1");

  // the stop of the first, the second and the third step
  for (int steps = 0; steps < 3; ++steps) {
    SCOPED_TRACE("the stop after " + std::to_string(steps) + " steps");

    int asked = 0;
    WeightsConversion stopped = conversion(WeightsFormat::Original, weights,
                                           WeightsFormat::Dxxwjz1, dst, cfg);

    stopped.stop_requested = [&asked, steps]() { return asked++ >= steps; };

    EXPECT_FALSE(WeightsConverter{}.convert(stopped));
    EXPECT_FALSE(std::filesystem::exists(dst));
  }

  EXPECT_EQ(files_of(workspace.dir()),
            (std::vector<std::string>{"small.cfg", "small.weights"}));

  // the stop never requested
  WeightsConversion completed = conversion(WeightsFormat::Original, weights,
                                           WeightsFormat::Dxxwjz1, dst, cfg);

  completed.stop_requested = []() { return false; };

  EXPECT_TRUE(WeightsConverter{}.convert(completed));
  EXPECT_TRUE(std::filesystem::exists(dst));
}

TEST_F(CTEST_WeightsConverter, truncated_sources_convert_with_the_warnings)
{
  std::string cfg;
  const std::string weights = saved(small_net(), cfg);
  const uint64_t size = file_size(weights);

  // the file of the part of its floats and the extra bytes
  for (const int64_t change : {-6, -4, 7}) {
    SCOPED_TRACE("the change of " + std::to_string(change) + " bytes");

    const std::string changed = workspace.file("changed.weights");

    std::filesystem::copy_file(
        weights, changed, std::filesystem::copy_options::overwrite_existing);

    if (change > 0) {
      ASSERT_TRUE(append_bytes(changed, static_cast<uint64_t>(change)));
    } else {
      ASSERT_TRUE(
          truncate_file(changed, size - static_cast<uint64_t>(-change)));
    }

    check_back_and_forth(changed, cfg, workspace.dir());
  }
}

/**
 * @brief The files of the generated networks convert back and forth: the
 * files darknet saves and the files of every header version made for the
 * load.
 */
class CTEST_WeightsConverter_variants : public TestWithParam<WeightsVariant>
{
 public:
  WeightsWorkspace workspace{CTEST_WeightsConverter_DATA_DIR};
};

TEST_P(CTEST_WeightsConverter_variants, converts_back_and_forth)
{
  const WeightsVariant& variant = GetParam();
  const std::string cfg = workspace.write_cfg(variant.spec);
  const std::string saved = workspace.file(variant.name + ".weights");
  const std::string made = workspace.file(variant.name + ".made.weights");

  {
    DarknetNetwork darknet{cfg, variant.mode};

    ASSERT_TRUE(darknet.fill(variant.spec.layout(), Pattern{variant.seed}));

    *darknet.net().cur_iteration = variant.iteration;

    ASSERT_TRUE(OrigWeightsWriter{}.save(darknet.net(), saved));
  }

  WeightsHeader header = variant.header;
  header.seen = header.stored_seen();

  LayoutOptions loading;
  loading.loading = true;
  loading.transposed = header.transposed();

  ASSERT_TRUE(write_weights_file(made, header, variant.spec.layout(loading),
                                 Pattern{variant.seed}));

  for (const std::string& weights : {saved, made}) {
    SCOPED_TRACE(weights);

    check_back_and_forth(weights, cfg, workspace.dir());
  }
}

INSTANTIATE_TEST_SUITE_P(Layers, CTEST_WeightsConverter_variants,
                         ValuesIn(variants::layers()), variant_name);

INSTANTIATE_TEST_SUITE_P(
    Random, CTEST_WeightsConverter_variants,
    ValuesIn(variants::random(variants::DEFAULT_RANDOM_SEED,
                              variants::DEFAULT_RANDOM_COUNT)),
    variant_name);
