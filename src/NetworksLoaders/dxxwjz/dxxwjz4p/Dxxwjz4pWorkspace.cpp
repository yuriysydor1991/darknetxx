#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pWorkspace.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <map>
#include <mutex>
#include <random>
#include <string>
#include <system_error>
#include <vector>

#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pArchive.h"
#include "src/log/log.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

namespace fs = std::filesystem;

namespace
{

/// @brief The project unpacked into the folder.
struct Unpacked
{
  uintmax_t size{0U};
  fs::file_time_type time;
  fs::path folder;
};

/// @brief The unpacked projects of the process, the folders of which are
/// removed at the end of it.
class Registry
{
 public:
  Registry() = default;
  Registry(const Registry&) = delete;
  Registry& operator=(const Registry&) = delete;

  ~Registry()
  {
    for (const fs::path& folder : folders) {
      std::error_code error;
      fs::remove_all(folder, error);
    }
  }

  std::mutex mutex;
  /// the unpacked projects of their absolute paths
  std::map<std::string, Unpacked> projects;
  /// every folder made, the ones of the projects changed since included
  std::vector<fs::path> folders;
};

Registry& registry()
{
  static Registry instance;
  return instance;
}

/// @brief The part of the folder name of the project name: the letters, the
/// digits, the dots, the dashes and the underscores of it.
std::string name_part(const std::string& stem)
{
  std::string part;

  for (const char symbol : stem) {
    const bool kept = (symbol >= 'a' && symbol <= 'z') ||
                      (symbol >= 'A' && symbol <= 'Z') ||
                      (symbol >= '0' && symbol <= '9') || symbol == '.' ||
                      symbol == '-' || symbol == '_';

    if (kept && part.size() < 40U) {
      part += symbol;
    }
  }

  return part.empty() ? std::string{"project"} : part;
}

/// @brief The new folder of the project of the temporary directory, the
/// owner's alone; the empty path for none.
fs::path create_folder(const std::string& stem, std::string& error)
{
  std::error_code failure;
  const fs::path temporary = fs::temp_directory_path(failure);

  if (failure) {
    error = "there is no temporary directory to unpack the project into: " +
            failure.message();
    return {};
  }

  std::random_device device;
  std::mt19937_64 random{(static_cast<uint64_t>(device()) << 32U) ^
                         static_cast<uint64_t>(device())};
  static const char* const DIGITS = "0123456789abcdef";

  for (int attempt = 0; attempt < 16; ++attempt) {
    std::string suffix;
    uint64_t value = random();

    for (int digit = 0; digit < 16; ++digit, value >>= 4U) {
      suffix += DIGITS[value & 0xFU];
    }

    const fs::path folder =
        temporary / ("darknetxx-dxxwjz4p-" + name_part(stem) + "-" + suffix);

    if (fs::create_directory(folder, failure) && !failure) {
      // the images of the project are the user's alone in the shared /tmp
      fs::permissions(folder, fs::perms::owner_all, fs::perm_options::replace,
                      failure);

      return folder;
    }
  }

  error = "the folder of the project can't be created in " +
          temporary.string() + ": " + failure.message();

  return {};
}

/// @brief The absolute path of the file of no symbolic links where
/// possible.
std::string key_of(const std::string& path)
{
  std::error_code error;
  const fs::path canonical = fs::weakly_canonical(path, error);

  return error ? fs::absolute(path, error).string() : canonical.string();
}

/// @brief Tells the path is inside of the folder.
bool inside(const fs::path& path, const fs::path& folder)
{
  const fs::path relative =
      path.lexically_normal().lexically_relative(folder.lexically_normal());

  return !relative.empty() && *relative.begin() != ".." &&
         relative.string() != ".";
}

/// @brief Tells the files are of the same bytes.
bool same_files(const fs::path& one, const fs::path& other)
{
  std::error_code error;

  if (fs::file_size(one, error) != fs::file_size(other, error) || error) {
    return false;
  }

  std::ifstream first{one, std::ios::binary};
  std::ifstream second{other, std::ios::binary};

  return first && second &&
         std::equal(std::istreambuf_iterator<char>{first},
                    std::istreambuf_iterator<char>{},
                    std::istreambuf_iterator<char>{second},
                    std::istreambuf_iterator<char>{});
}

}  // namespace

fs::path Dxxwjz4pWorkspace::of(const std::string& archive,
                               WeightsReport& report)
{
  const std::string key = key_of(archive);
  std::error_code error;
  const uintmax_t size = fs::file_size(key, error);
  const fs::file_time_type time = fs::last_write_time(key, error);

  if (error) {
    report.error("the file can't be opened: " + error.message());
    return {};
  }

  Registry& projects = registry();
  const std::lock_guard<std::mutex> lock{projects.mutex};
  const auto found = projects.projects.find(key);

  if (found != projects.projects.end() && found->second.size == size &&
      found->second.time == time &&
      fs::is_directory(found->second.folder, error)) {
    return found->second.folder;
  }

  std::string failure;
  const fs::path folder = create_folder(fs::path{key}.stem().string(), failure);

  if (folder.empty()) {
    report.error(failure);
    return {};
  }

  projects.folders.push_back(folder);

  if (!Dxxwjz4pArchive::unpack(key, folder, report)) {
    fs::remove_all(folder, error);
    return {};
  }

  LOGI("The " << archive << " project is unpacked into " << folder.string());

  projects.projects[key] = Unpacked{size, time, folder};

  return folder;
}

bool Dxxwjz4pWorkspace::owns(const fs::path& path)
{
  Registry& projects = registry();
  const std::lock_guard<std::mutex> lock{projects.mutex};

  for (const fs::path& folder : projects.folders) {
    if (inside(path, folder)) {
      return true;
    }
  }

  return false;
}

bool Dxxwjz4pWorkspace::export_images(Yolo4Annotations& annotations,
                                      const fs::path& directory,
                                      std::string& error)
{
  std::vector<fs::path> folders;

  {
    Registry& projects = registry();
    const std::lock_guard<std::mutex> lock{projects.mutex};

    folders = projects.folders;
  }

  std::map<std::string, std::string> copied;

  for (Yolo4Image& image : annotations.images) {
    const auto done = copied.find(image.path);

    if (done != copied.end()) {
      image.path = done->second;
      continue;
    }

    const fs::path source{image.path};
    fs::path target;

    for (const fs::path& folder : folders) {
      if (inside(source, folder)) {
        target = directory / source.lexically_normal().lexically_relative(
                                 folder.lexically_normal());
        break;
      }
    }

    // the image of no unpacked project stays where it is
    if (target.empty()) {
      continue;
    }

    std::error_code failure;

    if (fs::exists(target, failure)) {
      if (!same_files(source, target)) {
        error = "the image " + target.string() +
                " of another content exists, the project's one isn't copied "
                "over it";
        return false;
      }
    } else {
      fs::create_directories(target.parent_path(), failure);

      if (!failure) {
        fs::copy_file(source, target, failure);
      }

      if (failure) {
        error = "the image " + source.string() + " can't be copied into " +
                target.string() + ": " + failure.message();
        return false;
      }
    }

    copied[image.path] = target.string();
    image.path = target.string();
  }

  return true;
}

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1
