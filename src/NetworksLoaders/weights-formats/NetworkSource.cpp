#include "src/NetworksLoaders/weights-formats/NetworkSource.h"

#include <exception>
#include <filesystem>
#include <regex>
#include <sstream>
#include <string>

#include "src/NetworksLoaders/OrigDefaultLoader/CfgSection.h"
#include "src/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/NetworksLoaders/weights-model/NetworkFiles.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
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
  WeightsDocument rest;

  if (!read(path, rest)) {
    return false;
  }

  description = rest.network;

  return true;
}

bool NetworkSource::read(const std::string& path, WeightsDocument& rest)
{
  if (describes(path)) {
    bool weights = false;

    return WeightsFileFormats::of_file(path)->create_reader()->peek(path, rest,
                                                                    weights);
  }

  std::string text;

  if (!NetworkFiles::read_text(path, text)) {
    LOGE("Couldn't open the network cfg file: " << path);
    return false;
  }

  rest.network = NetworkDescription{text, {}};

  return true;
}

std::string NetworkSource::base_of(const std::string& path, const int iteration)
{
  // the suffix goes before the cut at the first dot, which the average loss
  // of the numbered backup has
  const std::regex backup{"(.+)_(last|final|best|ema|" +
                          std::to_string(iteration) + "(_[^_]+)?)"};
  const std::string stem = std::filesystem::path{path}.stem().string();
  std::smatch match;
  const std::string base =
      describes(path) && std::regex_match(stem, match, backup) ? match.str(1)
                                                               : stem;

  return base.substr(0, base.find('.'));
}

std::string NetworkSource::option_of(const std::string& path,
                                     const std::string& key,
                                     const std::string& def)
{
  NetworkDescription description;

  if (!read(path, description)) {
    return def;
  }

  std::istringstream text{description.cfg()};

  try {
    CfgSection::CfgSections sections = CfgSection::read_cfg(text, path);

    // the [net] section is the first one
    return sections.empty() ? def
                            : sections.front().find_str_quiet(key, def.c_str());
  }
  catch (const std::exception& e) {
    LOGE("Couldn't read the [net] section of " << path << ": " << e.what());
    return def;
  }
}

}  // namespace darknet_adaptor::loaders
