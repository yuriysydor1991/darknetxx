#include "Yolo4Folder.h"

#include <exception>
#include <regex>
#include <system_error>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/CfgSection.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/log/log.h"

namespace darknetxx011
{

namespace fs = std::filesystem;

Yolo4Folder::Yolo4Folder(const std::string& npath)
    : root{npath.empty() ? fs::path{} : fs::absolute(npath)},
      cfgFile{find_cfg(root / "cfg")}
{
  if (valid()) {
    weightsFile = find_latest_weights();
  }
}

bool Yolo4Folder::valid() const { return !cfgFile.empty(); }

const fs::path& Yolo4Folder::path() const { return root; }

const std::string& Yolo4Folder::cfg() const { return cfgFile; }

const std::string& Yolo4Folder::latest_weights() const { return weightsFile; }

std::string Yolo4Folder::chart() const
{
  // the chart_<base>_<YYYY-MM-DD--HH-MM-SS>.png of every training start
  static const std::regex started{
      R"(_\d{4}-\d{2}-\d{2}--\d{2}-\d{2}-\d{2}\.png)"};
  const std::string prefix = "chart_" + base();
  std::error_code error;
  std::string newest;

  // the chart of the latest training start sorts the last by it's name
  for (const auto& entry : fs::directory_iterator{root, error}) {
    const std::string name = entry.path().filename().string();

    const bool of_base =
        name == prefix + ".png" ||
        (name.rfind(prefix, 0) == 0 &&
         std::regex_match(name.substr(prefix.size()), started));

    if (of_base && entry.path().string() > newest) {
      newest = entry.path().string();
    }
  }

  return newest;
}

bool Yolo4Folder::enter(LibraryContext& ctx) const
{
  std::error_code error;

  fs::current_path(root, error);

  if (error) {
    LOGE("Fail to enter the folder " << root << ": " << error.message());
    return false;
  }

  // the training ends the process when unable to save the weights
  fs::create_directories(backup_dir(), error);

  if (error) {
    LOGE("Fail to create the backup folder: " << error.message());
    return false;
  }

  const fs::path names = root / "data" / "obj.names";

  ctx.detect_cfg.clear();
  ctx.train_cfg = cfgFile;
  ctx.latest_weights = weightsFile;
  ctx.names = fs::exists(names, error) ? names.string() : std::string{};
  ctx.chart_path = chart();

  return true;
}

std::string Yolo4Folder::base() const
{
  const std::string name = fs::path{cfgFile}.filename().string();

  return name.substr(0, name.find('.'));
}

std::string Yolo4Folder::find_cfg(const fs::path& cfgDir)
{
  std::error_code error;
  fs::path cfg;

  // the first one by the name, since the directory order is the file system one
  for (const auto& entry : fs::directory_iterator{cfgDir, error}) {
    if (entry.path().extension() == ".cfg" &&
        (cfg.empty() || entry.path() < cfg)) {
      cfg = entry.path();
    }
  }

  return cfg.string();
}

fs::path Yolo4Folder::backup_dir() const
{
  try {
    auto sections = darknet_adaptor::loaders::CfgSection::read_cfg(cfgFile);

    if (!sections.empty()) {
      return root / sections.front().find_str_quiet("backup", "backup");
    }
  }
  catch (const std::exception& e) {
    LOGE("Fail to read the backup directory of " << cfgFile << ": "
                                                 << e.what());
  }

  return root / "backup";
}

std::string Yolo4Folder::find_latest_weights() const
{
  const std::string prefix = base() + "_";
  std::error_code error;
  fs::path latest;
  fs::file_time_type latestTime;
  bool latestLast = false;

  for (const auto& entry : fs::directory_iterator{backup_dir(), error}) {
    const fs::path& path = entry.path();

    if (!is_weights(path) || path.filename().string().rfind(prefix, 0) != 0) {
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

bool Yolo4Folder::is_weights(const fs::path& path)
{
  using darknet_adaptor::loaders::WeightsFileFormats;

  for (const auto& format : WeightsFileFormats::all()) {
    if (path.extension() == format->extension()) {
      return true;
    }
  }

  return false;
}

}  // namespace darknetxx011
