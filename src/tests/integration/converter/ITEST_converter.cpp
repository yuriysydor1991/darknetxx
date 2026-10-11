#include <gtest/gtest.h>
#include <zlib.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "ConverterRuns.h"
#include "ITEST_converter_provider.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Reader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h"
#include "src/log/default-logger/DefaultLogger.h"

using namespace testing;
using namespace converter_test;

using darknet_adaptor::loaders::OrigWeightsReader;
using darknet_adaptor::loaders::OrigWeightsWriter;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Reader;

namespace
{

/// @brief Keeps the info messages of the darknet parser (every layer of the
/// thousands of the generated networks) out of the tests output.
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

std::string reference(const std::string& name)
{
  return (std::filesystem::path{ITEST_converter_provider::REFERENCE_DIR} / name)
      .string();
}

/// @brief The text of the gzip compressed file, read by the zlib alone.
std::string gunzipped(const std::string& path)
{
  std::string text;
  gzFile file = gzopen(path.c_str(), "rb");

  if (file == nullptr) {
    return text;
  }

  char chunk[4096];
  int count = 0;

  while ((count = gzread(file, chunk, sizeof(chunk))) > 0) {
    text.append(chunk, static_cast<size_t>(count));
  }

  gzclose(file);

  return text;
}

}  // namespace

class ITEST_converter : public Test
{
 public:
  WeightsWorkspace workspace{ITEST_converter_provider::GENERATED_DIR};

  RunResult converter(const std::vector<std::string>& args)
  {
    return run(ITEST_converter_provider::CONVERTER, args,
               workspace.file("converter.log"));
  }
};

TEST_F(ITEST_converter, reference_weights_convert_back_and_forth)
{
  for (const std::string& name : {std::string{"1"}, std::string{"2"}}) {
    SCOPED_TRACE(name);

    const std::string cfg = reference(name + ".cfg");
    const std::string weights = workspace.file(name + ".weights");

    std::filesystem::copy_file(reference(name + ".weights"), weights);

    check_back_and_forth(ITEST_converter_provider::CONVERTER, weights, cfg,
                         workspace);

    // the dxxwjz1 file loads the network the weights file does
    DarknetNetwork from_weights{cfg, ParseMode::Inference};
    DarknetNetwork from_dxxwjz1{cfg, ParseMode::Inference};

    ASSERT_TRUE(OrigWeightsReader{}.load(from_weights.net(), weights));
    ASSERT_TRUE(Dxxwjz1Reader{}.load(from_dxxwjz1.net(), weights + ".dxxwjz1"));

    const std::string first = workspace.file(name + ".from_weights");
    const std::string second = workspace.file(name + ".from_dxxwjz1");

    ASSERT_TRUE(OrigWeightsWriter{}.save(from_weights.net(), first));
    ASSERT_TRUE(OrigWeightsWriter{}.save(from_dxxwjz1.net(), second));
    EXPECT_TRUE(files_equal(first, second));
  }
}

TEST_F(ITEST_converter, main_binary_converts_the_same_way)
{
  const std::string cfg = reference("2.cfg");
  const std::string weights = workspace.file("2.weights");

  std::filesystem::copy_file(reference("2.weights"), weights);

  check_back_and_forth(ITEST_converter_provider::DARKNETXX, weights, cfg,
                       workspace);

  const std::string by_converter = workspace.file("by_converter.dxxwjz1");

  ASSERT_EQ(converter(conversion_args("weights", weights, "dxxwjz1",
                                      by_converter, cfg))
                .status,
            0);

  // the dxxwjz1 file of either binary is one and the same file
  EXPECT_TRUE(files_equal(by_converter, weights + ".dxxwjz1"));
}

TEST_F(ITEST_converter, dxxwjz1_file_is_the_gzip_compressed_json_document)
{
  const std::string dxxwjz1 = workspace.file("1.dxxwjz1");

  ASSERT_EQ(converter(conversion_args("weights", reference("1.weights"),
                                      "dxxwjz1", dxxwjz1, reference("1.cfg")))
                .status,
            0);

  ASSERT_TRUE(is_gzip(dxxwjz1));

  const std::string text = gunzipped(dxxwjz1);

  EXPECT_EQ(text.rfind("{\n  \"format\": \"dxxwjz1\",\n  \"version\": 1,", 0U),
            0U)
      << text.substr(0U, 200U);
  EXPECT_NE(text.find("\"header\": {\"major\": 0, \"minor\": 2, \"revision\": "
                      "5, \"seen\": "),
            std::string::npos);
  EXPECT_NE(text.find("{\"index\": 0, \"type\": \"convolutional\", "
                      "\"arrays\": {"),
            std::string::npos);
}

TEST_F(ITEST_converter, dxxwjz2_file_keeps_the_network_description)
{
  const std::string dxxwjz2 = workspace.file("1.dxxwjz2");
  const std::string back = workspace.file("1.weights");

  std::vector<std::string> args =
      conversion_args("weights", reference("1.weights"), "dxxwjz2", dxxwjz2,
                      reference("1.cfg"));

  args.insert(args.end(), {"--convert-names", reference("1.names")});

  ASSERT_EQ(converter(args).status, 0);
  ASSERT_TRUE(is_gzip(dxxwjz2));

  const std::string text = gunzipped(dxxwjz2);

  EXPECT_EQ(text.rfind("{\n  \"format\": \"dxxwjz2\",\n  \"version\": 2,", 0U),
            0U)
      << text.substr(0U, 200U);
  EXPECT_NE(text.find("\"network\": {\"cfg\": \"[net]"), std::string::npos)
      << text.substr(0U, 400U);
  EXPECT_NE(text.find("{\"index\": 10, \"name\": "), std::string::npos);
  EXPECT_NE(text.find("\"training\": {\"seconds\": 0,"), std::string::npos);

  // the network kept, the conversion back needs no cfg
  ASSERT_EQ(converter(conversion_args("dxxwjz2", dxxwjz2, "weights", back, ""))
                .status,
            0);
  EXPECT_TRUE(files_equal(reference("1.weights"), back));
}

TEST_F(ITEST_converter, help_lists_the_conversion_alone)
{
  const RunResult result = converter({"--help"});

  EXPECT_EQ(result.status, 0);

  const std::string output = result.output();

  EXPECT_NE(
      output.find(std::filesystem::path{ITEST_converter_provider::CONVERTER}
                      .filename()
                      .string() +
                  " [OPTIONS]"),
      std::string::npos)
      << output;

  for (const std::string& part :
       {std::string{"--convert-from"}, std::string{"--convert-src"},
        std::string{"--convert-to"}, std::string{"--convert-dst"},
        std::string{"--convert-cfg"}, std::string{"--convert-names"},
        std::string{"weights"}, std::string{"dxxwjz1"},
        std::string{"dxxwjz2"}}) {
    EXPECT_NE(output.find(part), std::string::npos) << part;
  }

  // the converter only binary runs no training nor detection
  EXPECT_EQ(output.find("--train-cfg"), std::string::npos);
  EXPECT_EQ(output.find("--detect-cfg"), std::string::npos);
}

TEST_F(ITEST_converter, main_binary_help_lists_the_conversion_too)
{
  const RunResult result = run(ITEST_converter_provider::DARKNETXX, {"--help"},
                               workspace.file("darknetxx.log"));

  EXPECT_EQ(result.status, 0);

  const std::string output = result.output();

  for (const std::string& part :
       {std::string{"--convert-from"}, std::string{"--convert-dst"},
        std::string{"--train-cfg"}, std::string{"--detect-cfg"}}) {
    EXPECT_NE(output.find(part), std::string::npos) << part;
  }
}

TEST_F(ITEST_converter, version_names_the_converter)
{
  const RunResult result = converter({"--version"});

  EXPECT_EQ(result.status, 0);
  EXPECT_NE(result.output().find(
                std::filesystem::path{ITEST_converter_provider::CONVERTER}
                    .filename()
                    .string() +
                " version "),
            std::string::npos)
      << result.output();
}

TEST_F(ITEST_converter, empty_command_line_is_the_failed_help)
{
  const RunResult result = converter({});

  EXPECT_NE(result.status, 0);
  EXPECT_NE(result.output().find("--convert-from"), std::string::npos);
}

TEST_F(ITEST_converter, failed_conversions_write_nothing)
{
  const std::string cfg = reference("1.cfg");
  const std::string weights = reference("1.weights");
  const std::string dxxwjz1 = workspace.file("1.dxxwjz1");
  const std::string dst = workspace.file("out/never.file");

  ASSERT_EQ(
      converter(conversion_args("weights", weights, "dxxwjz1", dxxwjz1, cfg))
          .status,
      0);

  // the cut dxxwjz1 file
  const std::string cut = workspace.file("cut.dxxwjz1");

  std::filesystem::copy_file(dxxwjz1, cut);
  std::filesystem::resize_file(cut, std::filesystem::file_size(cut) - 3U);

  const std::vector<std::vector<std::string>> failures{
      // the unknown formats
      conversion_args("darknet", weights, "dxxwjz1", dst, cfg),
      conversion_args("weights", weights, "json", dst, cfg),
      // the original weights file without its network
      conversion_args("weights", weights, "dxxwjz1", dst, ""),
      // the missing files
      conversion_args("weights", workspace.file("missing.weights"), "dxxwjz1",
                      dst, cfg),
      conversion_args("weights", weights, "dxxwjz1", dst,
                      workspace.file("missing.cfg")),
      // the dxxwjz1 file of another network
      conversion_args("dxxwjz1", dxxwjz1, "weights", dst, reference("2.cfg")),
      // the corrupted dxxwjz1 file
      conversion_args("dxxwjz1", cut, "weights", dst, ""),
      // the files of the other formats
      conversion_args("dxxwjz1", weights, "weights", dst, ""),
      conversion_args("weights", dxxwjz1, "dxxwjz1", dst, cfg),
      conversion_args("dxxwjz2", dxxwjz1, "weights", dst, ""),
      // the dxxwjz2 file of no network description
      conversion_args("dxxwjz1", dxxwjz1, "dxxwjz2", dst, ""),
      // the missing class names file
      {"--convert-from", "weights", "--convert-src", weights, "--convert-to",
       "dxxwjz2", "--convert-dst", dst, "--convert-cfg", cfg, "--convert-names",
       workspace.file("missing.names")},
      // the part of the conversion flags
      {"--convert-from", "weights", "--convert-src", weights, "--convert-to",
       "dxxwjz1"},
      // the conversion of the training
      {"--train-cfg", cfg, "--convert-from", "weights", "--convert-src",
       weights, "--convert-to", "dxxwjz1", "--convert-dst", dst,
       "--convert-cfg", cfg}};

  for (const std::vector<std::string>& args : failures) {
    std::string command;

    for (const std::string& arg : args) {
      command += " " + arg;
    }

    SCOPED_TRACE(command);

    const RunResult result = converter(args);

    EXPECT_NE(result.status, 0) << result.output();
    EXPECT_FALSE(std::filesystem::exists(dst));
  }

  // nothing but the files of the test is left behind
  std::vector<std::string> files;

  for (const auto& entry :
       std::filesystem::directory_iterator{workspace.dir()}) {
    files.push_back(entry.path().filename().string());
  }

  std::sort(files.begin(), files.end());

  EXPECT_EQ(files, (std::vector<std::string>{"1.dxxwjz1", "converter.log",
                                             "cut.dxxwjz1"}));
}
