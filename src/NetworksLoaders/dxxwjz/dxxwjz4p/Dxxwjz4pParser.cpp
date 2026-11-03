#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pParser.h"

#include <filesystem>
#include <fstream>
#include <istream>
#include <string>
#include <system_error>

#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Parser.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pFormat.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pSaxHandler.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pWorkspace.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

namespace fs = std::filesystem;

using dxxwjz1::v1::Dxxwjz1Parser;

bool Dxxwjz4pParser::parse(std::istream& json, WeightsDocument& document,
                           WeightsReport& report, const fs::path& directory)
{
  document = WeightsDocument{};

  Dxxwjz4pSaxHandler handler{document, report, directory};

  return Dxxwjz1Parser::parse(json, handler, document, report);
}

bool Dxxwjz4pParser::parse_file(const std::string& filename,
                                WeightsDocument& document,
                                WeightsReport& report)
{
  return read(filename, false, document, report);
}

bool Dxxwjz4pParser::peek(const std::string& filename, WeightsDocument& rest,
                          bool& weights, WeightsReport& report)
{
  const bool parsed = read(filename, true, rest, report);

  // the peek stops at the first weight of the layers
  weights = rest.floats() > 0U;
  rest.layers.clear();
  rest.trailing.clear();

  return parsed;
}

bool Dxxwjz4pParser::read(const std::string& filename, const bool peek,
                          WeightsDocument& document, WeightsReport& report)
{
  document = WeightsDocument{};

  const fs::path directory = Dxxwjz4pWorkspace::of(filename, report);

  if (directory.empty()) {
    return false;
  }

  std::ifstream json{directory / Dxxwjz4pFormat::PROJECT_MEMBER,
                     std::ios::binary};

  if (!json) {
    report.error("the " + std::string{Dxxwjz4pFormat::PROJECT_MEMBER} +
                 " member of the project can't be read");
    return false;
  }

  Dxxwjz4pSaxHandler handler{document, report, directory, peek};

  if (!Dxxwjz1Parser::parse(json, handler, document, report)) {
    return false;
  }

  if (!contained(document, directory, report)) {
    document = WeightsDocument{};
    return false;
  }

  return true;
}

bool Dxxwjz4pParser::contained(const WeightsDocument& document,
                               const fs::path& directory, WeightsReport& report)
{
  const fs::path root = directory.lexically_normal();

  for (const Yolo4Image& image : document.annotations.images) {
    const fs::path path = fs::path{image.path}.lexically_normal();
    const fs::path relative = path.lexically_relative(root);
    std::error_code error;

    if (relative.empty() || *relative.begin() == "..") {
      report.error("the image " + image.path +
                   " isn't the one of the project, the dxxwjz4p project "
                   "keeps it's images");
      return false;
    }

    if (!fs::is_regular_file(path, error)) {
      report.error("the image " + relative.generic_string() +
                   " of the annotations isn't in the project");
      return false;
    }
  }

  return true;
}

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1
