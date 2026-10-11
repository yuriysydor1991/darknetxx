#include "src/darknet-adaptor/adaptors/NetworksLoaders/yolo4-folder/Yolo4FolderLayout.h"

#include <algorithm>
#include <system_error>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/NetworkSource.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"

namespace darknet_adaptor::loaders::yolo4
{

namespace fs = std::filesystem;

std::string Yolo4FolderLayout::cfg(const fs::path& root)
{
  std::error_code error;
  fs::path cfg;

  // the first one by the name, since the directory order is the file system one
  for (const auto& entry : fs::directory_iterator{root / CFG_DIR, error}) {
    if (entry.path().extension() == ".cfg" &&
        (cfg.empty() || entry.path() < cfg)) {
      cfg = entry.path();
    }
  }

  return cfg.string();
}

std::string Yolo4FolderLayout::names(const fs::path& root)
{
  std::error_code error;

  return fs::exists(root / NAMES, error) ? (root / NAMES).string()
                                         : std::string{};
}

std::string Yolo4FolderLayout::latest_weights(const fs::path& root,
                                              const std::string& cfg)
{
  const std::string prefix = NetworkSource::base_of(cfg, 0) + "_";
  const std::vector<IWeightsFileFormatPtr>& formats = WeightsFileFormats::all();
  std::error_code error;
  fs::path latest;
  fs::file_time_type latestTime;
  bool latestLast = false;

  for (const auto& entry : fs::directory_iterator{
           root / NetworkSource::option_of(cfg, "backup", BACKUP), error}) {
    const fs::path& path = entry.path();
    const bool weights = std::any_of(
        formats.begin(), formats.end(), [&path](const auto& format) {
          return path.extension() == format->extension();
        });

    if (!weights || path.filename().string().rfind(prefix, 0) != 0) {
      continue;
    }

    // a stopped training saves these
    const bool last = path.stem().string() == prefix + "last";
    const auto time = entry.last_write_time(error);

    if (latest.empty() || (last && !latestLast) ||
        (last == latestLast && time > latestTime)) {
      latest = path;
      latestTime = time;
      latestLast = last;
    }
  }

  return latest.string();
}

}  // namespace darknet_adaptor::loaders::yolo4
