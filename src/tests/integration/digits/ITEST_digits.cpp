#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
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

    std::vector<detected_object> objects;
    std::ifstream json_file{json};
    std::string line;

    while (std::getline(json_file, line)) {
      std::smatch class_id;
      std::smatch confidence;

      if (std::regex_search(line, class_id, class_id_regex) &&
          std::regex_search(line, confidence, confidence_regex)) {
        objects.push_back({std::stoi(class_id[1]), std::stod(confidence[1])});
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
