#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pArchive.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>
#include <set>
#include <string>
#include <system_error>
#include <vector>

#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pFormat.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pZip.h"
#include "src/log/log.h"
#include "src/zlib/ZlibController.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

namespace fs = std::filesystem;

namespace
{

/// @brief The size of the data of the gzip compressed or the plain file,
/// false for the file unable to be read.
bool size_of(const fs::path& path, uint64_t& size)
{
  std::unique_ptr<zlibi::GzipIStream> file =
      zlibi::ZlibController::create()->create_file_reader(path.string());

  if (file == nullptr) {
    return false;
  }

  std::vector<char> chunk(1U << 16U);

  size = 0U;

  while (*file) {
    file->read(chunk.data(), static_cast<std::streamsize>(chunk.size()));
    size += static_cast<uint64_t>(file->gcount());
  }

  return !file->failed() && !file->bad();
}

}  // namespace

bool Dxxwjz4pArchive::member_path(const std::string& name, fs::path& relative)
{
  relative.clear();

  if (name.empty() || name.front() == '/' ||
      name.find('\\') != std::string::npos ||
      name.find('\0') != std::string::npos) {
    return false;
  }

  const fs::path path{name};

  if (path.has_root_name() || path.has_root_directory()) {
    return false;
  }

  for (const fs::path& part : path) {
    const std::string text = part.string();

    if (text == "..") {
      return false;
    }

    // the "./" parts and the trailing slash of the directories
    if (!text.empty() && text != ".") {
      relative /= part;
    }
  }

  return !relative.empty();
}

bool Dxxwjz4pArchive::unpack(const std::string& archive,
                             const fs::path& directory, WeightsReport& report)
{
  std::ifstream file{archive, std::ios::binary};

  if (!file) {
    report.error("the file can't be opened");
    return false;
  }

  // the dxxwjz3p project or another gzip compressed dxxwjz file renamed
  if (zlibi::ZlibController::is_gzip_file(archive)) {
    report.error(
        "the file is gzip compressed, it isn't the ZIP archive of the "
        "dxxwjz4p project; convert it's source into the dxxwjz4p project");
    return false;
  }

  Dxxwjz4pZipReader zip{file};
  Dxxwjz4pZipEntry entry;
  std::set<fs::path> members;
  std::error_code error;
  bool project = false;

  while (zip.next(entry)) {
    fs::path relative;

    // the root folder of the archive of the folder packed of the "./" names
    if (!entry.file && (entry.name == "." || entry.name == "./")) {
      continue;
    }

    if (!member_path(entry.name, relative)) {
      report.error("the archive member " + entry.name +
                   " isn't of a path inside of the project");
      return false;
    }

    const fs::path target = directory / relative;

    if (!entry.file) {
      fs::create_directories(target, error);

      if (error) {
        report.error("the folder " + relative.generic_string() +
                     " can't be created: " + error.message());
        return false;
      }

      continue;
    }

    if (!members.insert(relative).second) {
      report.error("the archive member " + relative.generic_string() +
                   " is given twice");
      return false;
    }

    fs::create_directories(target.parent_path(), error);

    std::ofstream out{target, std::ios::binary | std::ios::trunc};

    if (error || !out) {
      report.error("the archive member " + relative.generic_string() +
                   " can't be unpacked into " + target.string());
      return false;
    }

    if (!zip.read(out)) {
      break;
    }

    out.close();

    if (!out) {
      report.error("the archive member " + relative.generic_string() +
                   " can't be written into " + target.string());
      return false;
    }

    project = project || relative == fs::path{Dxxwjz4pFormat::PROJECT_MEMBER};
  }

  if (!zip.error().empty()) {
    report.error(zip.error());
    return false;
  }

  if (!project) {
    report.error("the archive has no " +
                 std::string{Dxxwjz4pFormat::PROJECT_MEMBER} +
                 " member, it isn't the dxxwjz4p project");
    return false;
  }

  return true;
}

bool Dxxwjz4pArchive::pack(const fs::path& json,
                           const std::vector<Member>& members,
                           const std::string& archive, const int level)
{
  uint64_t size = 0U;

  if (!size_of(json, size)) {
    LOGE("Fail to read the JSON document " << json.string());
    return false;
  }

  // the ZIP writer seeks back to the local headers of the members
  std::ofstream file{archive,
                     std::ios::binary | std::ios::out | std::ios::trunc};

  if (!file) {
    LOGE("Fail to open the file " << archive);
    return false;
  }

  Dxxwjz4pZipWriter zip{file, level};
  std::unique_ptr<zlibi::GzipIStream> text =
      zlibi::ZlibController::create()->create_file_reader(json.string());

  if (text == nullptr || !zip.file(Dxxwjz4pFormat::PROJECT_MEMBER, size) ||
      !zip.data(*text) || !zip.complete() || text->failed()) {
    LOGE("Fail to pack the JSON document into " << archive);
    return false;
  }

  std::set<std::string> folders;
  std::error_code error;

  for (const Member& member : members) {
    // the folders of the members before them, so the tools unpack them of
    // the usual permissions
    const fs::path parent = fs::path{member.first}.parent_path();
    std::string folder;

    for (const fs::path& part : parent) {
      folder += part.string() + "/";

      if (folders.insert(folder).second && !zip.directory(folder)) {
        LOGE("Fail to write the folder " << folder << " into " << archive);
        return false;
      }
    }

    const uint64_t bytes = fs::file_size(member.second, error);
    std::ifstream in{member.second, std::ios::binary};

    if (error || !in) {
      LOGE("Fail to read the file " << member.second.string());
      return false;
    }

    if (!zip.file(member.first, bytes) || !zip.data(in) || !zip.complete()) {
      LOGE("Fail to pack the file " << member.second.string() << " into "
                                    << archive
                                    << " (the file changed while packed?)");
      return false;
    }
  }

  if (!zip.finish()) {
    LOGE("Failure while writing the file " << archive);
    return false;
  }

  file.close();

  if (!file) {
    LOGE("Failure while writing the file " << archive);
    return false;
  }

  return true;
}

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1
