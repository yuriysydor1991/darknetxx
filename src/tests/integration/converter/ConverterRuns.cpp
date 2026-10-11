#include "ConverterRuns.h"

#include <sys/wait.h>

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <limits>
#include <random>
#include <string>
#include <tuple>
#include <vector>

#include "ITEST_converter_provider.h"
#include "NetworkChecks.h"
#include "WeightsFile.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Reader.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Reader.h"
#include "src/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/NetworksLoaders/original-weights/OrigWeightsWriter.h"

namespace converter_test
{

using darknet_adaptor::loaders::OrigWeightsReader;
using darknet_adaptor::loaders::OrigWeightsWriter;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Reader;
using darknet_adaptor::loaders::dxxwjz2::v2::Dxxwjz2Reader;

namespace
{

/// @brief The lines of the log shown for a failed run, the end tells the most.
constexpr const size_t LOG_TAIL = 4000U;

/// @brief The networks of the tens of millions of the floats (the 4K
/// resolution ones), the JSON text of which takes the minutes to write and to
/// read in the Debug build: they convert the file darknet saves alone.
constexpr const uint64_t HUGE_FLOATS = 4U * 1024U * 1024U;

/// @brief Quotes the argument for the POSIX shell.
std::string quote(const std::string& arg)
{
  std::string quoted{"'"};

  for (const char c : arg) {
    if (c == '\'') {
      quoted += "'\\''";
    } else {
      quoted += c;
    }
  }

  return quoted + "'";
}

std::string tail_of(const RunResult& result)
{
  const std::string output = result.output();

  return output.size() > LOG_TAIL ? output.substr(output.size() - LOG_TAIL)
                                  : output;
}

/// @brief The loader reads every array the file keeps: no dontload layers
/// and no dontloadscales batch normalization arrays, which the original
/// loader reads the next arrays in place of.
bool loads_every_array(const Layout& file, const Layout& loaded)
{
  if (file.size() != loaded.size()) {
    return false;
  }

  for (size_t index = 0U; index < file.size(); ++index) {
    if (file[index].layer != loaded[index].layer ||
        file[index].sub != loaded[index].sub ||
        file[index].array != loaded[index].array) {
      return false;
    }
  }

  return true;
}

void same_arrays(DarknetNetwork& first, DarknetNetwork& second,
                 const Layout& memory)
{
  for (const Block& block : memory) {
    const float* a = first.array(block);
    const float* b = second.array(block);

    ASSERT_EQ(a == nullptr, b == nullptr) << describe(block);

    if (a == nullptr) {
      continue;
    }

    for (uint64_t index = 0U; index < block.count; ++index) {
      ASSERT_EQ(bits(a[index]), bits(b[index]))
          << describe(block) << " at the index " << index;
    }
  }
}

}  // namespace

std::string RunResult::output() const
{
  std::ifstream file{log};

  return std::string{std::istreambuf_iterator<char>{file},
                     std::istreambuf_iterator<char>{}};
}

RunResult run(const std::string& executable,
              const std::vector<std::string>& args, const std::string& log)
{
  std::string command = quote(executable);

  for (const std::string& arg : args) {
    command += " " + quote(arg);
  }

  command += " > " + quote(log) + " 2>&1";

  const int status = std::system(command.c_str());

  RunResult result;
  result.log = log;
  result.status = status != -1 && WIFEXITED(status) ? WEXITSTATUS(status) : -1;

  return result;
}

std::vector<std::string> conversion_args(const std::string& from,
                                         const std::string& src,
                                         const std::string& to,
                                         const std::string& dst,
                                         const std::string& cfg)
{
  std::vector<std::string> args{"--convert-from", from, "--convert-src", src,
                                "--convert-to",   to,   "--convert-dst", dst};

  if (!cfg.empty()) {
    args.insert(args.end(), {"--convert-cfg", cfg});
  }

  return args;
}

bool is_gzip(const std::string& path)
{
  std::ifstream file{path, std::ios::binary};
  unsigned char magic[2] = {0U, 0U};

  file.read(reinterpret_cast<char*>(magic), sizeof(magic));

  return file.gcount() == 2 && magic[0] == 0x1FU && magic[1] == 0x8BU;
}

void check_back_and_forth(const std::string& executable,
                          const std::string& weights, const std::string& cfg,
                          const WeightsWorkspace& workspace)
{
  const std::string dxxwjz1 = weights + ".dxxwjz1";
  const std::string back = weights + ".back.weights";
  const std::string validated = weights + ".validated.weights";
  const std::string again = weights + ".again.dxxwjz1";
  const std::string log = workspace.file("converter.log");

  RunResult result =
      run(executable,
          conversion_args("weights", weights, "dxxwjz1", dxxwjz1, cfg), log);

  ASSERT_EQ(result.status, 0) << "the conversion into " << dxxwjz1 << ":\n"
                              << tail_of(result);
  EXPECT_TRUE(is_gzip(dxxwjz1));

  // the dxxwjz1 file keeps its structure, so it converts back without the
  // network cfg file
  result = run(executable,
               conversion_args("dxxwjz1", dxxwjz1, "weights", back, ""), log);

  ASSERT_EQ(result.status, 0) << "the conversion into " << back << ":\n"
                              << tail_of(result);
  EXPECT_TRUE(files_equal(weights, back));

  // and with the network cfg file the file is validated against it
  result =
      run(executable,
          conversion_args("dxxwjz1", dxxwjz1, "weights", validated, cfg), log);

  ASSERT_EQ(result.status, 0) << "the conversion into " << validated << ":\n"
                              << tail_of(result);
  EXPECT_TRUE(files_equal(weights, validated));

  // the dxxwjz1 file converts into itself
  result = run(executable,
               conversion_args("dxxwjz1", dxxwjz1, "dxxwjz1", again, cfg), log);

  ASSERT_EQ(result.status, 0) << "the conversion into " << again << ":\n"
                              << tail_of(result);
  EXPECT_TRUE(files_equal(dxxwjz1, again));
}

void check_variant(const WeightsVariant& variant,
                   const WeightsWorkspace& workspace)
{
  const std::string cfg = workspace.write_cfg(variant.spec);
  const Layout memory = variant.spec.layout();
  const Pattern pattern{variant.seed};
  const Pattern before{~variant.seed};

  // the file darknet saves of the pattern filled network
  const std::string saved = workspace.file(variant.name + ".weights");

  {
    DarknetNetwork darknet{cfg, variant.mode};

    ASSERT_TRUE(matches_spec(darknet.net(), variant.spec, variant.mode));
    ASSERT_TRUE(darknet.fill(memory, pattern));

    *darknet.net().cur_iteration = variant.iteration;

    ASSERT_TRUE(OrigWeightsWriter{}.save(darknet.net(), saved));
  }

  // the file of the variant header made for the load: no arrays of the
  // dontload layers and no batch normalization arrays of the dontloadscales
  // ones, the transposed connected weights of the old versions
  WeightsHeader header = variant.header;
  header.seen = header.stored_seen();

  LayoutOptions loading;
  loading.loading = true;
  loading.transposed = header.transposed();

  const Layout made_layout = variant.spec.layout(loading);
  const std::string made = workspace.file(variant.name + ".made.weights");

  std::vector<std::tuple<std::string, Layout, bool>> files{
      {saved, memory, false}};

  if (count_floats(memory) <= HUGE_FLOATS) {
    ASSERT_TRUE(write_weights_file(made, header, made_layout, pattern));

    files.emplace_back(made, made_layout, header.transposed());
  }

  for (const auto& [weights, layout, transposed] : files) {
    SCOPED_TRACE(weights);

    check_back_and_forth(ITEST_converter_provider::CONVERTER, weights, cfg,
                         workspace);

    if (::testing::Test::HasFatalFailure()) {
      return;
    }

    // the dxxwjz1 file of the binary loads the network the weights file
    // does, unless the original loader reads the arrays of the file in place
    // of the arrays it skips
    LayoutOptions reading;
    reading.loading = true;
    reading.transposed = transposed;

    if (!loads_every_array(layout, variant.spec.layout(reading))) {
      continue;
    }

    DarknetNetwork from_weights{cfg, variant.mode};
    DarknetNetwork from_dxxwjz1{cfg, variant.mode};

    ASSERT_TRUE(from_weights.fill(memory, before));
    ASSERT_TRUE(from_dxxwjz1.fill(memory, before));

    ASSERT_TRUE(OrigWeightsReader{}.load(from_weights.net(), weights));
    ASSERT_TRUE(Dxxwjz1Reader{}.load(from_dxxwjz1.net(), weights + ".dxxwjz1"));

    EXPECT_EQ(*from_dxxwjz1.net().seen, *from_weights.net().seen);

    same_arrays(from_weights, from_dxxwjz1, memory);
  }
}

bool write_random_weights_file(const std::string& path,
                               const WeightsHeader& header,
                               const uint64_t floats, const uint64_t seed)
{
  std::ofstream file{path, std::ios::binary};
  const int32_t version[3] = {header.major, header.minor, header.revision};
  const uint64_t seen64 = header.seen;
  const auto seen32 = static_cast<uint32_t>(header.seen);

  file.write(reinterpret_cast<const char*>(version), sizeof(version));

  if (header.seen64()) {
    file.write(reinterpret_cast<const char*>(&seen64), sizeof(seen64));
  } else {
    file.write(reinterpret_cast<const char*>(&seen32), sizeof(seen32));
  }

  std::mt19937 random{static_cast<std::mt19937::result_type>(seed)};

  for (uint64_t index = 0U; index < floats; ++index) {
    const auto bits = static_cast<uint32_t>(random());

    file.write(reinterpret_cast<const char*>(&bits), sizeof(bits));
  }

  return static_cast<bool>(file);
}

void check_random_round_trips(const WeightsVariant& variant,
                              const WeightsWorkspace& workspace)
{
  const std::string cfg = workspace.write_cfg(variant.spec);
  const Layout memory = variant.spec.layout();
  const std::string weights = workspace.file(variant.name + ".weights");
  const std::string log = workspace.file("converter.log");
  const std::string converter = ITEST_converter_provider::CONVERTER;

  WeightsHeader header = variant.header;
  header.seen = header.stored_seen();

  ASSERT_TRUE(write_random_weights_file(weights, header, count_floats(memory),
                                        variant.seed));

  const auto convert = [&](const std::string& from, const std::string& src,
                           const std::string& to, const std::string& dst,
                           const std::string& with_cfg) {
    const RunResult result =
        run(converter, conversion_args(from, src, to, dst, with_cfg), log);

    EXPECT_EQ(result.status, 0) << "the conversion into " << dst << ":\n"
                                << tail_of(result);
  };

  const std::string dxxwjz2 = weights + ".dxxwjz2";
  const std::string dxxwjz1 = weights + ".dxxwjz1";
  const std::string through = weights + ".through.dxxwjz2";
  const std::string again = weights + ".again.dxxwjz2";
  const std::string back = weights + ".back.weights";
  const std::string around = weights + ".around.weights";

  // the dxxwjz2 file keeps the network, the conversions of it need no cfg
  convert("weights", weights, "dxxwjz2", dxxwjz2, cfg);
  convert("dxxwjz2", dxxwjz2, "weights", back, "");
  convert("dxxwjz2", dxxwjz2, "dxxwjz2", again, "");
  convert("dxxwjz2", dxxwjz2, "dxxwjz1", dxxwjz1, "");
  convert("dxxwjz1", dxxwjz1, "dxxwjz2", through, cfg);
  convert("dxxwjz2", through, "weights", around, cfg);

  if (::testing::Test::HasFailure()) {
    return;
  }

  EXPECT_TRUE(is_gzip(dxxwjz2));
  EXPECT_TRUE(files_equal(weights, back));
  EXPECT_TRUE(files_equal(dxxwjz2, again));
  EXPECT_TRUE(files_equal(weights, around));

  // the dxxwjz2 file loads the network the weights file does, unless the
  // original loader reads the arrays of the file in place of the arrays it
  // skips
  LayoutOptions reading;
  reading.loading = true;
  reading.transposed = header.transposed();

  if (!loads_every_array(memory, variant.spec.layout(reading))) {
    return;
  }

  const Pattern before{~variant.seed};
  DarknetNetwork from_weights{cfg, variant.mode};
  DarknetNetwork from_dxxwjz2{cfg, variant.mode};

  ASSERT_TRUE(from_weights.fill(memory, before));
  ASSERT_TRUE(from_dxxwjz2.fill(memory, before));

  ASSERT_TRUE(OrigWeightsReader{}.load(from_weights.net(), weights));
  ASSERT_TRUE(Dxxwjz2Reader{}.load(from_dxxwjz2.net(), dxxwjz2));

  EXPECT_EQ(*from_dxxwjz2.net().seen, *from_weights.net().seen);

  same_arrays(from_weights, from_dxxwjz2, memory);
}

}  // namespace converter_test
