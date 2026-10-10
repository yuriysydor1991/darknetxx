#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pParser.h"

#include <filesystem>
#include <istream>
#include <memory>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Parser.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Parser.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pParser.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pSaxHandler.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

using dxxwjz1::v1::Dxxwjz1Parser;

namespace
{

/// @brief The handlers of the file, the ones of the peek or the parse.
Dxxwjz1Parser::HandlerFactory handlers(const std::string& filename,
                                       const bool peek)
{
  return [directory = dxxwjz2p::v1::Dxxwjz2pParser::directory_of(filename),
          peek](WeightsDocument& parsed, WeightsReport& findings) {
    return std::make_unique<Dxxwjz3pSaxHandler>(parsed, findings, directory,
                                                peek);
  };
}

}  // namespace

bool Dxxwjz3pParser::parse(std::istream& json, WeightsDocument& document,
                           WeightsReport& report,
                           const std::filesystem::path& directory)
{
  document = WeightsDocument{};

  Dxxwjz3pSaxHandler handler{document, report, directory};

  return Dxxwjz1Parser::parse(json, handler, document, report);
}

bool Dxxwjz3pParser::parse_file(const std::string& filename,
                                WeightsDocument& document,
                                WeightsReport& report)
{
  return Dxxwjz1Parser::parse_file(filename, handlers(filename, false),
                                   document, report);
}

bool Dxxwjz3pParser::peek(const std::string& filename, WeightsDocument& rest,
                          bool& weights, WeightsReport& report)
{
  return dxxwjz2::v2::Dxxwjz2Parser::peek(filename, handlers(filename, true),
                                          rest, weights, report);
}

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1
