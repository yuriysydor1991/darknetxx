#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Writer.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkFiles.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/yolo4-folder/Yolo4FolderReader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/yolo4-folder/Yolo4FolderWriter.h"

using darknet_adaptor::loaders::NetworkDescription;
using darknet_adaptor::loaders::NetworkFiles;
using darknet_adaptor::loaders::OrigDefaultLoader;
using darknet_adaptor::loaders::WeightsConversion;
using darknet_adaptor::loaders::WeightsConverter;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsFileFormats;
using darknet_adaptor::loaders::Yolo4Box;
using darknet_adaptor::loaders::Yolo4Image;
using darknet_adaptor::loaders::yolo4::Yolo4FolderReader;
using darknet_adaptor::loaders::yolo4::Yolo4FolderWriter;

namespace fs = std::filesystem;

namespace
{

const fs::path DIGITS{CTEST_Yolo4FolderFormat_YOLO4_DIR};

std::string text_of(const fs::path& path)
{
  std::ifstream file{path, std::ios::binary};
  std::ostringstream text;

  text << file.rdbuf();

  return text.str();
}

Yolo4Image image_of(const fs::path& path, const std::vector<Yolo4Box>& boxes,
                    const bool validation)
{
  Yolo4Image result;
  result.path = path.string();
  result.boxes = boxes;
  result.validation = validation;
  return result;
}

/// @brief Converts the source of the conversion format into the destination.
bool convert(const std::string& from, const fs::path& src,
             const std::string& to, const fs::path& dst)
{
  WeightsConversion conversion;
  conversion.from =
      WeightsFileFormats::named(from, WeightsFileFormats::convertible());
  conversion.src = src.string();
  conversion.to =
      WeightsFileFormats::named(to, WeightsFileFormats::convertible());
  conversion.dst = dst.string();
  return WeightsConverter{}.convert(conversion);
}

}  // namespace

class CTEST_Yolo4FolderFormat : public testing::Test
{
 public:
  CTEST_Yolo4FolderFormat()
      : dir{fs::path{CTEST_Yolo4FolderFormat_DATA_DIR} /
            testing::UnitTest::GetInstance()->current_test_info()->name()}
  {
    fs::remove_all(dir);
    fs::create_directories(dir);
  }

  ~CTEST_Yolo4FolderFormat() override { fs::remove_all(dir); }

  fs::path dir;
};

TEST_F(CTEST_Yolo4FolderFormat, digits_folder_reads_into_the_project)
{
  WeightsDocument document;

  ASSERT_TRUE(Yolo4FolderReader{}.read(DIGITS.string(), nullptr, document));

  const std::vector<Yolo4Image>& images = document.annotations.images;

  EXPECT_TRUE(document.network_alone());
  EXPECT_EQ(document.network.classes(),
            (std::vector<std::string>{"0", "1", "2", "3", "4", "5", "6", "7",
                                      "8", "9"}));
  EXPECT_EQ(document.network.cfg(), text_of(DIGITS / "cfg" / "digits.cfg"));
  ASSERT_EQ(images.size(), 80U);
  EXPECT_EQ(images[0].path,
            (DIGITS / "data" / "obj" / "digit_0_0.png").string());

  // the 9x11 glyph of the zero of the smallest size
  EXPECT_EQ(images[0].width, 9);
  EXPECT_EQ(images[0].height, 11);

  for (std::size_t index = 0U; index < images.size(); ++index) {
    const int digit = static_cast<int>(index / 8U);

    EXPECT_EQ(images[index].boxes,
              (std::vector<Yolo4Box>{Yolo4Box{digit, 0.5f, 0.5f, 1.f, 1.f}}))
        << images[index].path;
    EXPECT_FALSE(images[index].validation);
    EXPECT_GT(images[index].width, 0);
  }
}

TEST_F(CTEST_Yolo4FolderFormat, folder_of_no_list_is_no_project)
{
  WeightsDocument document;

  fs::copy(DIGITS, dir / "digits", fs::copy_options::recursive);
  fs::remove(dir / "digits" / "data" / "train.txt");

  EXPECT_FALSE(
      Yolo4FolderReader{}.read((dir / "digits").string(), nullptr, document));
  EXPECT_FALSE(Yolo4FolderReader{}.read(dir.string(), nullptr, document));
}

TEST_F(CTEST_Yolo4FolderFormat, project_writes_the_yolo4_folder)
{
  const fs::path folder = dir / "cars.converting-7";
  const std::vector<Yolo4Box> car{Yolo4Box{1, 0.5f, 0.25f, 0.125f, 0.375f}};
  WeightsDocument project;
  WeightsDocument read;

  // the images of the same names of the folders, the one of both of the lists
  for (const std::string name : {"a/x.png", "b/x.png", "b/X.jpg"}) {
    fs::create_directories((dir / "src" / name).parent_path());
    std::ofstream{dir / "src" / name} << name;
  }

  project.network = NetworkDescription{
      "[net]\n# the lists\ntrain = /elsewhere/train.txt\nwidth=8\n"
      "train=again.txt\n\n[convolutional]\nfilters=8\n",
      {"cat", "car"}};
  project.annotations.images = {
      image_of(dir / "src" / "a" / "x.png", car, false),
      image_of(dir / "src" / "b" / "x.png", {}, false),
      image_of(dir / "src" / "b" / "X.jpg",
               {Yolo4Box{0, 0.75f, 0.5f, 0.5f, 1.f}}, true),
      image_of(dir / "src" / "a" / "x.png", car, true)};

  ASSERT_TRUE(Yolo4FolderWriter{}.write(project, folder.string()));

  // the keys of the folder files, the first line of a key alone
  EXPECT_EQ(text_of(folder / "cfg" / "cars.cfg"),
            "[net]\n# the lists\ntrain=data/train.txt\nwidth=8\n\n"
            "backup=backup\nnames=data/obj.names\nvalid=data/val.txt\n"
            "[convolutional]\nfilters=8\n");
  EXPECT_EQ(text_of(folder / "data" / "obj.names"), "cat\ncar\n");
  EXPECT_EQ(text_of(folder / "data" / "obj.data"),
            "classes=2\ntrain=data/train.txt\nvalid=data/val.txt\n"
            "names=data/obj.names\nbackup=backup/\n");
  EXPECT_EQ(text_of(folder / "data" / "train.txt"),
            "data/obj/x.png\ndata/obj/x_1.png\n");
  EXPECT_EQ(text_of(folder / "data" / "val.txt"),
            "data/obj/X_2.jpg\ndata/obj/x.png\n");
  EXPECT_EQ(text_of(folder / "data" / "obj" / "x.png"), "a/x.png");
  EXPECT_EQ(text_of(folder / "data" / "obj" / "x_1.png"), "b/x.png");
  EXPECT_EQ(text_of(folder / "data" / "obj" / "X_2.jpg"), "b/X.jpg");
  EXPECT_EQ(text_of(folder / "data" / "obj" / "x.txt"),
            "1 0.5 0.25 0.125 0.375\n");
  EXPECT_EQ(text_of(folder / "data" / "obj" / "x_1.txt"), "");
  EXPECT_EQ(text_of(folder / "data" / "obj" / "X_2.txt"), "0 0.75 0.5 0.5 1\n");

  // the network to train keeps no weights
  EXPECT_TRUE(fs::is_directory(folder / "backup"));
  EXPECT_TRUE(fs::is_empty(folder / "backup"));

  // the folder reads back the lists of the images and their boxes
  ASSERT_TRUE(Yolo4FolderReader{}.read(folder.string(), nullptr, read));
  ASSERT_EQ(read.annotations.images.size(), 4U);
  EXPECT_EQ(read.network.classes(), project.network.classes());

  for (std::size_t index = 0U; index < 4U; ++index) {
    EXPECT_EQ(read.annotations.images[index].boxes,
              (std::vector<std::vector<Yolo4Box>>{
                  car, {}, project.annotations.images[2].boxes, car}[index]));
    EXPECT_EQ(read.annotations.images[index].validation, index >= 2U);
  }
}

TEST_F(CTEST_Yolo4FolderFormat, training_images_are_the_ones_of_the_map)
{
  const fs::path folder = dir / "plain";
  WeightsDocument project;

  fs::create_directories(dir / "src");
  std::ofstream{dir / "src" / "a.png"} << "a";

  project.network = NetworkDescription{"[net]\nwidth=8\n", {"cat"}};
  project.annotations.images = {image_of(dir / "src" / "a.png", {}, false)};

  ASSERT_TRUE(Yolo4FolderWriter{}.write(project, folder.string()));

  EXPECT_EQ(text_of(folder / "cfg" / "plain.cfg"),
            "[net]\nwidth=8\nbackup=backup\nnames=data/obj.names\n"
            "train=data/train.txt\nvalid=data/train.txt\n");
  EXPECT_FALSE(fs::exists(folder / "data" / "val.txt"));
  EXPECT_NE(text_of(folder / "data" / "obj.data").find("valid=data/train.txt"),
            std::string::npos);

  // the image of no file is no folder
  project.annotations.images.push_back(
      image_of(dir / "src" / "none.png", {}, false));

  EXPECT_FALSE(Yolo4FolderWriter{}.write(project, (dir / "none").string()));
}

TEST_F(CTEST_Yolo4FolderFormat, folder_converts_into_the_project_and_back)
{
  const fs::path digits = dir / "digits";
  const fs::path weights = digits / "backup" / "digits_last.dxxwjz2";
  WeightsDocument project;
  WeightsDocument back;

  fs::copy(DIGITS, digits, fs::copy_options::recursive);
  fs::create_directories(digits / "backup");

  // the weights of the training of the folder
  {
    const auto net = OrigDefaultLoader{}.parse_network_cfg_custom(
        (digits / "cfg" / "digits.cfg").string(), 1, 1);
    WeightsDocument rest;
    std::string cfg;

    ASSERT_TRUE(
        NetworkFiles::read_text((digits / "cfg" / "digits.cfg").string(), cfg));
    rest.network = NetworkDescription{cfg, {}};

    ASSERT_TRUE(darknet_adaptor::loaders::dxxwjz2::v2::Dxxwjz2Writer{}.save(
        *net->get(), weights.string(), net->get()->n, false, rest));
  }

  ASSERT_TRUE(convert("yolo4", digits, "dxxwjz2p", dir / "digits.dxxwjz2p"));
  ASSERT_TRUE(WeightsFileFormats::named("dxxwjz2p")
                  ->create_reader()
                  ->read((dir / "digits.dxxwjz2p").string(), nullptr, project));

  EXPECT_EQ(project.annotations.images.size(), 80U);
  EXPECT_EQ(project.network.classes().size(), 10U);
  EXPECT_GT(project.floats(), 0U);

  ASSERT_TRUE(
      convert("dxxwjz2p", dir / "digits.dxxwjz2p", "yolo4", dir / "back"));

  // the same names, lists and labels of the images
  EXPECT_EQ(text_of(dir / "back" / "data" / "train.txt"),
            text_of(digits / "data" / "train.txt"));
  EXPECT_EQ(text_of(dir / "back" / "data" / "obj.names"),
            text_of(digits / "data" / "obj.names"));

  for (const auto& entry : fs::directory_iterator{digits / "data" / "obj"}) {
    EXPECT_EQ(text_of(dir / "back" / "data" / "obj" / entry.path().filename()),
              text_of(entry.path()))
        << entry.path();
  }

  ASSERT_TRUE(WeightsFileFormats::named("dxxwjz3")->create_reader()->read(
      (dir / "back" / "backup" / "back_last.dxxwjz3").string(), nullptr, back));
  EXPECT_EQ(back.floats(), project.floats());

  // the folder of the files isn't replaced
  EXPECT_FALSE(
      convert("dxxwjz2p", dir / "digits.dxxwjz2p", "yolo4", dir / "back"));
  EXPECT_TRUE(fs::exists(dir / "back" / "data" / "train.txt"));
}
