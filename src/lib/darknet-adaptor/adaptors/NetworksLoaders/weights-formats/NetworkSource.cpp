#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/NetworkSource.h"

#include <filesystem>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkFiles.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/log/log.h"

namespace darknet_adaptor::loaders
{

bool NetworkSource::describes(const std::string& path)
{
  return WeightsFileFormats::of_file(path)->keeps_network();
}

bool NetworkSource::read(const std::string& path,
                         NetworkDescription& description)
{
  if (describes(path)) {
    WeightsDocument rest;
    bool weights = false;

    if (!WeightsFileFormats::of_file(path)->create_reader()->peek(path, rest,
                                                                  weights)) {
      return false;
    }

    description = rest.network;
    return true;
  }

  std::string text;

  if (!NetworkFiles::read_text(path, text)) {
    LOGE("Couldn't open the network cfg file: " << path);
    return false;
  }

  description = NetworkDescription{text, {}};

  return true;
}

std::string NetworkSource::base_of(const std::string& path, const int iteration)
{
  const std::string name = std::filesystem::path{path}.filename().string();
  const std::string base = name.substr(0, name.find('.'));

  if (!describes(path)) {
    return base;
  }

  for (const std::string& suffix :
       {std::string{"_last"}, std::string{"_final"}, std::string{"_best"},
        std::string{"_ema"}, "_" + std::to_string(iteration)}) {
    if (base.size() > suffix.size() &&
        base.compare(base.size() - suffix.size(), suffix.size(), suffix) == 0) {
      return base.substr(0, base.size() - suffix.size());
    }
  }

  return base;
}

}  // namespace darknet_adaptor::loaders
