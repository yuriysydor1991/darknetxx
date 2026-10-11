#include "ATrainingProject.h"

#include <regex>
#include <system_error>
#include <utility>

#include "src/NetworksLoaders/weights-formats/NetworkSource.h"
#include "src/log/log.h"

namespace darknetxx011
{

namespace fs = std::filesystem;

using darknet_adaptor::loaders::NetworkSource;

ATrainingProject::ATrainingProject(fs::path nroot) : root{std::move(nroot)} {}

bool ATrainingProject::valid() const { return !cfgFile.empty(); }

const fs::path& ATrainingProject::path() const { return root; }

const std::string& ATrainingProject::cfg() const { return cfgFile; }

const std::string& ATrainingProject::latest_weights() const
{
  return weightsFile;
}

std::string ATrainingProject::chart() const
{
  // the chart_<base>_<YYYY-MM-DD--HH-MM-SS>.png of every training start
  static const std::regex started{
      R"(_\d{4}-\d{2}-\d{2}--\d{2}-\d{2}-\d{2}\.png)"};
  const std::string prefix = "chart_" + NetworkSource::base_of(cfgFile, 0);
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

bool ATrainingProject::enter(LibraryContext& ctx) const
{
  std::error_code error;

  fs::current_path(root, error);

  if (error) {
    LOGE("Fail to enter the folder " << root << ": " << error.message());
    return false;
  }

  // the training ends the process when unable to save the weights
  fs::create_directories(
      root / NetworkSource::option_of(cfgFile, "backup", "backup"), error);

  if (error) {
    LOGE("Fail to create the backup folder: " << error.message());
    return false;
  }

  ctx.detect_cfg.clear();
  ctx.train_cfg = cfgFile;
  ctx.latest_weights = weightsFile;
  ctx.names = names();
  ctx.chart_path = chart();

  return true;
}

fs::path ATrainingProject::absolute_of(const std::string& path)
{
  std::error_code error;

  return path.empty() ? fs::path{} : fs::absolute(path, error);
}

void ATrainingProject::train(std::string ncfg, std::string nweights)
{
  cfgFile = std::move(ncfg);
  weightsFile = std::move(nweights);
}

}  // namespace darknetxx011
