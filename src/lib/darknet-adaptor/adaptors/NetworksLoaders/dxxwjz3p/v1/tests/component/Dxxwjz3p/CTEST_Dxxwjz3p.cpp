#include <gtest/gtest.h>
#include <zlib.h>

#include <filesystem>
#include <memory>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pParser.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pWriter.h"
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
using darknet_adaptor::loaders::dxxwjz3p::v1::Dxxwjz3pParser;
using darknet_adaptor::loaders::dxxwjz3p::v1::Dxxwjz3pReader;
using darknet_adaptor::loaders::dxxwjz3p::v1::Dxxwjz3pWriter;

namespace fs = std::filesystem;

namespace
{

const std::string NETWORK_MEMBER =
    R"("network": {"net": {"width": 8}, "layers": [{"type": "convolutional", "arrays": {"biases": [0.5, -1]}}, {"type": "maxpool"}], "classes": [{"index": 0, "name": "cat"}, {"index": 1, "name": "dog"}]})";

const std::string ANNOTATIONS_MEMBER =
    R"("annotations": {"ImagesAnnotator": {"version": "0.12.0"}, "annotations": [{"db": {"path": "data"}, "annotations": [
  {"rel_path": "park.jpg", "width": 640, "height": 480, "annotations": [
    {"name": "dog", "rect": {"x": 200, "y": 130, "width": 48, "height": 52}}]},
  {"rel_path": "street.png", "width": 320, "height": 200, "validation": true, "annotations": []}]}]})";

const std::string WEIGHTS_MEMBERS =
    R"("header": {"major": 0, "minor": 2, "revision": 5, "seen": 192}, "trailing": [7])";

const std::string READ_CFG = "[net]\nwidth=8\n\n[convolutional]\n\n[maxpool]\n";

std::string document(const std::string& members)
{
  return R"({"format": "dxxwjz3p", "version": 1, )" + members + "}";
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

}  // namespace

class CTEST_Dxxwjz3p : public testing::Test
{
 public:
  CTEST_Dxxwjz3p()
      : dir{fs::path{CTEST_Dxxwjz3p_DATA_DIR} /
            testing::UnitTest::GetInstance()->current_test_info()->name()}
  {
    fs::remove_all(dir);
    fs::create_directories(dir);
  }

  ~CTEST_Dxxwjz3p() override { fs::remove_all(dir); }

  /// @brief The gzip compressed file of the text.
  std::string file_of(const std::string& text)
  {
    const std::string path = (dir / "project.dxxwjz3p").string();
    std::unique_ptr<std::ostream> file =
        zlibi::ZlibController::create()->create_file_writer(path);

    *file << text;
    file.reset();

    return path;
  }

  fs::path dir;
};

TEST_F(CTEST_Dxxwjz3p, reads_the_annotations_of_the_json_network)
{
  WeightsDocument result;
  WeightsReport report;

  // the annotations before the network of their classes
  ASSERT_TRUE(Dxxwjz3pParser::parse_file(
      file_of(document(ANNOTATIONS_MEMBER + ", " + NETWORK_MEMBER + ", " +
                       WEIGHTS_MEMBERS)),
      result, report))
      << report.first_error();

  const std::vector<Yolo4Image>& images = result.annotations.images;

  EXPECT_TRUE(report.warnings().empty());
  EXPECT_EQ(result.network.cfg(), READ_CFG);
  ASSERT_EQ(result.layers.size(), 1U);
  EXPECT_EQ(result.layers[0].type, CONVOLUTIONAL);
  EXPECT_EQ(result.layers[0].arrays.at(0).values,
            (std::vector<float>{0.5f, -1.f}));
  EXPECT_EQ(result.trailing, std::vector<uint8_t>{7U});
  ASSERT_EQ(images.size(), 2U);
  EXPECT_EQ(images[0].path, (dir / "data" / "park.jpg").string());
  EXPECT_EQ(images[0].boxes.size(), 1U);
  EXPECT_EQ(images[0].boxes.front().id, 1);
  EXPECT_TRUE(images[1].validation);

  // the network of the cfg text of the dxxwjz2 one is the format error
  std::istringstream v2{
      document(R"("network": {"cfg": "[net]\n"}, )" + ANNOTATIONS_MEMBER)};
  WeightsReport v2_report;

  EXPECT_FALSE(Dxxwjz3pParser::parse(v2, result, v2_report));
  EXPECT_NE(v2_report.first_error().find("has no \"net\" key"),
            std::string::npos)
      << v2_report.first_error();
}

TEST_F(CTEST_Dxxwjz3p, peek_reads_the_annotations_after_the_weights)
{
  const std::string path = file_of(document(
      NETWORK_MEMBER + ", " + WEIGHTS_MEMBERS + ", " + ANNOTATIONS_MEMBER));
  WeightsDocument rest;
  WeightsReport report;
  bool weights = false;

  ASSERT_TRUE(Dxxwjz3pParser::peek(path, rest, weights, report))
      << report.first_error();

  EXPECT_TRUE(weights);
  EXPECT_TRUE(rest.layers.empty());
  EXPECT_EQ(rest.network.cfg(), READ_CFG);
  EXPECT_EQ(rest.annotations.images.size(), 2U);
}

TEST_F(CTEST_Dxxwjz3p, project_converts_through_the_formats_of_the_projects)
{
  const auto convert = [](const std::string& from, const std::string& src,
                          const std::string& to, const std::string& dst) {
    WeightsConversion conversion;
    conversion.from = WeightsFileFormats::named(from);
    conversion.src = src;
    conversion.to = WeightsFileFormats::named(to);
    conversion.dst = dst;
    conversion.cfg = from == "weights" ? CTEST_Dxxwjz3p_CFG : "";
    return WeightsConverter{}.convert(conversion);
  };
  const std::string project = (dir / "project.dxxwjz3p").string();
  const std::string back = (dir / "back.dxxwjz2p").string();
  WeightsDocument document;
  WeightsDocument read;
  WeightsDocument v2;

  // the project of the weights and the annotations
  ASSERT_TRUE(convert("weights", CTEST_Dxxwjz3p_WEIGHTS, "dxxwjz3p", project));
  ASSERT_TRUE(Dxxwjz3pReader{}.read(project, nullptr, document));

  document.network.classes({"cat", "dog"});
  document.annotations.images = {
      image_of(dir / "data" / "a.jpg", {Yolo4Box{1, 0.5f, 0.5f, 1.f, 1.f}}),
      image_of(dir / "data" / "b.jpg", {}, true)};

  ASSERT_TRUE(Dxxwjz3pWriter{}.write(document, project));

  const nlohmann::json json = gunzipped(project);

  EXPECT_EQ(json["format"], "dxxwjz3p");
  EXPECT_EQ(json["version"], 1);
  EXPECT_EQ(json["network"]["net"]["width"], 32);
  EXPECT_EQ(json["network"]["net"]["steps"],
            nlohmann::json::parse("[448000, 449600]"));
  EXPECT_EQ(json["network"]["layers"][0]["type"], "convolutional");
  EXPECT_EQ(json["annotations"]["annotations"][0]["db"]["path"], "data");

  // the dxxwjz2p project of the same weights and annotations and back
  ASSERT_TRUE(convert("dxxwjz3p", project, "dxxwjz2p", back));
  ASSERT_TRUE(convert("dxxwjz2p", back, "dxxwjz3p", project));
  ASSERT_TRUE(WeightsFileFormats::named("dxxwjz2p")
                  ->create_reader()
                  ->read(back, nullptr, v2));
  ASSERT_TRUE(Dxxwjz3pReader{}.read(project, nullptr, read));

  EXPECT_EQ(v2.network.cfg(), document.network.cfg());
  EXPECT_EQ(read.network.cfg(), document.network.cfg());
  EXPECT_EQ(read.floats(), document.floats());
  EXPECT_EQ(read.annotations.training_paths(),
            document.annotations.training_paths());
  EXPECT_EQ(read.annotations.validation_paths(),
            document.annotations.validation_paths());

  // the project is the network and the weights of the inference alone
  std::vector<std::string> classes;

  ASSERT_NE(
      OrigDefaultLoader{}.load_inference_network(project, project, "", classes),
      nullptr);
  EXPECT_EQ(classes, (std::vector<std::string>{"cat", "dog"}));
}

TEST_F(CTEST_Dxxwjz3p, writer_refuses_what_the_project_cannot_keep)
{
  const std::string path = (dir / "project.dxxwjz3p").string();
  WeightsDocument document;

  // the cfg of no [net] section first, the box of no class name
  document.network = NetworkDescription{"[maxpool]\n", {"cat"}};

  EXPECT_FALSE(Dxxwjz3pWriter{}.write(document, path));

  document.network = NetworkDescription{"[net]\n", {"cat"}};
  document.annotations.images = {
      image_of(dir / "a.jpg", {Yolo4Box{1, 0.5f, 0.5f, 1.f, 1.f}})};

  EXPECT_FALSE(Dxxwjz3pWriter{}.write(document, path));
  EXPECT_FALSE(fs::exists(path));
}
