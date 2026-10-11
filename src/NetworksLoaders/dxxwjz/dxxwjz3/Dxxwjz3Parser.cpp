#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3Parser.h"

#include <istream>
#include <memory>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Parser.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Parser.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3SaxHandler.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

using dxxwjz1::v1::Dxxwjz1Parser;

namespace
{

/// @brief The handlers of the file, the ones of the peek or the parse.
Dxxwjz1Parser::HandlerFactory handlers(const bool peek)
{
  return [peek](WeightsDocument& parsed, WeightsReport& findings) {
    return std::make_unique<Dxxwjz3SaxHandler>(parsed, findings, peek);
  };
}

}  // namespace

bool Dxxwjz3Parser::parse(std::istream& json, WeightsDocument& document,
                          WeightsReport& report)
{
  document = WeightsDocument{};

  Dxxwjz3SaxHandler handler{document, report};

  return Dxxwjz1Parser::parse(json, handler, document, report);
}

bool Dxxwjz3Parser::parse_file(const std::string& filename,
                               WeightsDocument& document, WeightsReport& report)
{
  return Dxxwjz1Parser::parse_file(filename, handlers(false), document, report);
}

bool Dxxwjz3Parser::peek(const std::string& filename, WeightsDocument& rest,
                         bool& weights, WeightsReport& report)
{
  return dxxwjz2::v2::Dxxwjz2Parser::peek(filename, handlers(true), rest,
                                          weights, report);
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
