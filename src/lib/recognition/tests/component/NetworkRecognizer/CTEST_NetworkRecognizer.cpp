#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <opencv2/imgcodecs.hpp>
#include <string>
#include <vector>

#include "LibraryFacade.h"
#include "src/tests/component/bias-networks/BiasNetworks.h"

using darknetxx011::LibraryContext;
using darknetxx011::LibraryFacade;
using darknetxx011::Recognition;

namespace fs = std::filesystem;

using namespace bias_networks;

class CTEST_NetworkRecognizer : public testing::Test
{
 public:
  void SetUp() override
  {
    fs::remove_all(dir);
    fs::create_directories(dir);

    write_image(file("image.ppm"));
    std::ofstream{file("abc.names")} << "a\nb\nc\n";

    ctx.names = file("abc.names");
    ctx.image = file("image.ppm");
  }

  void TearDown() override { fs::remove_all(dir); }

  std::string file(const std::string& name) const
  {
    return (dir / name).string();
  }

  /// @brief The network of the context recognizes by.
  void network(const std::string& cfg, const std::vector<float>& biases,
               const std::size_t weights)
  {
    ctx.detect_cfg = file("net.cfg");
    ctx.latest_weights = file("net.weights");

    bias_networks::write(ctx.detect_cfg, ctx.latest_weights, cfg, biases,
                         weights);
  }

  /// @brief The weights file of the network converted into the format, which
  /// keeps the network and the names of the context, when it keeps any.
  std::string convert(const std::string& format)
  {
    auto conversion = LibraryFacade::create_library_context();

    conversion->convert_from = "weights";
    conversion->convert_src = file("net.weights");
    conversion->convert_to = format;
    conversion->convert_dst = file("net." + format);
    conversion->convert_cfg = ctx.detect_cfg;
    conversion->convert_names = ctx.names;

    EXPECT_TRUE(LibraryFacade::create_library(conversion)->libcall(conversion));

    return conversion->convert_dst;
  }

  /// @brief The text of the most probable object, the error for none.
  std::string first()
  {
    const Recognition recognition = recognizer->recognize(ctx);

    return recognition.objects.empty() ? recognition.summary_text()
                                       : recognition.objects.front().text();
  }

  fs::path dir{fs::path{CTEST_NetworkRecognizer_DATA_DIR} /
               testing::UnitTest::GetInstance()->current_test_info()->name()};
  LibraryContext ctx;
  darknetxx011::IRecognizerPtr recognizer{LibraryFacade::create_recognizer()};
};

TEST_F(CTEST_NetworkRecognizer, the_detector_boxes_the_objects)
{
  network(DETECTOR_CFG, DETECTOR_BIASES, DETECTOR_WEIGHTS);

  const Recognition recognition = recognizer->recognize(ctx);

  ASSERT_TRUE(recognition.succeeded()) << recognition.error;
  ASSERT_EQ(recognition.objects.size(), 1U);
  EXPECT_EQ(recognition.objects.front().text(),
            "b: 100.0% (left_x: 0 top_y: 0 width: 31 height: 23)");
  EXPECT_EQ(recognition.summary_text().rfind("1 object recognized in ", 0), 0U);

  // enlarged for the labels, the box over the image
  const cv::Mat png = cv::imdecode(recognition.png, cv::IMREAD_COLOR);

  ASSERT_EQ(png.size(), cv::Size(32 * 15, 24 * 15));
  EXPECT_NE(png.at<cv::Vec3b>(png.rows / 2, 0), cv::Vec3b(40, 80, 160));
  EXPECT_EQ(png.at<cv::Vec3b>(png.rows / 2, png.cols / 2),
            cv::Vec3b(40, 80, 160));
}

TEST_F(CTEST_NetworkRecognizer, the_classifier_classifies_the_whole_image)
{
  network(CLASSIFIER_CFG, CLASSIFIER_BIASES, CLASSIFIER_WEIGHTS);
  ctx.thresh = .005f;

  const Recognition recognition = recognizer->recognize(ctx);

  ASSERT_TRUE(recognition.succeeded()) << recognition.error;
  ASSERT_EQ(recognition.objects.size(), 3U);
  EXPECT_EQ(recognition.objects[0].text(), "c: 98.7%");
  EXPECT_EQ(recognition.objects[1].text(), "a: 0.7%");
  EXPECT_FALSE(recognition.objects[0].boxed());
  EXPECT_EQ(cv::imdecode(recognition.png, cv::IMREAD_COLOR).at<cv::Vec3b>(0, 0),
            cv::Vec3b(40, 80, 160));
}

TEST_F(CTEST_NetworkRecognizer, the_gray_network_takes_the_gray_image)
{
  std::string cfg = CLASSIFIER_CFG;

  network(cfg.replace(cfg.find("channels=3"), 10, "channels=1"),
          CLASSIFIER_BIASES, CLASSIFIER_WEIGHTS / 3);

  EXPECT_EQ(first(), "c: 98.7%");
}

TEST_F(CTEST_NetworkRecognizer, the_threshold_leaves_the_more_probable_ones)
{
  network(CLASSIFIER_CFG, CLASSIFIER_BIASES, CLASSIFIER_WEIGHTS);

  ctx.thresh = .99f;

  EXPECT_EQ(first().rfind("0 objects recognized in ", 0), 0U);
}

TEST_F(CTEST_NetworkRecognizer, every_weights_file_format_recognizes_the_same)
{
  network(DETECTOR_CFG, DETECTOR_BIASES, DETECTOR_WEIGHTS);

  const std::string expected = first();

  for (const std::string format : {"dxxwjz1", "dxxwjz2"}) {
    ctx.latest_weights = convert(format);

    EXPECT_EQ(first(), expected) << format;
  }

  // the dxxwjz2 file keeps the network and the names
  ctx.detect_cfg.clear();
  ctx.names.clear();

  EXPECT_EQ(first(), expected);
}

TEST_F(CTEST_NetworkRecognizer, the_classes_of_no_names_are_numbered)
{
  network(DETECTOR_CFG, DETECTOR_BIASES, DETECTOR_WEIGHTS);
  ctx.names.clear();

  EXPECT_EQ(first().rfind("class 1: 100.0%", 0), 0U);
}

TEST_F(CTEST_NetworkRecognizer, the_network_is_loaded_once_for_it_s_files)
{
  network(CLASSIFIER_CFG, CLASSIFIER_BIASES, CLASSIFIER_WEIGHTS);

  EXPECT_EQ(first(), "c: 98.7%");

  // the loaded network goes on recognizing
  network(CLASSIFIER_CFG, {5, 0, 0}, CLASSIFIER_WEIGHTS);

  EXPECT_EQ(first(), "c: 98.7%");

  // the other names file of the same names is another network
  fs::copy_file(file("abc.names"), file("other.names"));
  ctx.names = file("other.names");

  EXPECT_EQ(first(), "a: 98.7%");
}

TEST_F(CTEST_NetworkRecognizer, the_failures_are_told)
{
  network(DETECTOR_CFG, DETECTOR_BIASES, DETECTOR_WEIGHTS);

  ctx.detect_cfg.clear();

  EXPECT_EQ(first(), "The " + file("net.weights") +
                         " file keeps no network, give it's cfg file too");

  ctx.detect_cfg = file("net.cfg");
  ctx.image = file("abc.names");

  EXPECT_EQ(first(), "Couldn't read the image " + file("abc.names"));

  ctx.latest_weights = file("missing.weights");

  EXPECT_EQ(first().rfind("Couldn't load the network: ", 0), 0U);

  ctx.latest_weights.clear();

  EXPECT_EQ(first(), "No network weights file given");
}
