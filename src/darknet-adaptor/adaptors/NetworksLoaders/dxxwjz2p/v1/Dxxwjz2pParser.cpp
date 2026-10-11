#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pParser.h"

#include <memory>
#include <system_error>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Parser.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Parser.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pSaxHandler.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

using dxxwjz1::v1::Dxxwjz1Parser;

namespace
{

/// @brief The handlers of the file, the ones of the peek or the parse.
Dxxwjz1Parser::HandlerFactory handlers(const std::string& filename,
                                       const bool peek)
{
  return [directory = Dxxwjz2pParser::directory_of(filename), peek](
             WeightsDocument& parsed, WeightsReport& findings) {
    return std::make_unique<Dxxwjz2pSaxHandler>(parsed, findings, directory,
                                                peek);
  };
}

}  // namespace

bool Dxxwjz2pParser::parse(std::istream& json, WeightsDocument& document,
                           WeightsReport& report,
                           const std::filesystem::path& directory)
{
  document = WeightsDocument{};

  Dxxwjz2pSaxHandler handler{document, report, directory};

  return Dxxwjz1Parser::parse(json, handler, document, report);
}

bool Dxxwjz2pParser::parse_file(const std::string& filename,
                                WeightsDocument& document,
                                WeightsReport& report)
{
  return Dxxwjz1Parser::parse_file(filename, handlers(filename, false),
                                   document, report);
}

bool Dxxwjz2pParser::peek(const std::string& filename, WeightsDocument& rest,
                          bool& weights, WeightsReport& report)
{
  return dxxwjz2::v2::Dxxwjz2Parser::peek(filename, handlers(filename, true),
                                          rest, weights, report);
}

std::filesystem::path Dxxwjz2pParser::directory_of(const std::string& filename)
{
  std::error_code error;

  return std::filesystem::absolute(filename, error).parent_path();
}

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1
