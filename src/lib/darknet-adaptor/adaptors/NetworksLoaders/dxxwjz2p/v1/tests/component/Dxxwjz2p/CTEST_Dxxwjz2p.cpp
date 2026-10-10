#include <gtest/gtest.h>
#include <zlib.h>

#include <filesystem>
#include <limits>
#include <memory>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pParser.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pWriter.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/lib/zlib/ZlibController.h"

using darknet_adaptor::loaders::NetworkDescription;
using darknet_adaptor::loaders::OrigDefaultLoader;
using darknet_adaptor::loaders::WeightsConversion;
using darknet_adaptor::loaders::WeightsConverter;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsFileFormats;
using darknet_adaptor::loaders::WeightsReport;
using darknet_adaptor::loaders::Yolo4Box;
using darknet_adaptor::loaders::Yolo4Image;
using darknet_adaptor::loaders::dxxwjz2p::v1::Dxxwjz2pParser;
using darknet_adaptor::loaders::dxxwjz2p::v1::Dxxwjz2pReader;
using darknet_adaptor::loaders::dxxwjz2p::v1::Dxxwjz2pWriter;

namespace fs = std::filesystem;

namespace
{

const std::string NETWORK_MEMBER =
    R"("network": {"cfg": "[net]\nwidth=8\n", "classes": [{"index": 0, "name": "cat"}, {"index": 1, "name": "dog"}]})";

const std::string PARK_GROUP = R"({"db": {"path": "data"}, "annotations": [
  {"rel_path": "park.jpg", "width": 640, "height": 480, "scale_factor": 1.5, "annotations": [
    {"name": "dog", "rect": {"x": 200, "y": 130, "width": 48, "height": 52}},
    {"name": "cat", "rect": {"x": 0, "y": 0, "width": 320, "height": 240}}]},
  {"rel_path": "street.png", "width": 320, "height": 200, "validation": true, "annotations": []}]})";

const std::string LAYERS_MEMBER =
    R"("header": {"major": 0, "minor": 2, "revision": 5, "seen": 192}, "layers": [{"index": 0, "type": "convolutional", "arrays": {"biases": [0.5, -1]}}])";

/// @brief The annotations member of the groups of the images.
std::string annotations(const std::string& groups)
{
  return R"("annotations": {"ImagesAnnotator": {"version": "0.12.0"}, "annotations": [)" +
         groups + "]}";
}

/// @brief The annotations of the image of the rects of the pixels.
std::string image_group(const std::string& image)
{
  return annotations(R"({"db": {"path": "data"}, "annotations": [)" + image +
                     "]}");
}

std::string document(const std::string& members)
{
  return R"({"format": "dxxwjz2p", "version": 1, )" + members + "}";
}

/// @brief The box of the rect of the pixels of the image of the size.
Yolo4Box box_of(const int id, const double x, const double y, const double w,
                const double h, const double width, const double height)
{
  return Yolo4Box{id, static_cast<float>((x + w / 2) / width),
                  static_cast<float>((y + h / 2) / height),
                  static_cast<float>(w / width),
                  static_cast<float>(h / height)};
}

/// @brief The parse of the text expected to fail with the error of the words.
::testing::AssertionResult rejects(const std::string& text,
                                   const std::vector<std::string>& words)
{
  std::istringstream json{text};
  WeightsDocument result;
  WeightsReport report;

  if (Dxxwjz2pParser::parse(json, result, report)) {
    return ::testing::AssertionFailure() << "the text is accepted: " << text;
  }

  for (const std::string& word : words) {
    if (report.first_error().find(word) == std::string::npos) {
      return ::testing::AssertionFailure()
             << "the error \"" << report.first_error() << "\" has no \"" << word
             << "\"";
    }
  }

  return ::testing::AssertionSuccess();
}

/// @brief The JSON document of the gzip compressed file.
nlohmann::json gunzipped(const std::string& path)
{
  std::string text;
  gzFile file = gzopen(path.c_str(), "rb");
  char chunk[4096];
  int count = 0;

  while (file != nullptr && (count = gzread(file, chunk, sizeof(chunk))) > 0) {
    text.append(chunk, static_cast<std::size_t>(count));
  }

  if (file != nullptr) {
    gzclose(file);
  }

  return nlohmann::json::parse(text, nullptr, false);
}

/// @brief The image of the path of the boxes of the class indexes.
Yolo4Image image_of(const fs::path& path, const std::vector<Yolo4Box>& boxes,
                    const bool validation = false)
{
  Yolo4Image result;
  result.path = path.string();
  result.width = 640;
  result.height = 480;
  result.boxes = boxes;
  result.validation = validation;
  return result;
}

/// @brief The network to train of the classes with the annotations.
WeightsDocument project_of(const std::vector<Yolo4Image>& images)
{
  WeightsDocument project;
  project.network = NetworkDescription{"[net]\nwidth=8\n", {"cat", "dog"}};
  project.annotations.images = images;
  return project;
}

}  // namespace

class CTEST_Dxxwjz2p : public testing::Test
{
 public:
  CTEST_Dxxwjz2p()
      : dir{fs::path{CTEST_Dxxwjz2p_DATA_DIR} /
            testing::UnitTest::GetInstance()->current_test_info()->name()}
  {
    fs::remove_all(dir);
    fs::create_directories(dir);
  }

  ~CTEST_Dxxwjz2p() override { fs::remove_all(dir); }

  /// @brief The gzip compressed file of the text.
  std::string file_of(const std::string& text)
  {
    const std::string path = (dir / "project.dxxwjz2p").string();
    std::unique_ptr<std::ostream> file =
        zlibi::ZlibController::create()->create_file_writer(path);

    *file << text;
    file.reset();

    return path;
  }

  fs::path dir;
};

TEST_F(CTEST_Dxxwjz2p, reads_the_annotations_of_the_images_annotator_project)
{
  WeightsDocument result;
  WeightsReport report;

  // the annotations before the network of their classes
  ASSERT_TRUE(Dxxwjz2pParser::parse_file(
      file_of(document(
          annotations(
              PARK_GROUP +
              R"(, {"db": {"path": "/srv/shots"}, "annotations": [{"rel_path": "a/b.jpg", "annotations": []}]})") +
          ", " + NETWORK_MEMBER)),
      result, report))
      << report.first_error();

  const std::vector<Yolo4Image>& images = result.annotations.images;

  EXPECT_TRUE(report.warnings().empty());
  ASSERT_EQ(images.size(), 3U);
  EXPECT_EQ(images[0].path, (dir / "data" / "park.jpg").string());
  EXPECT_EQ(images[0].width, 640);
  EXPECT_EQ(images[0].height, 480);
  EXPECT_FALSE(images[0].validation);
  EXPECT_EQ(images[0].boxes,
            (std::vector<Yolo4Box>{box_of(1, 200, 130, 48, 52, 640, 480),
                                   box_of(0, 0, 0, 320, 240, 640, 480)}));
  EXPECT_EQ(images[1].path, (dir / "data" / "street.png").string());
  EXPECT_TRUE(images[1].validation);
  EXPECT_TRUE(images[1].boxes.empty());
  EXPECT_EQ(images[2].path, "/srv/shots/a/b.jpg");
  EXPECT_EQ(images[2].width, 0);
  EXPECT_EQ(result.annotations.training_paths(),
            (std::vector<std::string>{images[0].path, images[2].path}));
  EXPECT_EQ(result.annotations.validation_paths(),
            std::vector<std::string>{images[1].path});
}

TEST_F(CTEST_Dxxwjz2p, wrong_annotations_are_rejected)
{
  const std::string rect =
      R"("rect": {"x": 1, "y": 2, "width": 3, "height": 4})";
  const std::vector<std::pair<std::string, std::vector<std::string>>> cases{
      {document(NETWORK_MEMBER), {"the document has no \"annotations\" key"}},
      {document(NETWORK_MEMBER + R"(, "annotations": [])"),
       {"isn't an object"}},
      {document(NETWORK_MEMBER + ", " +
                annotations(R"({"db": {"path": 7}, "annotations": []})")),
       {"\"path\"", "isn't a string"}},
      {document(NETWORK_MEMBER + ", " +
                annotations(R"({"db": {}, "annotations": []})")),
       {"has no \"path\" key"}},
      {document(NETWORK_MEMBER + ", " +
                image_group(R"({"rel_path": "", "annotations": []})")),
       {"\"rel_path\"", "isn't a path"}},
      {document(
           NETWORK_MEMBER + ", " +
           image_group(
               R"({"rel_path": "a.jpg", "validation": 1, "annotations": []})")),
       {"\"validation\"", "isn't a boolean"}},
      {document(
           NETWORK_MEMBER + ", " +
           image_group(
               R"({"rel_path": "a.jpg", "width": -1, "annotations": []})")),
       {"\"width\"", "isn't a count of the pixels"}},
      {document(
           NETWORK_MEMBER + ", " +
           image_group(
               R"({"rel_path": "a.jpg", "width": 9, "height": 9, "annotations": [{"name": "bird", )" +
               rect + "}]}")),
       {"\"name\"", "isn't a class of the network"}},
      {document(
           NETWORK_MEMBER + ", " +
           image_group(
               R"({"rel_path": "a.jpg", "annotations": [{"name": "cat", )" +
               rect + "}]}")),
       {"of no width and height"}},
      {document(
           NETWORK_MEMBER + ", " +
           image_group(
               R"({"rel_path": "a.jpg", "width": 9, "height": 9, "annotations": [{"name": "cat", "rect": {"x": 1, "y": 2, "width": 3}}]})")),
       {"has no \"height\" key"}},
      {document(
           NETWORK_MEMBER + ", " +
           image_group(
               R"({"rel_path": "a.jpg", "width": 9, "height": 9, "annotations": [{"name": "cat", "rect": {"x": "1", "y": 2, "width": 3, "height": 4}}]})")),
       {"\"x\"", "isn't a number"}},
      {document(NETWORK_MEMBER + ", " +
                image_group(R"({"rel_path": ")" +
                            std::string(Yolo4Image::MAX_PATH, 'a') +
                            R"(", "annotations": []})")),
       {"longer than the darknet ones"}},
      {R"({"format": "dxxwjz2", "version": 2, )" + NETWORK_MEMBER + ", " +
           annotations("") + "}",
       {"dxxwjz2"}}};

  for (const auto& [text, words] : cases) {
    EXPECT_TRUE(rejects(text, words));
  }
}

TEST_F(CTEST_Dxxwjz2p, unknown_keys_of_the_images_are_the_warnings)
{
  std::istringstream json{document(
      NETWORK_MEMBER + ", " +
      image_group(R"({"rel_path": "a.jpg", "tags": [], "annotations": []})"))};
  WeightsDocument result;
  WeightsReport report;

  ASSERT_TRUE(Dxxwjz2pParser::parse(json, result, report))
      << report.first_error();
  ASSERT_EQ(report.warnings().size(), 1U);
  EXPECT_NE(report.warnings().front().find("\"tags\""), std::string::npos);
  EXPECT_EQ(result.annotations.images.size(), 1U);
}

TEST_F(CTEST_Dxxwjz2p, peek_reads_the_annotations_after_the_weights)
{
  const std::string path = file_of(document(
      NETWORK_MEMBER + ", " + LAYERS_MEMBER + ", " + annotations(PARK_GROUP)));
  WeightsDocument rest;
  WeightsReport report;
  bool weights = false;

  ASSERT_TRUE(Dxxwjz2pParser::peek(path, rest, weights, report))
      << report.first_error();

  EXPECT_TRUE(weights);
  EXPECT_TRUE(rest.layers.empty());
  EXPECT_EQ(rest.header.seen, 192U);
  EXPECT_EQ(rest.annotations.images.size(), 2U);
}

TEST_F(CTEST_Dxxwjz2p, written_project_reads_back_the_annotations)
{
  // the dyadic fractions of the pixels of the images read back bit by bit
  const WeightsDocument project = project_of(
      {image_of(dir / "data" / "park.jpg",
                {Yolo4Box{1, 0.5f, 0.25f, 0.125f, 0.375f},
                 Yolo4Box{0, 0.75f, 0.5f, 0.5f, 1.f}}),
       image_of(dir / "data" / "sub" / "street.png", {}, true),
       image_of("/srv/shots/a/b.jpg", {Yolo4Box{0, 0.5f, 0.5f, 1.f, 1.f}})});
  const std::string path = (dir / "project.dxxwjz2p").string();
  WeightsDocument read;

  ASSERT_TRUE(Dxxwjz2pWriter{}.write(project, path));
  ASSERT_TRUE(Dxxwjz2pReader{}.read(path, nullptr, read));

  ASSERT_EQ(read.annotations.images.size(), 3U);

  for (const Yolo4Image& expected : project.annotations.images) {
    const auto found = std::find_if(
        read.annotations.images.begin(), read.annotations.images.end(),
        [&expected](const Yolo4Image& r) { return r.path == expected.path; });

    ASSERT_NE(found, read.annotations.images.end()) << expected.path;
    EXPECT_EQ(found->boxes, expected.boxes) << expected.path;
    EXPECT_EQ(found->width, expected.width);
    EXPECT_EQ(found->height, expected.height);
    EXPECT_EQ(found->validation, expected.validation);
  }

  // the ImagesAnnotator structure of the groups of the folders of the images
  const nlohmann::json json = gunzipped(path);
  const nlohmann::json& groups = json["annotations"]["annotations"];

  EXPECT_EQ(json["format"], "dxxwjz2p");
  EXPECT_EQ(json["version"], 1);
  EXPECT_EQ(json["annotations"]["ImagesAnnotator"]["version"], "0.12.0");
  ASSERT_EQ(groups.size(), 3U);
  EXPECT_EQ(groups[0]["db"]["path"], "/srv/shots/a");
  EXPECT_EQ(groups[1]["db"]["path"], "data");
  EXPECT_EQ(groups[2]["db"]["path"], "data/sub");
  EXPECT_EQ(groups[1]["annotations"][0]["rel_path"], "park.jpg");
  EXPECT_EQ(
      groups[1]["annotations"][0]["annotations"][0],
      nlohmann::json::parse(
          R"({"name": "dog", "rect": {"x": 280, "y": 30, "width": 80, "height": 180}})"));
  EXPECT_FALSE(groups[1]["annotations"][0].contains("validation"));
  EXPECT_EQ(groups[2]["annotations"][0]["validation"], true);
}

TEST_F(CTEST_Dxxwjz2p, relative_images_move_with_the_project_folder)
{
  const fs::path project = dir / "project";
  WeightsDocument read;

  fs::create_directories(project);

  ASSERT_TRUE(Dxxwjz2pWriter{}.write(
      project_of({image_of(project / "images" / "a.jpg", {}),
                  image_of(dir / "elsewhere" / "b.jpg", {})}),
      (project / "p.dxxwjz2p").string()));

  fs::rename(project, dir / "moved");

  ASSERT_TRUE(Dxxwjz2pReader{}.read((dir / "moved" / "p.dxxwjz2p").string(),
                                    nullptr, read));
  ASSERT_EQ(read.annotations.images.size(), 2U);

  // the images out of the project folder are of their absolute paths
  EXPECT_EQ(read.annotations.images[0].path,
            (dir / "elsewhere" / "b.jpg").string());
  EXPECT_EQ(read.annotations.images[1].path,
            (dir / "moved" / "images" / "a.jpg").string());
}

TEST_F(CTEST_Dxxwjz2p, writer_refuses_the_boxes_it_cannot_keep)
{
  Yolo4Image unsized =
      image_of(dir / "a.jpg", {Yolo4Box{0, 0.5f, 0.5f, 1.f, 1.f}});

  unsized.width = 0;

  for (const Yolo4Image& wrong :
       {unsized, image_of(dir / "a.jpg", {Yolo4Box{2, 0.5f, 0.5f, 1.f, 1.f}}),
        image_of(dir / "a.jpg", {Yolo4Box{-1, 0.5f, 0.5f, 1.f, 1.f}}),
        image_of(dir / "a.jpg",
                 {Yolo4Box{0, std::numeric_limits<float>::quiet_NaN(), 0.5f,
                           1.f, 1.f}}),
        image_of(dir / "", {})}) {
    const std::string path = (dir / "project.dxxwjz2p").string();

    EXPECT_FALSE(Dxxwjz2pWriter{}.write(project_of({wrong}), path))
        << wrong.path;
    EXPECT_FALSE(fs::exists(path));
  }
}

TEST_F(CTEST_Dxxwjz2p, project_converts_into_the_weights_of_the_inference)
{
  const auto convert = [](const std::string& from, const std::string& src,
                          const std::string& to, const std::string& dst) {
    WeightsConversion conversion;
    conversion.from = WeightsFileFormats::named(from);
    conversion.src = src;
    conversion.to = WeightsFileFormats::named(to);
    conversion.dst = dst;
    conversion.cfg = from == "weights" ? CTEST_Dxxwjz2p_CFG : "";
    return WeightsConverter{}.convert(conversion);
  };
  const std::string project = (dir / "project.dxxwjz2p").string();
  const std::string weights = (dir / "net.dxxwjz2").string();
  WeightsDocument document;
  WeightsDocument direct;
  WeightsDocument stripped;

  // the project of the weights and the annotations
  ASSERT_TRUE(convert("weights", CTEST_Dxxwjz2p_WEIGHTS, "dxxwjz2p", project));
  ASSERT_TRUE(Dxxwjz2pReader{}.read(project, nullptr, document));

  document.network.classes({"cat", "dog"});
  document.annotations.images = {
      image_of(dir / "a.jpg", {Yolo4Box{1, 0.5f, 0.5f, 1.f, 1.f}})};

  ASSERT_TRUE(Dxxwjz2pWriter{}.write(document, project));

  // the weights file of the project keeps no annotations
  ASSERT_TRUE(convert("dxxwjz2p", project, "dxxwjz2", weights));
  ASSERT_TRUE(convert("weights", CTEST_Dxxwjz2p_WEIGHTS, "dxxwjz2",
                      (dir / "direct.dxxwjz2").string()));
  ASSERT_TRUE(WeightsFileFormats::named("dxxwjz2")->create_reader()->read(
      weights, nullptr, stripped));
  ASSERT_TRUE(WeightsFileFormats::named("dxxwjz2")->create_reader()->read(
      (dir / "direct.dxxwjz2").string(), nullptr, direct));

  EXPECT_FALSE(gunzipped(weights).contains("annotations"));
  EXPECT_TRUE(stripped.annotations.empty());
  EXPECT_EQ(stripped.floats(), direct.floats());
  EXPECT_EQ(stripped.network.classes(),
            (std::vector<std::string>{"cat", "dog"}));

  // the project is the network and the weights of the inference alone
  std::vector<std::string> classes;

  ASSERT_NE(
      OrigDefaultLoader{}.load_inference_network(project, project, "", classes),
      nullptr);
  EXPECT_EQ(classes, (std::vector<std::string>{"cat", "dog"}));
}
