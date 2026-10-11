#include <gtest/gtest.h>
#include <zlib.h>

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <nlohmann/json.hpp>
#include <regex>
#include <string>
#include <vector>

#include "ITEST_digits_provider.h"

using namespace testing;

/// Checks the network the ITEST_digits_training test has trained: the
/// darknetxx detection run on every glyph of the training data set is to
/// recognize the digit of that glyph.
class ITEST_digits : public Test, virtual public ITEST_digits_provider
{
 public:
  struct detected_object
  {
    int class_id{-1};
    double confidence{0.};
    std::string name;
  };

  ~ITEST_digits() = default;
  ITEST_digits() = default;

  /// Quotes the argument for the POSIX shell.
  static std::string quote(const std::string& arg)
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

  /// Reads the objects of the darknetxx detection JSON file, which holds
  /// every object on a line of its own.
  static std::vector<detected_object> read_objects(
      const std::filesystem::path& json)
  {
    static const std::regex class_id_regex{"\"class_id\":\\s*(-?[0-9]+)"};
    static const std::regex confidence_regex{
        "\"confidence\":\\s*([-+.0-9eE]+)"};
    static const std::regex name_regex{"\"name\":\\s*\"([^\"]*)\""};

    std::vector<detected_object> objects;
    std::ifstream json_file{json};
    std::string line;

    while (std::getline(json_file, line)) {
      std::smatch class_id;
      std::smatch confidence;

      std::smatch name;

      if (std::regex_search(line, class_id, class_id_regex) &&
          std::regex_search(line, confidence, confidence_regex)) {
        std::regex_search(line, name, name_regex);
        objects.push_back({std::stoi(class_id[1]), std::stod(confidence[1]),
                           name.size() > 1 ? name[1].str() : std::string{}});
      }
    }

    return objects;
  }

  /// The darknetxx saves the predictions image into the current directory, so
  /// every digit gets a directory of its own for the parallel ctest runs.
  static std::filesystem::path work_dir_of(const int digit)
  {
    return std::filesystem::path{DETECTIONS_DIR} / std::to_string(digit);
  }

  /// Runs the darknetxx detection on a single glyph image and expects the
  /// most confident object detected to be the digit of the glyph.
  static void expect_glyph_recognized(const int digit, const int size)
  {
    const std::string name =
        "digit_" + std::to_string(digit) + "_" + std::to_string(size);
    const auto image = std::filesystem::path{DATA_DIR} / (name + ".png");
    const auto work_dir = work_dir_of(digit);
    const auto json = work_dir / (name + ".json");
    const auto log = work_dir / (name + ".log");

    SCOPED_TRACE(image.string());

    const std::string command =
        "cd " + quote(work_dir.string()) + " && " + quote(DARKNETXX) +
        " --detect-cfg " + quote(CFG) + " --weights " + quote(WEIGHTS) +
        " --image " + quote(image.string()) + " --dont-show --out " +
        quote(json.string()) + " > " + quote(log.string()) + " 2>&1";

    ASSERT_EQ(std::system(command.c_str()), 0)
        << "the darknetxx detection failed, see the " << log << " file";

    const auto objects = read_objects(json);

    ASSERT_FALSE(objects.empty()) << "no object detected, see the " << json;

    const auto recognized = std::max_element(
        objects.begin(), objects.end(), [](const auto& a, const auto& b) {
          return a.confidence < b.confidence;
        });

    EXPECT_EQ(recognized->class_id, digit)
        << "recognized with the " << std::to_string(recognized->confidence)
        << " confidence, see the " << json;
  }

  static void expect_recognized(const int digit)
  {
    // nothing of a previous run may pass for the results of this one, and the
    // darknetxx appends its log to the file of the working directory
    std::filesystem::remove_all(work_dir_of(digit));
    std::filesystem::create_directories(work_dir_of(digit));

    for (int size = 0; size < GLYPH_SIZES; ++size) {
      expect_glyph_recognized(digit, size);
    }
  }
};

TEST_F(ITEST_digits, recognizes_digit_0) { expect_recognized(0); }

TEST_F(ITEST_digits, recognizes_digit_1) { expect_recognized(1); }

TEST_F(ITEST_digits, recognizes_digit_2) { expect_recognized(2); }

TEST_F(ITEST_digits, recognizes_digit_3) { expect_recognized(3); }

TEST_F(ITEST_digits, recognizes_digit_4) { expect_recognized(4); }

TEST_F(ITEST_digits, recognizes_digit_5) { expect_recognized(5); }

TEST_F(ITEST_digits, recognizes_digit_6) { expect_recognized(6); }

TEST_F(ITEST_digits, recognizes_digit_7) { expect_recognized(7); }

TEST_F(ITEST_digits, recognizes_digit_8) { expect_recognized(8); }

TEST_F(ITEST_digits, recognizes_digit_9) { expect_recognized(9); }

/// Checks the dxxwjz3 files of the trainings: the one the ITEST_digits_training
/// test saved, the one of the training resumed of it and the one of the network
/// of no weights trained, and the detection with the dxxwjz3 file alone. The
/// files are the gzip compressed JSON documents, read and written here by the
/// zlib and the nlohmann json alone.
class ITEST_digits_dxxwjz3 : public ITEST_digits
{
 public:
  using json = nlohmann::json;

  /// The fresh working directory of the further training or detection.
  static std::filesystem::path work_dir(const std::string& name)
  {
    const auto dir = std::filesystem::path{RESUMED_DIR} / name;

    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir / "backup");

    return dir;
  }

  /// The darknetxx run of the arguments in the directory, the output into
  /// it's log.
  static int run(const std::filesystem::path& dir, const std::string& args)
  {
    const std::string command = "cd " + quote(dir.string()) + " && " +
                                quote(DARKNETXX) + " " + args + " > " +
                                quote((dir / "output.log").string()) + " 2>&1";

    return std::system(command.c_str());
  }

  static std::string text_of(const std::filesystem::path& path)
  {
    std::ifstream file{path};

    return std::string{std::istreambuf_iterator<char>{file},
                       std::istreambuf_iterator<char>{}};
  }

  /// The cfg text of the training network of the key values replaced.
  static std::string cfg_with(
      const std::vector<std::pair<std::string, std::string>>& values)
  {
    std::string text = text_of(CFG);

    for (const auto& [key, value] : values) {
      text = std::regex_replace(text, std::regex{"\n" + key + "=[^\n]*"},
                                "\n" + key + "=" + value);
    }

    return text;
  }

  /// The document of the dxxwjz3 file.
  static json document_of(const std::filesystem::path& path)
  {
    std::string text;
    gzFile file = gzopen(path.string().c_str(), "rb");
    char chunk[65536];
    int count = 0;

    while (file != nullptr &&
           (count = gzread(file, chunk, sizeof(chunk))) > 0) {
      text.append(chunk, static_cast<size_t>(count));
    }

    if (file != nullptr) {
      gzclose(file);
    }

    return json::parse(text, nullptr, false);
  }

  static std::vector<std::string> classes_of(const json& document)
  {
    return document["network"]["classes"].get<std::vector<std::string>>();
  }

  static std::vector<std::string> charts_of(const std::filesystem::path& dir)
  {
    std::vector<std::string> charts;

    for (const auto& entry : std::filesystem::directory_iterator{dir}) {
      const std::string name = entry.path().filename().string();

      if (name.rfind("chart", 0) == 0) {
        charts.push_back(name);
      }
    }

    return charts;
  }

  inline static const std::vector<std::string> WORDS{
      "zero", "one", "two",   "three", "four",
      "five", "six", "seven", "eight", "nine"};
};

TEST_F(ITEST_digits_dxxwjz3, trained_file_keeps_the_network_and_the_training)
{
  const json document = document_of(WEIGHTS);

  ASSERT_TRUE(document.is_object());

  const json& training = document["training"];

  EXPECT_EQ(document["format"], "dxxwjz3");
  EXPECT_EQ(document["network"]["net"]["max_batches"], 400);
  EXPECT_EQ(document["network"]["net"]["steps"], json::parse("[300, 350]"));
  const json& layers = document["network"]["layers"];

  ASSERT_EQ(layers.size(), 9U);
  EXPECT_EQ(layers[0]["type"], "convolutional");
  EXPECT_EQ(
      layers[0]["options"],
      json::parse(
          R"({"batch_normalize": 1, "filters": 8, "size": 3, "stride": 1, "pad": 1, "activation": "leaky"})"));
  // the weights of the layer in it's arrays
  EXPECT_EQ(layers[0]["arrays"]["biases"].size(), 8U);
  EXPECT_FALSE(document.contains("layers"));
  EXPECT_EQ(classes_of(document),
            (std::vector<std::string>{"0", "1", "2", "3", "4", "5", "6", "7",
                                      "8", "9"}));
  // the losses of the iterations of their indexes
  EXPECT_FALSE(training.contains("iteration"));
  EXPECT_EQ(training["loss"].size(), 400U);
  EXPECT_EQ(training["avg_loss"].size(), 400U);
  EXPECT_GE(training["seconds"].get<double>(), 1.0);
  EXPECT_LT(training["avg_loss"].back().get<double>(),
            training["avg_loss"].front().get<double>());
}

TEST_F(ITEST_digits_dxxwjz3, chart_is_named_after_the_training_start)
{
  const std::vector<std::string> charts = charts_of(TRAINING_DIR);

  ASSERT_EQ(charts.size(), 1U);
  EXPECT_TRUE(std::regex_match(
      charts.front(),
      std::regex{R"(chart_digits_\d{4}-\d{2}-\d{2}--\d{2}-\d{2}-\d{2}\.png)"}))
      << charts.front();
}

TEST_F(ITEST_digits_dxxwjz3, file_alone_detects_with_its_class_names)
{
  const auto dir = work_dir("named");
  const auto words = dir / "words.names";
  const auto named = dir / "named.dxxwjz3";

  {
    std::ofstream file{words};

    for (const std::string& word : WORDS) {
      file << word << "\n";
    }
  }

  ASSERT_EQ(run(dir, "--convert-from dxxwjz3 --convert-src " + quote(WEIGHTS) +
                         " --convert-to dxxwjz3 --convert-dst " +
                         quote(named.string()) + " --convert-names " +
                         quote(words.string())),
            0);

  for (int digit = 0; digit < 10; ++digit) {
    const auto glyph = std::filesystem::path{DATA_DIR} /
                       ("digit_" + std::to_string(digit) + "_" +
                        std::to_string(GLYPH_SIZES / 2) + ".png");
    const auto detections = dir / (std::to_string(digit) + ".json");

    SCOPED_TRACE(glyph.string());

    ASSERT_EQ(run(dir, "--weights " + quote(named.string()) + " --image " +
                           quote(glyph.string()) + " --dont-show --out " +
                           quote(detections.string())),
              0);

    const auto objects = read_objects(detections);

    ASSERT_FALSE(objects.empty());

    const auto recognized = std::max_element(
        objects.begin(), objects.end(), [](const auto& a, const auto& b) {
          return a.confidence < b.confidence;
        });

    EXPECT_EQ(recognized->name, WORDS[static_cast<size_t>(digit)]);
  }
}

TEST_F(ITEST_digits_dxxwjz3, resumed_training_adds_its_record)
{
  const auto dir = work_dir("resumed");
  const std::string cfg = cfg_with({{"max_batches", "420\nsaveweights=20"},
                                    {"backup", (dir / "backup").string()}});

  std::filesystem::copy_file(WEIGHTS, dir / "start.dxxwjz3");
  std::ofstream{dir / "more.cfg"} << cfg;

  ASSERT_EQ(
      run(dir, "--train-cfg more.cfg --weights start.dxxwjz3 --dont-show"), 0)
      << "see the " << dir / "output.log";

  const json first = document_of(WEIGHTS);
  const json resumed = document_of(dir / "backup" / "more_final.dxxwjz3");
  const json& losses = resumed["training"]["loss"];

  ASSERT_TRUE(resumed.is_object());

  EXPECT_EQ(resumed["network"]["net"]["max_batches"], 420);
  EXPECT_EQ(resumed["network"]["net"]["backup"], (dir / "backup").string());
  EXPECT_EQ(classes_of(resumed), classes_of(first));
  EXPECT_EQ(losses.size(), 420U);
  EXPECT_EQ(json(std::vector<json>(losses.begin(), losses.begin() + 400)),
            first["training"]["loss"]);
  EXPECT_GE(resumed["training"]["seconds"].get<double>(),
            first["training"]["seconds"].get<double>());

  // the numbered backup names the average loss of the iteration
  char numbered[64];

  std::snprintf(numbered, sizeof(numbered), "more_420_%f.dxxwjz3",
                resumed["training"]["avg_loss"][419].get<double>());
  EXPECT_TRUE(std::filesystem::exists(dir / "backup" / numbered)) << numbered;

  // the average loss goes on of the record
  const double average = resumed["training"]["avg_loss"][400].get<double>();

  EXPECT_NEAR(average,
              0.9 * first["training"]["avg_loss"][399].get<double>() +
                  0.1 * losses[400].get<double>(),
              1e-4 * average);

  // the chart of the whole record, saved at the end too
  const std::vector<std::string> charts = charts_of(dir);

  ASSERT_EQ(charts.size(), 1U);
  EXPECT_GE(
      std::filesystem::last_write_time(dir / charts.front()),
      std::filesystem::last_write_time(dir / "backup" / "more_final.dxxwjz3"));
}

TEST_F(ITEST_digits_dxxwjz3, map_calculations_go_into_the_record)
{
  const auto dir = work_dir("mapped");

  // the class names of the file alone, the names= one of the cfg is missing
  std::filesystem::copy_file(WEIGHTS, dir / "start.dxxwjz3");
  std::ofstream{dir / "mapped.cfg"} << cfg_with(
      {{"max_batches", "420"},
       {"backup", (dir / "backup").string()},
       {"names", (dir / "missing.names").string()},
       {"train", std::string{TRAIN_LIST} + "\nvalid=" + TRAIN_LIST}});

  ASSERT_EQ(
      run(dir,
          "--train-cfg mapped.cfg --weights start.dxxwjz3 --dont-show --map"),
      0)
      << "see the " << dir / "output.log";

  // the structure alone: the mAP values of the tiny network vary
  for (const std::string& file : {"mapped_final", "mapped_best"}) {
    const json document = document_of(dir / "backup" / (file + ".dxxwjz3"));
    const json& training = document["training"];

    SCOPED_TRACE(file);

    ASSERT_TRUE(training.contains("map_iteration"));
    EXPECT_EQ(training["map"].size(), training["map_iteration"].size());

    for (const json& map : training["map"]) {
      EXPECT_GE(map.get<double>(), 0.0);
      EXPECT_LE(map.get<double>(), 1.0);
    }
  }

  EXPECT_EQ(document_of(dir / "backup" /
                        "mapped_final.dxxwjz3")["training"]["map_iteration"]
                .back(),
            420);
}

TEST_F(ITEST_digits_dxxwjz3, chart_of_the_file_is_drawn)
{
  const auto dir = work_dir("chart");
  const auto png = dir / "drawn.png";

  ASSERT_EQ(run(dir, "--weights " + quote(WEIGHTS) + " --draw-chart " +
                         quote(png.string())),
            0)
      << "see the " << dir / "output.log";

  // the PNG signature and the IHDR chunk of the 1000x1000 image
  const std::string text = text_of(png);
  const auto at = [&text](const size_t offset) {
    return (static_cast<uint32_t>(static_cast<uint8_t>(text[offset])) << 24U) |
           (static_cast<uint32_t>(static_cast<uint8_t>(text[offset + 1]))
            << 16U) |
           (static_cast<uint32_t>(static_cast<uint8_t>(text[offset + 2]))
            << 8U) |
           static_cast<uint32_t>(static_cast<uint8_t>(text[offset + 3]));
  };

  ASSERT_GE(text.size(), 24U);
  EXPECT_EQ(text.substr(0U, 8U), std::string{"\x89PNG\r\n\x1a\n"});
  EXPECT_EQ(at(16U), 1000U);
  EXPECT_EQ(at(20U), 1000U);

  // the cfg file keeps no training record
  EXPECT_NE(run(dir, "--weights " + quote(CFG) + " --draw-chart never.png"), 0);
  EXPECT_FALSE(std::filesystem::exists(dir / "never.png"));
}

TEST_F(ITEST_digits_dxxwjz3, network_of_no_weights_is_trained)
{
  const auto dir = work_dir("project");

  std::ofstream{dir / "project.cfg"} << cfg_with(
      {{"max_batches", "12"}, {"backup", (dir / "backup").string()}});

  {
    std::ofstream file{dir / "words.names"};

    for (const std::string& word : WORDS) {
      file << word << "\n";
    }
  }

  // the network to train the converter makes of the cfg
  ASSERT_EQ(run(dir,
                "--convert-to dxxwjz3 --convert-dst project.dxxwjz3 "
                "--convert-cfg project.cfg --convert-names words.names"),
            0)
      << "see the " << dir / "output.log";

  const json made = document_of(dir / "project.dxxwjz3");

  EXPECT_FALSE(made.contains("header"));
  EXPECT_FALSE(made["network"]["layers"][0].contains("arrays"));

  // the file alone keeps the network and it's class names
  std::filesystem::remove(dir / "project.cfg");
  std::filesystem::remove(dir / "words.names");

  ASSERT_EQ(run(dir, "--weights project.dxxwjz3 --dont-show"), 0)
      << "see the " << dir / "output.log";

  const json trained = document_of(dir / "backup" / "project_final.dxxwjz3");

  ASSERT_TRUE(trained.is_object());

  EXPECT_EQ(classes_of(trained), WORDS);
  EXPECT_EQ(trained["training"]["loss"].size(), 12U);
  EXPECT_TRUE(trained["network"]["layers"][0].contains("arrays"));
}
