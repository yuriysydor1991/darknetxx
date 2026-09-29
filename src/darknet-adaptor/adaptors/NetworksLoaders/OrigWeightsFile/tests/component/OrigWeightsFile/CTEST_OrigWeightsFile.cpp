#include <gtest/gtest.h>

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <limits>
#include <numeric>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "CTEST_OrigWeightsFile_provider.h"
#include "DarknetNetwork.h"
#include "WeightsChecks.h"
#include "WeightsFile.h"
#include "WeightsNetSpec.h"
#include "WeightsVariants.h"
#include "src/darknet-adaptor/adaptors/NetworkError.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/OrigWeightsFile/OrigWeightsFile.h"
#include "src/darknet.h"
#include "src/log/default-logger/DefaultLogger.h"
#include "version.h"

using namespace testing;
using namespace weights_test;

using darknet_adaptor::adaptors::NetworkError;
using darknet_adaptor::loaders::OrigDefaultLoader;
using darknet_adaptor::loaders::OrigWeightsFile;

namespace
{

/**
 * @brief Keeps the info messages of the darknet parser (every layer of the
 * thousands of the generated networks) out of the tests output, while the
 * warnings and the errors stay.
 */
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

constexpr const uint64_t SEED = 0xDA2C4E7U;

NetSpec net(const std::string& name, int width, int height, int channels,
            const std::vector<LayerSpec>& layers)
{
  NetSpec spec;
  spec.name = name;
  spec.width = width;
  spec.height = height;
  spec.channels = channels;
  spec.layers = layers;
  return spec;
}

/// @brief The small network of every weights layer kind.
NetSpec every_kind_net()
{
  return net("every_kind", 16, 12, 3,
             {conv(6, 3).with_bn(),                       // 0
              batchnorm(),                                // 1
              conv(6, 1),                                 // 2
              shortcut({-3}, "per_channel", "softmax"),   // 3
              maxpool(2, 2),                              // 4
              local(3, 3, 1, true),                       // 5
              crnn(4, 3, 3).with_bn(),                    // 6
              conv_lstm(3, 3).with_peephole().with_bn(),  // 7
              implicit(5, 2),                             // 8
              conv(4, 1).with_bn(),                       // 9
              rnn(6, 5).with_bn(),                        // 10
              lstm(4).with_bn(),                          // 11
              gru(3),                                     // 12
              connected(7).with_bn(),                     // 13
              softmax()});                                // 14
}

LayoutOptions loading(bool transposed = false)
{
  LayoutOptions options;
  options.loading = true;
  options.transposed = transposed;
  return options;
}

/// @brief Writes the raw floats file: the header and the given payload.
bool write_raw(const std::string& path, const WeightsHeader& header,
               const std::vector<float>& payload)
{
  std::ofstream file(path, std::ios::binary | std::ios::trunc);

  file.write(reinterpret_cast<const char*>(&header.major), sizeof(int32_t));
  file.write(reinterpret_cast<const char*>(&header.minor), sizeof(int32_t));
  file.write(reinterpret_cast<const char*>(&header.revision), sizeof(int32_t));

  if (header.seen64()) {
    file.write(reinterpret_cast<const char*>(&header.seen), sizeof(uint64_t));
  } else {
    const auto seen32 = static_cast<uint32_t>(header.seen);
    file.write(reinterpret_cast<const char*>(&seen32), sizeof(uint32_t));
  }

  file.write(reinterpret_cast<const char*>(payload.data()),
             static_cast<std::streamsize>(payload.size() * sizeof(float)));

  return file.good();
}

/**
 * @brief The in memory file the per layer routines of the OrigWeightsFile
 * write and read.
 */
class TemporaryFile
{
 public:
  TemporaryFile() = default;

  TemporaryFile(const TemporaryFile&) = delete;
  TemporaryFile& operator=(const TemporaryFile&) = delete;

  bool is_open() const { return true; }

  std::iostream& get() const { return mstream; }

  /// @brief Reads from the start of the file again.
  void rewind() const
  {
    mstream.clear();
    mstream.seekg(0);
  }

  /// @brief The count of the bytes written.
  long size() const { return static_cast<long>(mstream.str().size()); }

  /// @brief Reads the count of the values from the current position.
  template <typename T>
  size_t read(T* values, size_t count) const
  {
    mstream.read(reinterpret_cast<char*>(values),
                 static_cast<std::streamsize>(count * sizeof(T)));

    return static_cast<size_t>(mstream.gcount()) / sizeof(T);
  }

  /// @brief Every float of the file.
  std::vector<float> floats() const
  {
    std::vector<float> values;
    float value = 0.f;

    rewind();

    while (read(&value, 1U) == 1U) {
      values.push_back(value);
    }

    return values;
  }

 private:
  mutable std::stringstream mstream{std::ios::in | std::ios::out |
                                    std::ios::binary};
};

void append(std::vector<float>& to, const float* values, int count)
{
  to.insert(to.end(), values, values + count);
}

void fill_values(float* values, int count, float first)
{
  for (int index = 0; index < count; ++index) {
    values[index] = first + static_cast<float>(index) * 0.125f;
  }
}

::testing::AssertionResult same_bits(const std::vector<float>& actual,
                                     const std::vector<float>& expected)
{
  if (actual.size() != expected.size()) {
    return ::testing::AssertionFailure()
           << actual.size() << " floats instead of " << expected.size();
  }

  for (size_t index = 0U; index < actual.size(); ++index) {
    if (bits(actual[index]) != bits(expected[index])) {
      return ::testing::AssertionFailure()
             << actual[index] << " at the index " << index << " instead of "
             << expected[index];
    }
  }

  return ::testing::AssertionSuccess();
}

std::vector<float> copy_of(const float* values, int count)
{
  return std::vector<float>(values, values + count);
}

float lrelu(float value) { return value > 0.001f ? value : 0.001f; }

}  // namespace

class CTEST_OrigWeightsFile : public Test
{
 public:
  WeightsWorkspace workspace;

  /// @brief Writes the file of the pattern values the network loads.
  std::string write_for(const NetSpec& spec, const WeightsHeader& header,
                        const Pattern& pattern, const std::string& name)
  {
    const std::string path = workspace.file(name);

    EXPECT_TRUE(write_weights_file(
        path, header, spec.layout(loading(header.transposed())), pattern));

    return path;
  }
};

TEST_F(CTEST_OrigWeightsFile, save_weights_writes_the_upstream_darknet_version)
{
  // the upstream darknet files version: the loader reads the 64 bits seen of
  // the (major * 10 + minor) >= 2 files and transposes the connected weights
  // of the major or minor > 1000 ones
  EXPECT_EQ(MAJOR_VERSION, 0);
  EXPECT_EQ(MINOR_VERSION, 2);
  EXPECT_EQ(PATCH_VERSION, 5);

  const NetSpec spec = every_kind_net();
  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Inference};
  const std::string weights = workspace.file("every_kind.weights");

  save(darknet.net(), weights);

  WeightsHeader header;

  ASSERT_TRUE(read_header(weights, header));

  EXPECT_EQ(header.major, MAJOR_VERSION);
  EXPECT_EQ(header.minor, MINOR_VERSION);
  EXPECT_EQ(header.revision, PATCH_VERSION);
  EXPECT_TRUE(header.seen64());
  EXPECT_FALSE(header.transposed());
  EXPECT_EQ(file_size(weights),
            header.size() + count_floats(spec.layout()) * sizeof(float));
}

TEST_F(CTEST_OrigWeightsFile,
       save_weights_stores_the_images_seen_by_the_iteration)
{
  struct Case
  {
    int batch;
    int subdivisions;
    int iteration;
  };

  // the mini batch of the 6 / 4 case is the single image
  for (const Case& c :
       {Case{1, 1, 0}, Case{1, 1, 1}, Case{64, 16, 12345}, Case{64, 64, 1000},
        Case{8, 2, INT_MAX}, Case{6, 4, 7}}) {
    SCOPED_TRACE("batch " + std::to_string(c.batch) + ", subdivisions " +
                 std::to_string(c.subdivisions) + ", iteration " +
                 std::to_string(c.iteration));

    NetSpec spec = net("seen", 4, 4, 1, {conv(2, 3)});
    spec.batch = c.batch;
    spec.subdivisions = c.subdivisions;

    DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Train};
    network& net = darknet.net();
    const std::string weights = workspace.file("seen.weights");

    // the stale value save_weights() recomputes
    *net.seen = 777U;
    *net.cur_iteration = c.iteration;

    save(net, weights);

    const uint64_t expected =
        static_cast<uint64_t>(c.iteration) *
        static_cast<uint64_t>(std::max(1, c.batch / c.subdivisions)) *
        static_cast<uint64_t>(c.subdivisions);

    WeightsHeader header;

    ASSERT_TRUE(read_header(weights, header));

    EXPECT_EQ(header.seen, expected);
    EXPECT_EQ(*net.seen, expected);
  }
}

TEST_F(CTEST_OrigWeightsFile, load_weights_reads_the_seen_of_every_version)
{
  NetSpec spec = net("versions", 6, 5, 2, {conv(3, 3).with_bn(), connected(4)});
  spec.batch = 64;
  spec.subdivisions = 16;

  const std::string cfg = workspace.write_cfg(spec);
  const Pattern pattern{SEED};
  const Pattern before{~SEED};
  const Layout memory = spec.layout();

  for (WeightsHeader header : variants::headers()) {
    for (const uint64_t seen :
         {uint64_t{0U}, uint64_t{1U}, uint64_t{4096U}, uint64_t{0xFFFFFFFFU},
          uint64_t{0x100000040ULL}, uint64_t{0x7FFFFFFFFFFFFFC0ULL}}) {
      header.seen = seen;

      SCOPED_TRACE("the version " + header.describe());

      const std::string weights =
          write_for(spec, header, pattern, "versions.weights");

      DarknetNetwork darknet{cfg, ParseMode::Train};
      network& net = darknet.net();

      ASSERT_TRUE(darknet.fill(memory, before));

      load(net, weights);

      // the old 32 bits seen files keep the low half of the value
      EXPECT_EQ(*net.seen, header.stored_seen());

      if (header.stored_seen() / 64U <= static_cast<uint64_t>(INT_MAX)) {
        EXPECT_EQ(*net.cur_iteration,
                  static_cast<int>(header.stored_seen() / 64U));
      }

      const Layout loaded = spec.layout(loading(header.transposed()));
      const FileModel file{loaded, pattern};

      EXPECT_TRUE(darknet.holds_loaded(memory, before, loaded, file,
                                       file.floats() * sizeof(float)));
    }
  }
}

TEST_F(CTEST_OrigWeightsFile,
       load_weights_transposes_the_connected_weights_above_the_1000_version)
{
  // the explicit 3 inputs x 2 outputs case: the file holds the weights as the
  // inputs x outputs matrix
  const NetSpec spec = net("transposed", 2, 2, 3, {avgpool(), connected(2)});
  const std::string cfg = workspace.write_cfg(spec);
  const std::vector<float> payload{10.f, 11.f, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f};

  for (const WeightsHeader& header :
       {WeightsHeader{1001, 0, 0, 0U}, WeightsHeader{0, 1001, 0, 0U},
        WeightsHeader{2000, 3000, 1, 0U}, WeightsHeader{1000, 1000, 0, 0U},
        WeightsHeader{0, 2, 5, 0U}, WeightsHeader{0, 1, 0, 0U}}) {
    SCOPED_TRACE("the version " + header.describe());

    const std::string weights = workspace.file("transposed.weights");

    ASSERT_TRUE(write_raw(weights, header, payload));

    DarknetNetwork darknet{cfg, ParseMode::Inference};
    const layer& l = darknet.net().layers[1];

    load(darknet.net(), weights);

    EXPECT_TRUE(same_bits(copy_of(l.biases, 2), {10.f, 11.f}));
    EXPECT_TRUE(
        same_bits(copy_of(l.weights, 6),
                  header.transposed()
                      ? std::vector<float>{0.f, 2.f, 4.f, 1.f, 3.f, 5.f}
                      : std::vector<float>{0.f, 1.f, 2.f, 3.f, 4.f, 5.f}));
  }

  // every connected weights of the recurrent layers get transposed, the
  // convolutional ones stay
  const NetSpec recurrent =
      net("recurrent", 5, 4, 2,
          {conv(3, 3).with_bn(), connected(4).with_bn(), rnn(5, 3), gru(4),
           lstm(3).with_bn(), connected(2)});
  const std::string recurrent_cfg = workspace.write_cfg(recurrent);
  const Pattern pattern{SEED};
  const Pattern before{~SEED};

  for (const WeightsHeader& header :
       {WeightsHeader{1001, 0, 0, 0U}, WeightsHeader{0, 1001, 0, 0U},
        WeightsHeader{1000, 1000, 0, 0U}}) {
    SCOPED_TRACE("the recurrent version " + header.describe());

    const std::string weights =
        write_for(recurrent, header, pattern, "recurrent.weights");

    DarknetNetwork darknet{recurrent_cfg, ParseMode::Train};

    ASSERT_TRUE(darknet.fill(recurrent.layout(), before));

    load(darknet.net(), weights);

    const Layout loaded = recurrent.layout(loading(header.transposed()));
    const FileModel file{loaded, pattern};

    EXPECT_TRUE(darknet.holds_loaded(recurrent.layout(), before, loaded, file,
                                     file.floats() * sizeof(float)));
  }
}

TEST_F(CTEST_OrigWeightsFile, load_weights_transposes_the_flipped_conv_weights)
{
  // 3 filters of the 2 channels 1x1 kernels: the file holds the weights as
  // the kernel x filters matrix
  const NetSpec spec =
      net("flipped", 2, 2, 2,
          {avgpool(), conv(3, 1).with_flipped().with_pad(false)});
  const std::string cfg = workspace.write_cfg(spec);
  const std::string weights = workspace.file("flipped.weights");
  const std::string resaved = workspace.file("flipped.resaved.weights");

  ASSERT_TRUE(write_raw(weights, WeightsHeader{0, 2, 5, 0U},
                        {10.f, 11.f, 12.f, 0.f, 1.f, 2.f, 3.f, 4.f, 5.f}));

  DarknetNetwork darknet{cfg, ParseMode::Inference};
  const layer& l = darknet.net().layers[1];

  load(darknet.net(), weights);

  EXPECT_TRUE(same_bits(copy_of(l.biases, 3), {10.f, 11.f, 12.f}));
  EXPECT_TRUE(same_bits(copy_of(l.weights, 6), {0.f, 3.f, 1.f, 4.f, 2.f, 5.f}));

  // the save doesn't flip them back
  save(darknet.net(), resaved);

  DarknetNetwork plain{
      workspace.write_cfg(
          net("plain", 2, 2, 2, {avgpool(), conv(3, 1).with_pad(false)})),
      ParseMode::Inference};

  load(plain.net(), resaved);

  EXPECT_TRUE(same_bits(copy_of(plain.net().layers[1].weights, 6),
                        {0.f, 3.f, 1.f, 4.f, 2.f, 5.f}));
}

TEST_F(CTEST_OrigWeightsFile, shared_conv_layers_keep_no_weights_in_the_file)
{
  const NetSpec spec = net("shared", 7, 7, 3,
                           {conv(8, 3).with_bn(), conv(8, 3).with_bn(),
                            conv(8, 3).with_bn().with_share(-1),
                            conv(8, 3).with_bn().with_share(-2)});
  const std::string cfg = workspace.write_cfg(spec);
  const std::string weights = workspace.file("shared.weights");
  const Pattern pattern{SEED};

  {
    DarknetNetwork darknet{cfg, ParseMode::Train};

    ASSERT_TRUE(darknet.fill(spec.layout(), pattern));

    save(darknet.net(), weights);
  }

  // the 3 channels layer: 8 biases, 3 x 8 batch normalization arrays and the
  // 3 x 8 x 3 x 3 weights; the 8 channels one: 8, 3 x 8 and 8 x 8 x 3 x 3
  EXPECT_EQ(file_size(weights), 20U + (8U + 24U + 216U + 8U + 24U + 576U) * 4U);

  DarknetNetwork darknet{cfg, ParseMode::Inference};
  network& net = darknet.net();

  load(net, weights);

  EXPECT_TRUE(darknet.holds(spec.layout(), pattern));

  for (const int index : {2, 3}) {
    EXPECT_EQ(net.layers[index].share_layer, &net.layers[1]);
    EXPECT_EQ(net.layers[index].weights, net.layers[1].weights);
    EXPECT_EQ(net.layers[index].biases, net.layers[1].biases);
    EXPECT_EQ(net.layers[index].scales, net.layers[1].scales);
    EXPECT_EQ(net.layers[index].rolling_mean, net.layers[1].rolling_mean);
    EXPECT_EQ(net.layers[index].rolling_variance,
              net.layers[1].rolling_variance);
  }
}

TEST_F(CTEST_OrigWeightsFile, dontload_layers_consume_no_file_data)
{
  const NetSpec spec =
      net("dontload", 8, 8, 3,
          {conv(4, 3), conv(5, 3).with_dontload(), conv(6, 3)});
  const std::string cfg = workspace.write_cfg(spec);
  const Pattern pattern{SEED};
  const Pattern before{~SEED};
  const Layout memory = spec.layout();
  const Layout loaded = spec.layout(loading());

  // the file without the dontload layer loads as the network expects
  {
    const std::string weights =
        write_for(spec, WeightsHeader{}, pattern, "dontload.weights");

    DarknetNetwork darknet{cfg, ParseMode::Inference};

    ASSERT_TRUE(darknet.fill(memory, before));

    load(darknet.net(), weights);

    const FileModel file{loaded, pattern};

    EXPECT_TRUE(darknet.holds_loaded(memory, before, loaded, file,
                                     file.floats() * sizeof(float)));
  }

  // the file of the whole network (saved by darknet) shifts the arrays: the
  // layer after the dontload one reads the dontload layer data
  const std::string saved = workspace.file("dontload.saved.weights");

  {
    DarknetNetwork darknet{cfg, ParseMode::Inference};

    ASSERT_TRUE(darknet.fill(memory, pattern));

    save(darknet.net(), saved);
  }

  DarknetNetwork darknet{cfg, ParseMode::Inference};
  network& net = darknet.net();

  ASSERT_TRUE(darknet.fill(memory, before));

  load(net, saved);

  const FileModel file{memory, pattern};

  EXPECT_TRUE(darknet.holds_loaded(memory, before, loaded, file,
                                   file.floats() * sizeof(float)));

  std::vector<float> dontload_biases(5U);

  for (uint64_t index = 0U; index < dontload_biases.size(); ++index) {
    dontload_biases[index] =
        pattern.value(Block{1, SubLayer::None, ArrayKind::Biases, 5U}, index);
  }

  EXPECT_TRUE(same_bits(copy_of(net.layers[2].biases, 5), dontload_biases));
}

TEST_F(CTEST_OrigWeightsFile,
       dontloadscales_layers_skip_the_batch_normalization_arrays)
{
  const NetSpec spec =
      net("dontloadscales", 6, 6, 2,
          {conv(4, 3).with_bn().with_dontloadscales(),
           connected(5).with_bn().with_dontloadscales(),
           batchnorm().with_dontloadscales(), conv(3, 1).with_bn()});
  const std::string cfg = workspace.write_cfg(spec);
  const Pattern pattern{SEED};
  const Pattern before{~SEED};
  const Layout memory = spec.layout();
  const Layout loaded = spec.layout(loading());

  // the batchnorm layer loads every array regardless of the option
  EXPECT_EQ(count_floats(memory) - count_floats(loaded), 3U * (4U + 5U));

  const std::string weights =
      write_for(spec, WeightsHeader{}, pattern, "dontloadscales.weights");

  DarknetNetwork darknet{cfg, ParseMode::Train};
  network& net = darknet.net();

  ASSERT_TRUE(darknet.fill(memory, before));

  load(net, weights);

  const FileModel file{loaded, pattern};

  EXPECT_TRUE(darknet.holds_loaded(memory, before, loaded, file,
                                   file.floats() * sizeof(float)));
  EXPECT_EQ(net.layers[0].scales[0],
            before.value(Block{0, SubLayer::None, ArrayKind::Scales, 4U}, 0U));
}

TEST_F(CTEST_OrigWeightsFile, layers_without_weights_keep_nothing_in_the_file)
{
  const NetSpec spec =
      net("no_weights", 12, 10, 3,
          {maxpool(2, 2), local_avgpool(2, 1), upsample(2), reorg(2),
           route({-1}), dropout(), activation("relu"), normalization(), empty(),
           shortcut({-2}), avgpool(), softmax()});
  const std::string cfg = workspace.write_cfg(spec);
  const std::string weights = workspace.file("no_weights.weights");

  DarknetNetwork darknet{cfg, ParseMode::Train};
  network& net = darknet.net();

  ASSERT_TRUE(matches_spec(net, spec, ParseMode::Train));

  *net.cur_iteration = 3;

  save(net, weights);

  EXPECT_EQ(file_size(weights), 20U);
  EXPECT_TRUE(file_matches(weights, saved_header(3U), Layout{}, Pattern{SEED}));

  *net.seen = 0U;

  load(net, weights);

  EXPECT_EQ(*net.seen, 3U);
}

TEST_F(CTEST_OrigWeightsFile,
       save_weights_upto_saves_the_layers_before_the_cutoff)
{
  // 4 floats of the conv layer, 3 + 3 x 8 of the connected one and 4 x 3 of
  // the batchnorm one
  const NetSpec spec =
      net("upto", 2, 2, 1, {conv(2, 1), connected(3), batchnorm()});
  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Train};

  for (const auto& [cutoff, size] :
       std::vector<std::pair<int, uint64_t>>{{INT_MIN, 20U},
                                             {-5, 20U},
                                             {0, 20U},
                                             {1, 36U},
                                             {2, 144U},
                                             {3, 192U},
                                             {100, 192U},
                                             {INT_MAX, 192U}}) {
    SCOPED_TRACE("the cutoff " + std::to_string(cutoff));

    const std::string weights = workspace.file("upto.weights");

    save_upto(darknet.net(), weights, cutoff, false);

    EXPECT_EQ(file_size(weights), size);
  }
}

TEST_F(CTEST_OrigWeightsFile,
       load_weights_upto_loads_the_layers_before_the_cutoff)
{
  const NetSpec spec =
      net("upto", 2, 2, 1, {conv(2, 1), connected(3), batchnorm()});
  const std::string cfg = workspace.write_cfg(spec);
  const std::string weights = workspace.file("upto.weights");
  const Pattern pattern{SEED};
  const Pattern before{~SEED};
  const Layout memory = spec.layout();

  {
    DarknetNetwork darknet{cfg, ParseMode::Train};

    ASSERT_TRUE(darknet.fill(memory, pattern));

    *darknet.net().cur_iteration = 11;

    save(darknet.net(), weights);
  }

  DarknetNetwork darknet{cfg, ParseMode::Train};
  network& net = darknet.net();
  const FileModel file{memory, pattern};

  for (const int cutoff : {INT_MIN, 0, 1, 2, 3, INT_MAX}) {
    SCOPED_TRACE("the cutoff " + std::to_string(cutoff));

    LayoutOptions options = loading();
    options.cutoff = cutoff;

    ASSERT_TRUE(darknet.fill(memory, before));

    *net.seen = 0U;

    load_upto(net, weights, cutoff);

    // the header is read regardless of the cutoff
    EXPECT_EQ(*net.seen, 11U);
    EXPECT_EQ(*net.cur_iteration, 11);
    EXPECT_TRUE(darknet.holds_loaded(memory, before, spec.layout(options), file,
                                     file.floats() * sizeof(float)));
  }
}

TEST_F(CTEST_OrigWeightsFile, save_weights_writes_the_same_bytes_every_time)
{
  const NetSpec spec = every_kind_net();
  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Train};
  network& net = darknet.net();

  ASSERT_TRUE(darknet.fill(spec.layout(), Pattern{SEED}));

  *net.cur_iteration = 5;

  const std::string first = workspace.file("first.weights");
  const std::string second = workspace.file("second.weights");
  const std::string upto_all = workspace.file("upto_all.weights");
  const std::string upto_more = workspace.file("upto_more.weights");

  save(net, first);
  save(net, second);
  save_upto(net, upto_all, net.n, false);
  save_upto(net, upto_more, net.n + 100, false);

  EXPECT_TRUE(files_equal(first, second));
  EXPECT_TRUE(files_equal(first, upto_all));
  EXPECT_TRUE(files_equal(first, upto_more));
}

TEST_F(CTEST_OrigWeightsFile, ema_save_writes_the_ema_arrays_of_the_conv_layers)
{
  check_ema(variants::make("every_kind", every_kind_net(), ParseMode::Train,
                           WeightsHeader{}),
            workspace);

  // the first floats of the file are the ema biases of the first layer
  const NetSpec spec = net("ema", 5, 5, 1, {conv(3, 3).with_bn()});
  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Train};
  const layer& l = darknet.net().layers[0];
  const std::string weights = workspace.file("ema.weights");

  fill_values(l.biases, 3, 1.f);
  fill_values(l.biases_ema, 3, -1.f);
  fill_values(l.scales, 3, 2.f);
  fill_values(l.scales_ema, 3, -2.f);
  fill_values(l.rolling_mean, 3, 3.f);
  fill_values(l.rolling_variance, 3, 4.f);
  fill_values(l.weights, 27, 5.f);
  fill_values(l.weights_ema, 27, -5.f);

  save_upto(darknet.net(), weights, 1, true);

  std::vector<float> expected;
  append(expected, l.biases_ema, 3);
  append(expected, l.scales_ema, 3);
  append(expected, l.rolling_mean, 3);
  append(expected, l.rolling_variance, 3);
  append(expected, l.weights_ema, 27);

  std::ifstream file(weights, std::ios::binary);
  std::vector<float> actual(expected.size() + 1U);

  // the arrays follow the 20 bytes header
  file.seekg(20);
  // cppcheck-suppress invalidPointerCast
  file.read(reinterpret_cast<char*>(actual.data()),
            static_cast<std::streamsize>(actual.size() * sizeof(float)));
  actual.resize(static_cast<size_t>(file.gcount()) / sizeof(float));

  EXPECT_TRUE(same_bits(actual, expected));
}

TEST_F(CTEST_OrigWeightsFile, truncated_files_load_the_whole_floats_they_hold)
{
  const NetSpec spec = every_kind_net();
  const std::string cfg = workspace.write_cfg(spec);
  const Pattern pattern{SEED};
  const Pattern before{~SEED};
  const Layout memory = spec.layout();
  const Layout loaded = spec.layout(loading());
  const FileModel file{loaded, pattern};
  const WeightsHeader header{0, 2, 5, 640U};
  const uint64_t full = header.size() + file.floats() * sizeof(float);

  // every cut of the header seen, of the first floats and around every array
  // edge
  std::set<uint64_t> cuts;

  for (uint64_t cut = 12U; cut < header.size() + 64U; ++cut) {
    cuts.insert(cut);
  }

  for (const uint64_t offset : file.offsets()) {
    const uint64_t edge = header.size() + offset * sizeof(float);

    for (const uint64_t cut : {edge - 1U, edge, edge + 1U, edge + 4U}) {
      if (cut < full) {
        cuts.insert(cut);
      }
    }
  }

  const std::string weights = write_for(spec, header, pattern, "cut.weights");

  ASSERT_EQ(file_size(weights), full);

  for (auto cut = cuts.rbegin(); cut != cuts.rend(); ++cut) {
    SCOPED_TRACE("the file cut at the " + std::to_string(*cut) + " bytes");

    ASSERT_TRUE(truncate_file(weights, *cut));

    DarknetNetwork darknet{cfg, ParseMode::Inference};
    network& net = darknet.net();

    ASSERT_TRUE(darknet.fill(memory, before));

    load(net, weights);

    if (*cut >= header.size()) {
      EXPECT_EQ(*net.seen, 640U);
      EXPECT_TRUE(darknet.holds_loaded(memory, before, loaded, file,
                                       *cut - header.size()));
    } else {
      // the cut seen is indeterminate, the arrays stay
      EXPECT_TRUE(darknet.holds(memory, before));
    }
  }
}

TEST_F(CTEST_OrigWeightsFile, trailing_file_bytes_are_ignored)
{
  const NetSpec spec = every_kind_net();
  const std::string cfg = workspace.write_cfg(spec);
  const Pattern pattern{SEED};
  const Pattern before{~SEED};
  const Layout memory = spec.layout();
  const Layout loaded = spec.layout(loading());
  const std::string weights =
      write_for(spec, WeightsHeader{}, pattern, "trailing.weights");

  ASSERT_TRUE(append_bytes(weights, 13U));

  DarknetNetwork darknet{cfg, ParseMode::Train};

  ASSERT_TRUE(darknet.fill(memory, before));

  load(darknet.net(), weights);

  const FileModel file{loaded, pattern};

  EXPECT_TRUE(darknet.holds_loaded(memory, before, loaded, file,
                                   file.floats() * sizeof(float)));
}

TEST_F(CTEST_OrigWeightsFile, load_fails_on_a_missing_file)
{
  // the original darknet load_weights() ended the process instead
  const NetSpec spec = net("missing", 4, 4, 1, {conv(2, 3)});
  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Inference};
  const std::string missing = workspace.file("missing.weights");

  EXPECT_FALSE(OrigWeightsFile::load(darknet.net(), missing));
  EXPECT_FALSE(OrigWeightsFile::load(darknet.net(), missing, 1));
}

TEST_F(CTEST_OrigWeightsFile, save_fails_on_an_unwritable_path)
{
  // the original darknet save_weights() ended the process instead
  const NetSpec spec = net("unwritable", 4, 4, 1, {conv(2, 3)});
  DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Inference};
  const std::string unwritable = workspace.file("no/such/dir/x.weights");

  EXPECT_FALSE(OrigWeightsFile::save(darknet.net(), unwritable));
  EXPECT_FALSE(OrigWeightsFile::save(darknet.net(), unwritable, 1, true));
}

TEST_F(CTEST_OrigWeightsFile, load_network_throws_on_a_missing_weights_file)
{
  const std::string cfg =
      workspace.write_cfg(net("missing", 4, 4, 1, {conv(2, 3)}));
  const std::string missing = workspace.file("missing.weights");

  OrigDefaultLoader loader;

  EXPECT_THROW(loader.load_network(cfg, missing, false), NetworkError);
  EXPECT_THROW(loader.load_network_custom(cfg, missing, false, 1),
               NetworkError);
}

TEST_F(CTEST_OrigWeightsFile, special_float_values_round_trip_bit_exact)
{
  static const uint32_t specials[] = {
      0x00000000U,  // +0
      0x80000000U,  // -0
      0x00000001U,  // the smallest denormal
      0x807FFFFFU,  // the biggest negative denormal
      0x00800000U,  // FLT_MIN
      0x7F7FFFFFU,  // FLT_MAX
      0xFF7FFFFFU,  // -FLT_MAX
      0x7F800000U,  // +inf
      0xFF800000U,  // -inf
      0x7FC00000U,  // the quiet NaN
      0xFFC12345U,  // the negative quiet NaN with the payload
      0x7FA00001U,  // the signaling NaN
      0x3F800000U,  // 1
      0x34000000U,  // FLT_EPSILON
  };
  constexpr const size_t specials_count =
      sizeof(specials) / sizeof(specials[0]);

  const NetSpec spec = every_kind_net();
  const std::string cfg = workspace.write_cfg(spec);
  const std::string weights = workspace.file("specials.weights");
  const Layout layout = spec.layout();

  {
    DarknetNetwork darknet{cfg, ParseMode::Train};

    for (size_t block = 0U; block < layout.size(); ++block) {
      float* values = darknet.array(layout[block]);

      ASSERT_NE(values, nullptr) << describe(layout[block]);

      for (uint64_t index = 0U; index < layout[block].count; ++index) {
        std::memcpy(&values[index], &specials[(block + index) % specials_count],
                    sizeof(float));
      }
    }

    save(darknet.net(), weights);
  }

  DarknetNetwork darknet{cfg, ParseMode::Inference};

  ASSERT_TRUE(darknet.fill(layout, Pattern{SEED}));

  load(darknet.net(), weights);

  for (size_t block = 0U; block < layout.size(); ++block) {
    const float* values = darknet.array(layout[block]);

    for (uint64_t index = 0U; index < layout[block].count; ++index) {
      uint32_t actual = 0U;
      std::memcpy(&actual, &values[index], sizeof(actual));

      ASSERT_EQ(actual, specials[(block + index) % specials_count])
          << describe(layout[block]) << " at the index " << index;
    }
  }
}

TEST_F(CTEST_OrigWeightsFile, load_network_loads_the_weights_as_they_are)
{
  const NetSpec spec = every_kind_net();
  std::string cfg = workspace.write_cfg(spec);
  std::string weights = workspace.file("load_network.weights");
  const Pattern pattern{SEED};
  const Layout layout = spec.layout();

  {
    DarknetNetwork darknet{cfg, ParseMode::Train};

    ASSERT_TRUE(darknet.fill(layout, pattern));

    *darknet.net().cur_iteration = 21;

    save(darknet.net(), weights);
  }

  for (const int clear : {0, 1}) {
    SCOPED_TRACE("clear " + std::to_string(clear));

    DarknetNetwork darknet{
        OrigDefaultLoader{}.load_network(cfg, weights, clear != 0)};
    network& net = darknet.net();

    ASSERT_TRUE(matches_spec(net, spec, ParseMode::Train));

    // no batch normalization fusion
    EXPECT_TRUE(darknet.holds(layout, pattern));
    EXPECT_EQ(net.layers[0].batch_normalize, 1);

    // the clear starts the training from the scratch
    EXPECT_EQ(*net.seen,
              clear != 0 ? 0U : saved_seen(spec, ParseMode::Train, 21));
    EXPECT_EQ(*net.cur_iteration, clear != 0 ? 0 : 21);
  }
}

TEST_F(CTEST_OrigWeightsFile,
       load_network_custom_sets_the_batch_and_fuses_the_batch_normalization)
{
  const NetSpec spec = net("custom", 6, 6, 2,
                           {conv(4, 3).with_bn(), conv(4, 1).with_bn(),
                            shortcut({-2}, "per_channel", "relu"),
                            shortcut({-3}, "per_feature", "softmax"),
                            conv(3, 3), connected(5).with_bn()});
  std::string cfg = workspace.write_cfg(spec);
  std::string weights = workspace.file("custom.weights");
  const Pattern pattern{SEED};
  const Layout layout = spec.layout();

  {
    DarknetNetwork darknet{cfg, ParseMode::Train};

    ASSERT_TRUE(darknet.fill(layout, pattern));

    *darknet.net().cur_iteration = 4;

    save(darknet.net(), weights);
  }

  DarknetNetwork darknet{
      OrigDefaultLoader{}.load_network_custom(cfg, weights, false, 3)};
  network& net = darknet.net();

  EXPECT_EQ(net.batch, 3);
  // the 4 images of the 4 single image iterations make the single iteration
  // of the forced 3 images batch
  EXPECT_EQ(*net.seen, 4U);
  EXPECT_EQ(*net.cur_iteration, 1);

  const auto value = [&pattern](int layer_index, ArrayKind array,
                                uint64_t index) {
    return pattern.value(Block{layer_index, SubLayer::None, array, 0U}, index);
  };

  // the conv batch normalization gets folded into the biases and the weights
  for (const int index : {0, 1}) {
    const layer& l = net.layers[index];
    const int filter_size = l.size * l.size * l.c / l.groups;

    EXPECT_EQ(l.batch_normalize, 0);
    EXPECT_EQ(l.scales, nullptr);
    EXPECT_EQ(l.rolling_mean, nullptr);
    EXPECT_EQ(l.rolling_variance, nullptr);

    for (int f = 0; f < l.n; ++f) {
      const auto filter = static_cast<uint64_t>(f);
      const float scale = value(index, ArrayKind::Scales, filter);
      const double root =
          std::sqrt(static_cast<double>(
                        value(index, ArrayKind::RollingVariance, filter)) +
                    .00001);
      const auto bias = static_cast<float>(
          static_cast<double>(value(index, ArrayKind::Biases, filter)) -
          static_cast<double>(scale) *
              static_cast<double>(
                  value(index, ArrayKind::RollingMean, filter)) /
              root);

      EXPECT_NEAR(l.biases[f], bias, 1e-6f * std::max(1.f, std::fabs(bias)));

      for (int i = 0; i < filter_size; ++i) {
        const auto weight_index = static_cast<uint64_t>(f * filter_size + i);
        const auto weight =
            static_cast<float>(static_cast<double>(value(
                                   index, ArrayKind::Weights, weight_index)) *
                               (static_cast<double>(scale) / root));

        ASSERT_NEAR(l.weights[weight_index], weight,
                    1e-6f * std::max(1.f, std::fabs(weight)))
            << "the layer " << index << " weight " << weight_index;
      }
    }
  }

  // the shortcut weights get normalized
  for (const int index : {2, 3}) {
    const layer& l = net.layers[index];
    const bool softmax_normalization = index == 3;
    const int step = l.nweights / (l.n + 1);

    EXPECT_EQ(l.weights_normalization, NO_NORMALIZATION);

    for (int channel = 0; channel < step; ++channel) {
      std::vector<float> source;

      for (int input = 0; input <= l.n; ++input) {
        source.push_back(value(index, ArrayKind::Weights,
                               static_cast<uint64_t>(channel + input * step)));
      }

      const float max_value = *std::max_element(source.begin(), source.end());
      const auto normalized = [&](float weight) {
        return softmax_normalization ? std::exp(weight - max_value)
                                     : lrelu(weight);
      };
      // summed in the fuse_conv_batchnorm() order, from the same epsilon
      const float sum = std::accumulate(source.begin(), source.end(), 0.0001f,
                                        [&](float total, float weight) {
                                          return total + normalized(weight);
                                        });

      for (int input = 0; input <= l.n; ++input) {
        const float expected =
            normalized(source[static_cast<size_t>(input)]) / sum;

        EXPECT_NEAR(l.weights[channel + input * step], expected, 1e-6f)
            << "the layer " << index << " channel " << channel;
      }
    }
  }

  // the rest stays as it was saved
  Layout unfused;

  std::copy_if(layout.begin(), layout.end(), std::back_inserter(unfused),
               [](const Block& block) { return block.layer >= 4; });

  EXPECT_TRUE(darknet.holds(unfused, pattern));
  EXPECT_EQ(net.layers[5].batch_normalize, 1);
}

TEST_F(CTEST_OrigWeightsFile, reference_weights_files_resave_byte_identical)
{
  for (const auto& [cfg, weights] :
       std::vector<std::pair<std::string, std::string>>{
           {CTEST_OrigWeightsFile_provider::CFG_1,
            CTEST_OrigWeightsFile_provider::W_1},
           {CTEST_OrigWeightsFile_provider::CFG_2,
            CTEST_OrigWeightsFile_provider::W_2}}) {
    SCOPED_TRACE(weights);

    const std::string resaved = workspace.file("resaved.weights");

    DarknetNetwork darknet{cfg, ParseMode::Inference};

    load(darknet.net(), weights);
    save(darknet.net(), resaved);

    EXPECT_TRUE(files_equal(weights, resaved));
  }
}

TEST_F(CTEST_OrigWeightsFile, transpose_matrix_transposes_the_row_major_matrix)
{
  for (const auto& [rows, cols] : std::vector<std::pair<int, int>>{
           {1, 1}, {1, 5}, {5, 1}, {2, 3}, {3, 2}, {7, 11}, {64, 3}}) {
    SCOPED_TRACE(std::to_string(rows) + "x" + std::to_string(cols));

    std::vector<float> values(static_cast<size_t>(rows * cols));

    for (size_t index = 0U; index < values.size(); ++index) {
      values[index] = static_cast<float>(index);
    }

    const std::vector<float> source = values;

    OrigWeightsFile::transpose_matrix(values.data(), rows, cols);

    for (int x = 0; x < rows; ++x) {
      for (int y = 0; y < cols; ++y) {
        ASSERT_EQ(values[static_cast<size_t>(y * rows + x)],
                  source[static_cast<size_t>(x * cols + y)]);
      }
    }
  }
}

TEST_F(CTEST_OrigWeightsFile,
       binary_conv_routines_keep_the_filter_mean_and_the_weight_signs)
{
  // 2 filters of the 3 channels 3x3 kernels: 27 weights a filter, the file
  // keeps the signs of the 27 / 8 * 8 = 24 first of them
  const NetSpec spec = net("binary", 3, 3, 3, {conv(2, 3).with_binary()});
  const std::string cfg = workspace.write_cfg(spec);

  DarknetNetwork first{cfg, ParseMode::Inference};
  layer& l = first.net().layers[0];

  ASSERT_NE(l.binary_weights, nullptr);

  l.biases[0] = 0.5f;
  l.biases[1] = -0.25f;

  for (int index = 0; index < 27; ++index) {
    l.weights[index] = index % 3 == 0 ? -0.5f : 0.5f;
    l.weights[27 + index] = index % 2 == 0 ? -0.25f : 0.25f;
  }

  const TemporaryFile file;

  ASSERT_TRUE(file.is_open());

  OrigWeightsFile::save_convolutional_weights_binary(l, file.get());

  // the biases, then the mean and 3 bytes of the signs per filter
  EXPECT_EQ(file.size(), 2 * 4 + 2 * (4 + 3));

  file.rewind();

  float biases[2] = {0.f, 0.f};
  ASSERT_EQ(file.read(biases, 2U), 2U);
  EXPECT_EQ(biases[0], 0.5f);
  EXPECT_EQ(biases[1], -0.25f);

  for (int filter = 0; filter < 2; ++filter) {
    float mean = 0.f;
    unsigned char signs[3] = {0U, 0U, 0U};

    ASSERT_EQ(file.read(&mean, 1U), 1U);
    ASSERT_EQ(file.read(signs, 3U), 3U);

    EXPECT_EQ(mean, filter == 0 ? 0.5f : 0.25f);

    for (int index = 0; index < 24; ++index) {
      const bool positive = l.weights[filter * 27 + index] > 0.f;
      const bool bit = (signs[index / 8] & (1U << (index % 8))) != 0U;

      EXPECT_EQ(bit, positive) << "filter " << filter << " weight " << index;
    }
  }

  DarknetNetwork second{cfg, ParseMode::Inference};
  layer& loaded = second.net().layers[0];

  std::fill(loaded.weights, loaded.weights + 54, 7.f);

  file.rewind();

  OrigWeightsFile::load_convolutional_weights_binary(loaded, file.get());

  EXPECT_EQ(loaded.biases[0], 0.5f);
  EXPECT_EQ(loaded.biases[1], -0.25f);

  for (int filter = 0; filter < 2; ++filter) {
    const float mean = filter == 0 ? 0.5f : 0.25f;

    for (int index = 0; index < 27; ++index) {
      const int weight = filter * 27 + index;
      // the tail of the filter isn't stored and stays as it was
      const float expected =
          index >= 24 ? 7.f : (l.weights[weight] > 0.f ? mean : -mean);

      EXPECT_EQ(loaded.weights[weight], expected)
          << "filter " << filter << " weight " << index;
    }
  }
}

TEST_F(CTEST_OrigWeightsFile,
       layer_routines_write_and_read_their_arrays_in_order)
{
  const NetSpec spec = net(
      "routines", 4, 4, 2,
      {conv(3, 3).with_bn(), batchnorm(), shortcut({-2}, "per_channel", "relu"),
       implicit(5, 2), connected(4).with_bn()});
  const std::string cfg = workspace.write_cfg(spec);

  DarknetNetwork first{cfg, ParseMode::Train};
  DarknetNetwork second{cfg, ParseMode::Train};

  ASSERT_TRUE(first.fill(spec.layout(), Pattern{SEED}));
  ASSERT_TRUE(
      first.fill(spec.layout(LayoutOptions{std::numeric_limits<int>::max(),
                                           true, false, false}),
                 Pattern{SEED}));
  ASSERT_TRUE(second.fill(spec.layout(), Pattern{~SEED}));

  const layer& conv_layer = first.net().layers[0];
  const layer& bn_layer = first.net().layers[1];
  const layer& shortcut_layer = first.net().layers[2];
  const layer& implicit_layer = first.net().layers[3];
  const layer& connected = first.net().layers[4];

  // the convolutional layer: biases, scales, rolling mean and variance,
  // weights; the EMA flavour stores the EMA biases, scales and weights
  {
    std::vector<float> expected;
    append(expected, conv_layer.biases, 3);
    append(expected, conv_layer.scales, 3);
    append(expected, conv_layer.rolling_mean, 3);
    append(expected, conv_layer.rolling_variance, 3);
    append(expected, conv_layer.weights, conv_layer.nweights);

    const TemporaryFile file;
    ASSERT_TRUE(file.is_open());

    OrigWeightsFile::save_convolutional_weights(conv_layer, file.get());

    EXPECT_TRUE(same_bits(file.floats(), expected));

    file.rewind();

    const layer& target = second.net().layers[0];

    OrigWeightsFile::load_convolutional_weights(target, file.get());

    EXPECT_TRUE(
        same_bits(copy_of(target.biases, 3), copy_of(conv_layer.biases, 3)));
    EXPECT_TRUE(
        same_bits(copy_of(target.scales, 3), copy_of(conv_layer.scales, 3)));
    EXPECT_TRUE(same_bits(copy_of(target.rolling_mean, 3),
                          copy_of(conv_layer.rolling_mean, 3)));
    EXPECT_TRUE(same_bits(copy_of(target.rolling_variance, 3),
                          copy_of(conv_layer.rolling_variance, 3)));
    EXPECT_TRUE(same_bits(copy_of(target.weights, target.nweights),
                          copy_of(conv_layer.weights, conv_layer.nweights)));

    std::vector<float> expected_ema;
    append(expected_ema, conv_layer.biases_ema, 3);
    append(expected_ema, conv_layer.scales_ema, 3);
    append(expected_ema, conv_layer.rolling_mean, 3);
    append(expected_ema, conv_layer.rolling_variance, 3);
    append(expected_ema, conv_layer.weights_ema, conv_layer.nweights);

    const TemporaryFile ema_file;
    ASSERT_TRUE(ema_file.is_open());

    OrigWeightsFile::save_convolutional_weights_ema(conv_layer, ema_file.get());

    EXPECT_TRUE(same_bits(ema_file.floats(), expected_ema));
  }

  // the batchnorm layer: biases, scales, rolling mean and variance
  {
    std::vector<float> expected;
    append(expected, bn_layer.biases, 3);
    append(expected, bn_layer.scales, 3);
    append(expected, bn_layer.rolling_mean, 3);
    append(expected, bn_layer.rolling_variance, 3);

    const TemporaryFile file;
    ASSERT_TRUE(file.is_open());

    OrigWeightsFile::save_batchnorm_weights(bn_layer, file.get());

    EXPECT_TRUE(same_bits(file.floats(), expected));

    file.rewind();

    const layer& target = second.net().layers[1];

    OrigWeightsFile::load_batchnorm_weights(target, file.get());

    std::vector<float> actual;
    append(actual, target.biases, 3);
    append(actual, target.scales, 3);
    append(actual, target.rolling_mean, 3);
    append(actual, target.rolling_variance, 3);

    EXPECT_TRUE(same_bits(actual, expected));
  }

  // the shortcut and the implicit layers: the weights alone
  for (const int index : {2, 3}) {
    const layer& source = index == 2 ? shortcut_layer : implicit_layer;
    const layer& target = second.net().layers[index];
    const std::vector<float> expected =
        copy_of(source.weights, source.nweights);

    const TemporaryFile file;
    ASSERT_TRUE(file.is_open());

    if (index == 2) {
      OrigWeightsFile::save_shortcut_weights(source, file.get());
    } else {
      OrigWeightsFile::save_implicit_weights(source, file.get());
    }

    EXPECT_TRUE(same_bits(file.floats(), expected));

    file.rewind();

    if (index == 2) {
      OrigWeightsFile::load_shortcut_weights(target, file.get());
    } else {
      OrigWeightsFile::load_implicit_weights(target, file.get());
    }

    EXPECT_TRUE(same_bits(copy_of(target.weights, target.nweights), expected));
  }

  // the connected layer: biases, weights, then scales, rolling mean and
  // variance; the transposing load takes the inputs x outputs weights
  {
    const int outputs = connected.outputs;
    const int inputs = connected.inputs;

    std::vector<float> expected;
    append(expected, connected.biases, outputs);
    append(expected, connected.weights, outputs * inputs);
    append(expected, connected.scales, outputs);
    append(expected, connected.rolling_mean, outputs);
    append(expected, connected.rolling_variance, outputs);

    const TemporaryFile file;
    ASSERT_TRUE(file.is_open());

    OrigWeightsFile::save_connected_weights(connected, file.get());

    EXPECT_TRUE(same_bits(file.floats(), expected));

    for (const int transpose : {0, 1}) {
      SCOPED_TRACE("transpose " + std::to_string(transpose));

      file.rewind();

      const layer& target = second.net().layers[4];

      OrigWeightsFile::load_connected_weights(target, file.get(),
                                              transpose != 0);

      std::vector<float> actual;
      append(actual, target.biases, outputs);

      if (transpose != 0) {
        for (int input = 0; input < inputs; ++input) {
          for (int output = 0; output < outputs; ++output) {
            actual.push_back(target.weights[output * inputs + input]);
          }
        }
      } else {
        append(actual, target.weights, outputs * inputs);
      }

      append(actual, target.scales, outputs);
      append(actual, target.rolling_mean, outputs);
      append(actual, target.rolling_variance, outputs);

      EXPECT_TRUE(same_bits(actual, expected));
    }
  }
}

TEST_F(CTEST_OrigWeightsFile, conv_file_size_does_not_depend_on_the_resolution)
{
  // 8 biases, 3 x 8 batch normalization and the 3 x 8 x 8 x 8 weights of
  // the patchifying stem; the 16 filters 3x3 layer; the 32 filters 3x3 one
  // and the 4 filters 1x1 one
  const uint64_t floats = (8U + 24U + 3U * 8U * 64U) +
                          (16U + 48U + 8U * 16U * 9U) +
                          (32U + 96U + 16U * 32U * 9U) + (4U + 32U * 4U);
  const Pattern pattern{SEED};
  std::string reference;

  for (const Resolution& r : variants::resolutions()) {
    const std::string name =
        "conv_" + std::to_string(r.width) + "x" + std::to_string(r.height);

    SCOPED_TRACE(name);

    const NetSpec spec =
        net(name, r.width, r.height, 3,
            {conv(8, 8, 8).with_bn(), conv(16, 3).with_bn(), maxpool(2, 2),
             conv(32, 3, 2).with_bn(), conv(4, 1)});
    const std::string weights = workspace.file(name + ".weights");

    DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Inference};

    ASSERT_TRUE(matches_spec(darknet.net(), spec, ParseMode::Inference));
    ASSERT_TRUE(darknet.fill(spec.layout(), pattern));

    save(darknet.net(), weights);

    EXPECT_EQ(file_size(weights), 20U + floats * sizeof(float));

    if (reference.empty()) {
      reference = weights;
    } else {
      EXPECT_TRUE(files_equal(reference, weights));
    }
  }
}

TEST_F(CTEST_OrigWeightsFile,
       connected_and_local_file_sizes_follow_the_resolution)
{
  for (const Resolution& r : variants::resolutions()) {
    for (const int channels : {1, 3}) {
      const auto w = static_cast<uint64_t>(r.width);
      const auto h = static_cast<uint64_t>(r.height);
      const auto c = static_cast<uint64_t>(channels);
      const std::string name = "res_" + std::to_string(r.width) + "x" +
                               std::to_string(r.height) + "x" +
                               std::to_string(channels);

      SCOPED_TRACE(name);

      // a bias and a weight of every input
      {
        // the linear activation keeps the w > 1 and h > 1 input darknet
        // requires (see the NetSpec::has_2d_layer)
        const NetSpec spec =
            net(name + "_connected", r.width, r.height, channels,
                {activation("linear"), connected(1)});
        const std::string weights = workspace.file(name + "_connected.weights");

        DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Inference};

        save(darknet.net(), weights);

        EXPECT_EQ(file_size(weights), 20U + (1U + w * h * c) * sizeof(float));
      }

      // a bias of every output pixel, a weight of every channel of it
      {
        const NetSpec spec = net(name + "_local", r.width, r.height, channels,
                                 {local(1, 1, 2, false)});
        const std::string weights = workspace.file(name + "_local.weights");
        const uint64_t locations = ((w - 1U) / 2U + 1U) * ((h - 1U) / 2U + 1U);

        DarknetNetwork darknet{workspace.write_cfg(spec), ParseMode::Inference};

        save(darknet.net(), weights);

        EXPECT_EQ(file_size(weights),
                  20U + (locations + c * locations) * sizeof(float));
      }
    }
  }
}

TEST_F(CTEST_OrigWeightsFile,
       generated_variants_cover_the_weights_file_features)
{
  std::vector<WeightsVariant> all = variants::layers();

  for (const auto& more : {variants::resolutions_sweep(),
                           variants::random(variants::DEFAULT_RANDOM_SEED,
                                            variants::DEFAULT_RANDOM_COUNT),
                           variants::big()}) {
    all.insert(all.end(), more.begin(), more.end());
  }

  std::set<std::string> names;
  std::set<LayerKind> kinds;
  std::set<std::string> weights_types;
  std::set<std::string> header_versions;
  std::set<std::string> resolutions;
  std::set<ParseMode> modes;
  bool conv_flags[5] = {false, false, false, false, false};
  uint64_t smallest = std::numeric_limits<uint64_t>::max();
  uint64_t biggest = 0U;

  for (const WeightsVariant& variant : all) {
    EXPECT_TRUE(names.insert(variant.name).second)
        << "the duplicated name " << variant.name;
    EXPECT_TRUE(variant.spec.has_2d_layer())
        << variant.name << " crashes the darknet parser";

    modes.insert(variant.mode);
    header_versions.insert(variant.header.describe().substr(
        0U, variant.header.describe().find(' ')));
    resolutions.insert(std::to_string(variant.spec.width) + "x" +
                       std::to_string(variant.spec.height));

    for (const LayerSpec& l : variant.spec.layers) {
      kinds.insert(l.kind);

      if (l.kind == LayerKind::Shortcut) {
        weights_types.insert(l.weights_type);
      }

      if (l.kind == LayerKind::Convolutional) {
        conv_flags[0] = conv_flags[0] || l.flipped;
        conv_flags[1] = conv_flags[1] || l.share_index != 0;
        conv_flags[2] = conv_flags[2] || l.binary;
        conv_flags[3] = conv_flags[3] || l.xnor;
        conv_flags[4] = conv_flags[4] || l.groups > 1;
      }
    }

    const uint64_t size =
        20U + count_floats(variant.spec.layout()) * sizeof(float);

    smallest = std::min(smallest, size);
    biggest = std::max(biggest, size);
  }

  for (const LayerKind kind :
       {LayerKind::Convolutional, LayerKind::Connected,
        LayerKind::Batchnorm,     LayerKind::Shortcut,
        LayerKind::Implicit,      LayerKind::Local,
        LayerKind::Rnn,           LayerKind::Gru,
        LayerKind::Lstm,          LayerKind::Crnn,
        LayerKind::ConvLstm,      LayerKind::Maxpool,
        LayerKind::LocalAvgpool,  LayerKind::Avgpool,
        LayerKind::Route,         LayerKind::Upsample,
        LayerKind::Reorg,         LayerKind::Dropout,
        LayerKind::Activation,    LayerKind::Normalization,
        LayerKind::Empty,         LayerKind::Softmax,
        LayerKind::Yolo}) {
    EXPECT_EQ(kinds.count(kind), 1U) << "no " << to_string(kind) << " layer";
  }

  for (const char* const type :
       {"none", "per_feature", "per_layer", "per_channel"}) {
    EXPECT_EQ(weights_types.count(type), 1U) << "no " << type << " shortcut";
  }

  for (const WeightsHeader& header : variants::headers()) {
    const std::string version =
        header.describe().substr(0U, header.describe().find(' '));

    EXPECT_EQ(header_versions.count(version), 1U) << "no " << version;
  }

  for (const bool flag : conv_flags) {
    EXPECT_TRUE(flag);
  }

  EXPECT_EQ(modes.size(), 2U);
  EXPECT_EQ(resolutions.count("2x2"), 1U);
  EXPECT_EQ(resolutions.count("3840x2160"), 1U);
  EXPECT_EQ(resolutions.count("4096x2160"), 1U);
  // from the bare header files up to the hundreds of megabytes
  EXPECT_LE(smallest, 64U);
  EXPECT_GE(biggest, 200U * 1024U * 1024U);
}
