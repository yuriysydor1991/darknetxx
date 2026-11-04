#include <gtest/gtest.h>

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "FormatsAccess.h"
#include "src/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"

using darknet_adaptor::loaders::NetworkSource;
using darknet_adaptor::loaders::OrigDefaultLoader;
using darknet_adaptor::loaders::WeightsConversion;
using darknet_adaptor::loaders::WeightsConverter;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsFileFormats;
using darknet_adaptor::loaders::WeightsReport;
using darknet_adaptor::loaders::Yolo4Box;
using darknet_adaptor::loaders::Yolo4Image;
using darknet_adaptor::loaders::dxxwjz4::v4::Dxxwjz4Reader;
using darknet_adaptor::loaders::dxxwjz4p::v1::Dxxwjz4pReader;
using darknet_adaptor::loaders::dxxwjz4p::v1::Dxxwjz4pWriter;

namespace fs = std::filesystem;

namespace
{

/// the members of the archive: the names and the contents of the files
using Members = std::vector<std::pair<std::string, std::string>>;

const std::string NETWORK_MEMBER =
    R"("network": {"net": {"width": 8}, "layers": [{"type": "convolutional", "arrays": {"biases": [0.5, -1]}}, {"type": "maxpool"}], "classes": ["cat", "dog"]})";

const std::string WEIGHTS_MEMBERS =
    R"("header": {"major": 0, "minor": 2, "revision": 5, "seen": 192})";

/// the signature of the local header
const std::string LOCAL_SIGNATURE{"PK\x03\x04", 4U};

/// @brief The annotations member of the images of the db path.
std::string annotations_of(const std::string& db)
{
  return R"("annotations": {"ImagesAnnotator": {"version": "0.12.0"}, "annotations": [{"db": {"path": )" +
         nlohmann::json(db).dump() + R"(}, "annotations": [
  {"rel_path": "park.jpg", "width": 640, "height": 480, "annotations": [
    {"name": "dog", "rect": {"x": 200, "y": 130, "width": 48, "height": 52}}]},
  {"rel_path": "street.png", "width": 320, "height": 200, "validation": true, "annotations": []}]}]})";
}

/// @brief The JSON document of the project of the images of the db path.
std::string project_json(const std::string& db = "data",
                         const std::string& description = "the parks")
{
  return R"({"format": "dxxwjz4p", "version": 1, "description": )" +
         nlohmann::json(description).dump() + ", " + NETWORK_MEMBER + ", " +
         annotations_of(db) + ", " + WEIGHTS_MEMBERS + "}";
}

std::string bytes_of(const fs::path& path)
{
  std::ifstream file{path, std::ios::binary};

  return std::string{std::istreambuf_iterator<char>{file},
                     std::istreambuf_iterator<char>{}};
}

void write_file(const fs::path& path, const std::string& content)
{
  fs::create_directories(path.parent_path());
  std::ofstream{path, std::ios::binary} << content;
}

/// @brief The member of the ZIP archive the tests write byte by byte: the
/// data stored as it is of the fields of the headers.
struct RawMember
{
  std::string name;
  std::string data;
  uint16_t flags{0U};
  uint16_t method{0U};
  /// the Unix mode of the external attributes
  uint32_t mode{0100644U};
  /// added to the CRC-32 of the data
  uint32_t crc_error{0U};
};

void put(std::string& out, const uint64_t value, const std::size_t bytes)
{
  for (std::size_t at = 0U; at < bytes; ++at) {
    out += static_cast<char>((value >> (8U * at)) & 0xFFU);
  }
}

uint32_t crc32_of(const std::string& data)
{
  uint32_t crc = 0xFFFFFFFFU;

  for (const char byte : data) {
    crc ^= static_cast<unsigned char>(byte);

    for (int bit = 0; bit < 8; ++bit) {
      crc = (crc >> 1U) ^ (0xEDB88320U & (0U - (crc & 1U)));
    }
  }

  return ~crc;
}

/// @brief The ZIP archive of the Unix made members: the central directory
/// of the local headers of the indexes, of every member in it's order for
/// none.
std::string raw_zip(const std::vector<RawMember>& members,
                    std::vector<std::size_t> central = {})
{
  std::string zip;
  std::vector<uint64_t> offsets;

  for (const RawMember& member : members) {
    offsets.push_back(zip.size());
    put(zip, 0x04034B50U, 4U);
    put(zip, 20U, 2U);
    put(zip, member.flags, 2U);
    put(zip, member.method, 2U);
    // the time and the date
    put(zip, 0U, 4U);
    put(zip, crc32_of(member.data) + member.crc_error, 4U);
    put(zip, member.data.size(), 4U);
    put(zip, member.data.size(), 4U);
    put(zip, member.name.size(), 2U);
    put(zip, 0U, 2U);
    zip += member.name + member.data;
  }

  if (central.empty()) {
    for (std::size_t at = 0U; at < members.size(); ++at) {
      central.push_back(at);
    }
  }

  const uint64_t start = zip.size();

  for (const std::size_t at : central) {
    const RawMember& member = members[at];

    put(zip, 0x02014B50U, 4U);
    put(zip, (3U << 8U) | 20U, 2U);
    put(zip, 20U, 2U);
    put(zip, member.flags, 2U);
    put(zip, member.method, 2U);
    put(zip, 0U, 4U);
    put(zip, crc32_of(member.data) + member.crc_error, 4U);
    put(zip, member.data.size(), 4U);
    put(zip, member.data.size(), 4U);
    put(zip, member.name.size(), 2U);
    // the extra field, the comment, the disk and the internal attributes
    put(zip, 0U, 8U);
    put(zip, static_cast<uint64_t>(member.mode) << 16U, 4U);
    put(zip, offsets[at], 4U);
    zip += member.name;
  }

  const uint64_t size = zip.size() - start;

  put(zip, 0x06054B50U, 4U);
  put(zip, 0U, 4U);
  put(zip, central.size(), 2U);
  put(zip, central.size(), 2U);
  put(zip, size, 4U);
  put(zip, start, 4U);
  put(zip, 0U, 2U);

  return zip;
}

/**
 * @brief Writes the ZIP archive of the members, stored as they are: the
 * archives the tests break on purpose, which the writer of the library makes
 * none of (the ZIP writer and reader themselves are the tests of the
 * darknetxx-datadrivers library, they are a part of it's implementation).
 */
void archive_of(const fs::path& path, const Members& members)
{
  std::vector<RawMember> raw;

  for (const auto& [name, content] : members) {
    raw.push_back(RawMember{name, content});
  }

  write_file(path, raw_zip(raw));
}

/**
 * @brief Tells the read of the project file fails. The errors of the archive
 * are the ones of the reader of the darknetxx-datadrivers library, the tests
 * of the library check their texts.
 */
bool refused(const fs::path& path)
{
  WeightsDocument document;

  return !Dxxwjz4pReader{}.read(path.string(), nullptr, document);
}

/// @brief The command of the zip tool of the system extracting the archive
/// into the folder, empty for no tool.
std::string unzip_command(const fs::path& archive, const fs::path& folder)
{
#if defined(_WIN32)
  (void)archive;
  (void)folder;
  return {};
#else
  // the UTF-8 names of the members as they are
  const std::string quoted =
      " \"" + archive.string() + "\" -d \"" + folder.string() + "\"";

  if (std::system("unzip -v > /dev/null 2>&1") == 0) {
    return "LC_ALL=C.UTF-8 unzip -qq -o" + quoted + " > /dev/null 2>&1";
  }

  if (std::system("bsdtar --version > /dev/null 2>&1") == 0) {
    return "LC_ALL=C.UTF-8 bsdtar -xf \"" + archive.string() + "\" -C \"" +
           folder.string() + "\" > /dev/null 2>&1";
  }

  return {};
#endif
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

class CTEST_Dxxwjz4p : public testing::Test
{
 public:
  CTEST_Dxxwjz4p()
      : dir{fs::path{CTEST_Dxxwjz4p_DATA_DIR} /
            testing::UnitTest::GetInstance()->current_test_info()->name()}
  {
    fs::remove_all(dir);
    fs::create_directories(dir);
  }

  ~CTEST_Dxxwjz4p() override { fs::remove_all(dir); }

  static bool convert(const std::string& from, const std::string& src,
                      const std::string& to, const std::string& dst,
                      const std::string& description = {})
  {
    WeightsConversion conversion;
    conversion.from =
        WeightsFileFormats::named(from, WeightsFileFormats::convertible());
    conversion.src = src;
    conversion.to =
        WeightsFileFormats::named(to, WeightsFileFormats::convertible());
    conversion.dst = dst;
    conversion.cfg = from == "weights" ? CTEST_Dxxwjz4p_CFG : "";
    conversion.description = description;
    return WeightsConverter{}.convert(conversion);
  }

  /// @brief The document of the reference weights, the classes and the
  /// annotations of the images of the source folder: the same names of the
  /// two folders included.
  WeightsDocument reference_document()
  {
    const std::string net = (dir / "net.dxxwjz4").string();
    WeightsDocument document;

    EXPECT_TRUE(convert("weights", CTEST_Dxxwjz4p_WEIGHTS, "dxxwjz4", net));
    EXPECT_TRUE(Dxxwjz4Reader{}.read(net, nullptr, document));

    write_file(dir / "source" / "a.jpg", "the bytes of the a.jpg image");
    write_file(dir / "source" / "other" / "A.jpg", "the other A.jpg image");
    write_file(dir / "source" / "b.png", std::string(70000U, '\x7f'));

    document.network.classes({"cat", "dog"});
    document.description = "The cats and the dogs\nof the parks.";
    document.annotations.images = {
        image_of(dir / "source" / "a.jpg",
                 {Yolo4Box{1, 0.5f, 0.5f, 0.25f, 0.5f}}),
        image_of(dir / "source" / "other" / "A.jpg",
                 {Yolo4Box{0, 0.25f, 0.75f, 0.5f, 0.25f}}),
        image_of(dir / "source" / "b.png", {}, true)};

    return document;
  }

  /// @brief Checks the zip tool of the system extracts the members of the
  /// archive as the files of their own.
  ///
  /// @return False for no zip tool to check the archive with.
  bool unzipped(const fs::path& archive, const Members& members)
  {
    const fs::path out = dir / ("unzipped-" + archive.stem().string());
    const std::string command = unzip_command(archive, out);

    if (command.empty()) {
      return false;
    }

    fs::create_directories(out);

    EXPECT_EQ(std::system(command.c_str()), 0) << command;

    for (const auto& [name, content] : members) {
      EXPECT_TRUE(fs::is_regular_file(out / name)) << name;
      EXPECT_EQ(bytes_of(out / name), content) << name;
    }

    return true;
  }

  fs::path dir;
};

TEST_F(CTEST_Dxxwjz4p, format_is_registered)
{
  const auto format = WeightsFileFormats::named("dxxwjz4p");

  ASSERT_NE(format, nullptr);
  EXPECT_EQ(format->extension(), ".dxxwjz4p");
  EXPECT_EQ(WeightsFileFormats::of_file("cars.dxxwjz4p"), format);
  EXPECT_TRUE(format->keeps_network());
  EXPECT_TRUE(format->keeps_annotations());
  EXPECT_TRUE(format->keeps_images());
  EXPECT_TRUE(format->keeps_description());
  EXPECT_TRUE(WeightsFileFormats::named("dxxwjz3p")->keeps_annotations());
  EXPECT_FALSE(WeightsFileFormats::named("dxxwjz3p")->keeps_images());

  // the training of the project saves the dxxwjz4 file beside it
  EXPECT_EQ(NetworkSource::trained_of("a/cars.dxxwjz4p"),
            (fs::path{"a"} / "cars.dxxwjz4").string());
  EXPECT_EQ(NetworkSource::trained_of("a/cars.dxxwjz3p"), "a/cars.dxxwjz3p");
  EXPECT_TRUE(NetworkSource::keeps_images("cars.dxxwjz4p"));
  EXPECT_FALSE(NetworkSource::keeps_images("cars.dxxwjz3p"));
}

TEST_F(CTEST_Dxxwjz4p, training_resumes_from_the_dxxwjz4_file_beside)
{
  const std::string project = (dir / "cars.dxxwjz4p").string();
  const std::string trained = (dir / "cars.dxxwjz4").string();

  EXPECT_EQ(NetworkSource::resumed_of(project), project);

  write_file(trained, "the latest weights");

  EXPECT_EQ(NetworkSource::resumed_of(project), trained);
  EXPECT_EQ(NetworkSource::base_of(project, 0), "cars");
}

// The zip_archive_round_trips_and_is_the_standard_one, the
// zip64_records_round_trip, the
// archive_of_the_65535_members_and_more_round_trips and the
// member_paths_stay_inside_of_the_folder cases of the ZIP writer and reader
// and of the archive members are out of the darknetxx tests: the ZIP code is
// the private part of the darknetxx-datadrivers library, the
// CTEST_Dxxwjz4pArchive component test of which runs them.

TEST_F(CTEST_Dxxwjz4p, project_keeps_the_json_document_and_the_images)
{
  const std::string project = (dir / "project.dxxwjz4p").string();
  const WeightsDocument document = reference_document();

  ASSERT_TRUE(Dxxwjz4pWriter{}.write(document, project));
  EXPECT_FALSE(fs::exists(project + ".json-staging"));

  // the ZIP archive of the single JSON document and the images of their
  // names, the members of their own: the files of the folder the reader of
  // the library unpacks the project into (the order of the members is the
  // test of the ZIP writer of the darknetxx-datadrivers library)
  EXPECT_EQ(bytes_of(project).substr(0U, 4U), LOCAL_SIGNATURE);

  WeightsDocument unpacked_document;

  ASSERT_TRUE(Dxxwjz4pReader{}.read(project, nullptr, unpacked_document));
  ASSERT_FALSE(unpacked_document.annotations.images.empty());

  const fs::path unpacked_folder =
      fs::path{unpacked_document.annotations.images[0].path}
          .parent_path()
          .parent_path();
  const Members members{
      {"project.json", bytes_of(unpacked_folder / "project.json")},
      {"data/a.jpg", "the bytes of the a.jpg image"},
      {"data/A_1.jpg", "the other A.jpg image"},
      {"data/b.png", std::string(70000U, '\x7f')}};

  for (const auto& [name, content] : members) {
    EXPECT_TRUE(fs::is_regular_file(unpacked_folder / name)) << name;
    EXPECT_EQ(bytes_of(unpacked_folder / name), content) << name;
  }

  const nlohmann::json json = nlohmann::json::parse(members[0].second);

  EXPECT_EQ(json["format"], "dxxwjz4p");
  EXPECT_EQ(json["version"], 1);
  EXPECT_EQ(json["description"], document.description);
  EXPECT_EQ(json["network"]["classes"],
            nlohmann::json::parse(R"(["cat", "dog"])"));
  EXPECT_TRUE(json["network"]["layers"][0].contains("arrays"));
  ASSERT_EQ(json["annotations"]["annotations"].size(), 1U);
  EXPECT_EQ(json["annotations"]["annotations"][0]["db"]["path"], "data");
  EXPECT_EQ(json["annotations"]["annotations"][0]["annotations"][1]["rel_path"],
            "A_1.jpg");
  EXPECT_TRUE(json.contains("header"));

  // unzipped, the project is the folder of the JSON document and the images
  unzipped(project, members);

  // the read of the images unpacked into the temporary folder
  WeightsDocument read;

  ASSERT_TRUE(Dxxwjz4pReader{}.read(project, nullptr, read));

  EXPECT_EQ(read.description, document.description);
  EXPECT_EQ(read.network.cfg(), document.network.cfg());
  EXPECT_EQ(read.network.classes(), document.network.classes());
  EXPECT_EQ(read.floats(), document.floats());
  ASSERT_EQ(read.annotations.images.size(), 3U);

  const fs::path folder =
      fs::path{read.annotations.images[0].path}.parent_path().parent_path();

  EXPECT_EQ(fs::path{read.annotations.images[0].path}.parent_path().filename(),
            "data");
  EXPECT_EQ(folder.parent_path(), fs::temp_directory_path());
  EXPECT_EQ(folder.filename().string().rfind("darknetxx-dxxwjz4p-project-", 0),
            0U);
  EXPECT_EQ(bytes_of(folder / "project.json"), members[0].second);

  for (std::size_t at = 0U; at < read.annotations.images.size(); ++at) {
    const Yolo4Image& unpacked = read.annotations.images[at];
    const Yolo4Image& source = document.annotations.images[at];

    // the images of the folder the read unpacked the project into
    EXPECT_EQ(fs::path{unpacked.path}.parent_path().parent_path(), folder)
        << unpacked.path;
    EXPECT_EQ(bytes_of(unpacked.path), bytes_of(source.path));
    EXPECT_EQ(unpacked.validation, source.validation);
    EXPECT_EQ(unpacked.width, source.width);
    ASSERT_EQ(unpacked.boxes.size(), source.boxes.size());

    for (std::size_t at_box = 0U; at_box < unpacked.boxes.size(); ++at_box) {
      EXPECT_EQ(unpacked.boxes[at_box].id, source.boxes[at_box].id);
      EXPECT_FLOAT_EQ(unpacked.boxes[at_box].x, source.boxes[at_box].x);
      EXPECT_FLOAT_EQ(unpacked.boxes[at_box].h, source.boxes[at_box].h);
    }
  }

  EXPECT_NE(
      fs::path{document.annotations.images[0].path}.parent_path().parent_path(),
      folder);

  // the peek of the description and the annotations of no weights
  WeightsDocument rest;
  bool weights = false;

  ASSERT_TRUE(Dxxwjz4pReader{}.peek(project, rest, weights));
  EXPECT_TRUE(weights);
  EXPECT_TRUE(rest.layers.empty());
  EXPECT_EQ(rest.description, document.description);
  EXPECT_EQ(rest.annotations.training_paths(),
            read.annotations.training_paths());

  // the project is the network and the weights of the inference
  std::vector<std::string> classes;

  ASSERT_NE(
      OrigDefaultLoader{}.load_inference_network(project, project, "", classes),
      nullptr);
  EXPECT_EQ(classes, (std::vector<std::string>{"cat", "dog"}));
}

TEST_F(CTEST_Dxxwjz4p, reads_share_the_unpacked_folder_of_the_project)
{
  const std::string project = (dir / "shared.dxxwjz4p").string();

  ASSERT_TRUE(Dxxwjz4pWriter{}.write(reference_document(), project));

  // the folder of the images of the read of the project
  const auto folder_of = [&project]() {
    WeightsDocument read;

    if (!Dxxwjz4pReader{}.read(project, nullptr, read) ||
        read.annotations.images.empty()) {
      return fs::path{};
    }

    return fs::path{read.annotations.images[0].path}
        .parent_path()
        .parent_path();
  };

  const fs::path first = folder_of();
  const fs::path second = folder_of();

  ASSERT_FALSE(first.empty());
  EXPECT_EQ(first, second);
  EXPECT_TRUE(fs::is_regular_file(first / "data" / "b.png"));

  // the changed project is unpacked again
  WeightsDocument document = reference_document();

  document.annotations.images.pop_back();
  ASSERT_TRUE(Dxxwjz4pWriter{}.write(document, project));
  fs::last_write_time(project,
                      fs::last_write_time(project) + std::chrono::seconds{5});

  const fs::path changed = folder_of();

  ASSERT_FALSE(changed.empty());
  EXPECT_NE(changed, first);
  EXPECT_FALSE(fs::exists(changed / "data" / "b.png"));
}

TEST_F(CTEST_Dxxwjz4p, project_converts_through_the_other_formats)
{
  const std::string project = (dir / "project.dxxwjz4p").string();
  const std::string v3 = (dir / "out" / "project.dxxwjz3p").string();
  const std::string back = (dir / "back.dxxwjz4p").string();
  const std::string weights = (dir / "trained.dxxwjz4").string();
  const WeightsDocument document = reference_document();

  ASSERT_TRUE(Dxxwjz4pWriter{}.write(document, project));

  // the dxxwjz3p project refers to the copies of the images beside it
  ASSERT_TRUE(convert("dxxwjz4p", project, "dxxwjz3p", v3));

  WeightsDocument v3_read;

  ASSERT_TRUE(WeightsFileFormats::named("dxxwjz3p")
                  ->create_reader()
                  ->read(v3, nullptr, v3_read));
  ASSERT_EQ(v3_read.annotations.images.size(), 3U);
  EXPECT_EQ(v3_read.annotations.images[1].path,
            (dir / "out" / "data" / "A_1.jpg").string());
  EXPECT_EQ(bytes_of(v3_read.annotations.images[1].path),
            "the other A.jpg image");
  EXPECT_TRUE(v3_read.description.empty());
  EXPECT_EQ(v3_read.floats(), document.floats());

  // the same conversion again finds the same copies
  ASSERT_TRUE(convert("dxxwjz4p", project, "dxxwjz3p", v3));

  // the copy of another content isn't overwritten
  write_file(dir / "out" / "data" / "a.jpg", "another image");
  EXPECT_FALSE(convert("dxxwjz4p", project, "dxxwjz3p", v3));
  EXPECT_EQ(bytes_of(dir / "out" / "data" / "a.jpg"), "another image");
  write_file(dir / "out" / "data" / "a.jpg", "the bytes of the a.jpg image");

  // and back into the project of the description of the command line
  ASSERT_TRUE(convert("dxxwjz3p", v3, "dxxwjz4p", back, "back again"));

  WeightsDocument read;

  ASSERT_TRUE(Dxxwjz4pReader{}.read(back, nullptr, read));
  EXPECT_EQ(read.description, "back again");
  EXPECT_EQ(read.floats(), document.floats());
  ASSERT_EQ(read.annotations.images.size(), 3U);
  EXPECT_EQ(bytes_of(read.annotations.images[2].path),
            std::string(70000U, '\x7f'));

  // the dxxwjz4 weights file of the project keeps no annotations
  ASSERT_TRUE(convert("dxxwjz4p", project, "dxxwjz4", weights));

  WeightsDocument trained;

  ASSERT_TRUE(Dxxwjz4Reader{}.read(weights, nullptr, trained));
  EXPECT_EQ(trained.description, document.description);
  EXPECT_TRUE(trained.annotations.empty());
  EXPECT_EQ(trained.floats(), document.floats());

  // the YOLO4 folder of the copies of the images
  const fs::path folder = dir / "yolo4";

  ASSERT_TRUE(convert("dxxwjz4p", project, "yolo4", folder.string()));
  EXPECT_EQ(bytes_of(folder / "data" / "obj" / "b.png"),
            std::string(70000U, '\x7f'));
}

TEST_F(CTEST_Dxxwjz4p, archive_of_unsafe_members_is_refused)
{
  const std::string json = project_json();
  const std::string picture = "the image";

  for (const std::string& unsafe :
       {std::string{"../escaped.txt"}, std::string{"data/../../escaped.txt"},
        std::string{"/tmp/darknetxx-escaped.txt"},
        std::string{"data\\..\\escaped.txt"}}) {
    const fs::path path = dir / "unsafe.dxxwjz4p";

    archive_of(path, {{"project.json", json},
                      {"data/park.jpg", picture},
                      {"data/street.png", picture},
                      {unsafe, "escaped"}});

    EXPECT_TRUE(refused(path)) << unsafe;
  }

  EXPECT_FALSE(fs::exists(fs::temp_directory_path() / "escaped.txt"));
  EXPECT_FALSE(fs::exists(dir / "escaped.txt"));
  EXPECT_FALSE(fs::exists("/tmp/darknetxx-escaped.txt"));

  // the symbolic link member and the device one
  {
    const fs::path path = dir / "link.dxxwjz4p";
    RawMember link{"data/park.jpg", "/etc/passwd"};

    link.mode = 0120777U;
    write_file(path, raw_zip({{"project.json", json}, link}));

    EXPECT_TRUE(refused(path));

    RawMember device{"data/park.jpg", ""};

    device.mode = 060644U;
    write_file(path, raw_zip({{"project.json", json}, device}));

    EXPECT_TRUE(refused(path));
  }

  // the repeated member
  {
    const fs::path path = dir / "twice.dxxwjz4p";

    archive_of(path, {{"project.json", json}, {"./project.json", json}});

    EXPECT_TRUE(refused(path));
  }
}

TEST_F(CTEST_Dxxwjz4p, archive_of_the_wrong_members_is_refused)
{
  const fs::path path = dir / "wrong.dxxwjz4p";
  const std::string json = project_json();

  // the encrypted member
  {
    RawMember encrypted{"data/park.jpg", "the encrypted image"};

    encrypted.flags = 1U;
    write_file(path, raw_zip({{"project.json", json}, encrypted}));

    EXPECT_TRUE(refused(path));
  }

  // the member of the bzip2 compression method
  {
    RawMember bzip2{"data/park.jpg", "BZh91AY&SY"};

    bzip2.method = 12U;
    write_file(path, raw_zip({{"project.json", json}, bzip2}));

    EXPECT_TRUE(refused(path));
  }

  // the member of the wrong CRC-32
  {
    RawMember corrupted{"data/park.jpg", "the image"};

    corrupted.crc_error = 1U;
    write_file(path, raw_zip({{"project.json", json}, corrupted}));

    EXPECT_TRUE(refused(path));
  }

  // the members of the same data, the ones of the zip bombs
  {
    write_file(path, raw_zip({{"project.json", json},
                              {"data/park.jpg", std::string(1000U, 'p')}},
                             {0U, 1U, 1U}));

    EXPECT_TRUE(refused(path));
  }

  // the corrupted deflated data
  {
    archive_of(path, {{"project.json", json},
                      {"data/park.jpg", std::string(200000U, 'i')}});

    std::string bytes = bytes_of(path);
    // the deflated data right after the name of the local header
    const std::size_t start = bytes.find("data/park.jpg") + 13U;

    for (std::size_t at = start + 10U; at < start + 60U; ++at) {
      bytes[at] = static_cast<char>(~bytes[at]);
    }

    write_file(path, bytes);

    EXPECT_TRUE(refused(path));
  }

  // the deflated data of the bigger size than the central directory tells
  {
    archive_of(path, {{"project.json", json},
                      {"data/park.jpg", std::string(5000U, 'i')}});

    std::string bytes = bytes_of(path);
    const std::size_t central = bytes.rfind(std::string{"PK\x01\x02", 4U});

    // the uncompressed size field of the central header of the last member
    bytes[central + 24U] = '\x10';
    bytes[central + 25U] = '\0';
    write_file(path, bytes);

    EXPECT_TRUE(refused(path));
  }
}

TEST_F(CTEST_Dxxwjz4p, archive_of_no_project_is_refused)
{
  const fs::path path = dir / "wrong.dxxwjz4p";

  // no JSON document
  archive_of(path, {{"data/park.jpg", "the image"}});

  EXPECT_TRUE(refused(path));

  // the gzip compressed JSON document of the dxxwjz3p project renamed, the
  // gzip compressed tar archive alike
  {
    ASSERT_TRUE(darknet_adaptor::loaders::gzip_files::write(path.string(),
                                                            project_json()));

    EXPECT_TRUE(refused(path));
  }

  // the plain JSON document
  write_file(path, project_json());

  EXPECT_TRUE(refused(path));

  // the truncated archive
  archive_of(path, {{"project.json", project_json()},
                    {"data/park.jpg", std::string(200000U, 'i')}});
  fs::resize_file(path, fs::file_size(path) / 2U);

  EXPECT_TRUE(refused(path));

  // the empty file and no file at all
  write_file(path, "");

  EXPECT_TRUE(refused(path));
  EXPECT_TRUE(refused(dir / "none.dxxwjz4p"));
}

TEST_F(CTEST_Dxxwjz4p, project_keeps_every_image_of_it_s_annotations)
{
  const fs::path path = dir / "images.dxxwjz4p";
  const std::string picture = "the image";
  WeightsDocument document;

  // the images of the folder outside of the project
  write_file(dir / "outside" / "park.jpg", picture);
  write_file(dir / "outside" / "street.png", picture);
  archive_of(path, {{"project.json",
                     project_json((dir / "outside").generic_string())}});

  EXPECT_TRUE(refused(path));

  // the image the archive has no member of
  archive_of(path,
             {{"project.json", project_json()}, {"data/park.jpg", picture}});

  EXPECT_TRUE(refused(path));

  // the description of no string
  {
    std::string json = project_json();

    json.replace(json.find("\"the parks\""), 11U, "[1, 2]");
    archive_of(path, {{"project.json", json},
                      {"data/park.jpg", picture},
                      {"data/street.png", picture}});

    EXPECT_TRUE(refused(path));
  }

  // the complete project packed by hand, of the "./" names too, the folder
  // of the images of another name
  {
    archive_of(path, {{"./project.json", project_json("pictures")},
                      {"./pictures/park.jpg", picture},
                      {"pictures/street.png", picture}});

    ASSERT_TRUE(Dxxwjz4pReader{}.read(path.string(), nullptr, document));
    EXPECT_EQ(document.description, "the parks");
    ASSERT_EQ(document.annotations.images.size(), 2U);
    EXPECT_TRUE(document.annotations.images[1].validation);
    EXPECT_EQ(bytes_of(document.annotations.images[0].path), picture);
  }
}

TEST_F(CTEST_Dxxwjz4p, writer_refuses_the_images_of_no_file)
{
  const std::string path = (dir / "project.dxxwjz4p").string();
  WeightsDocument document = reference_document();

  document.annotations.images.push_back(
      image_of(dir / "source" / "missing.jpg", {}));

  EXPECT_FALSE(Dxxwjz4pWriter{}.write(document, path));
  EXPECT_FALSE(fs::exists(path));
  EXPECT_FALSE(fs::exists(path + ".json-staging"));
}
