#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Parser.h"

#include <algorithm>
#include <istream>
#include <memory>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Parser.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2SaxHandler.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

using dxxwjz1::v1::Dxxwjz1Parser;

bool Dxxwjz2Parser::parse(std::istream& json, WeightsDocument& document,
                          WeightsReport& report)
{
  document = WeightsDocument{};

  Dxxwjz2SaxHandler handler{document, report};

  return Dxxwjz1Parser::parse(json, handler, document, report);
}

bool Dxxwjz2Parser::parse_file(const std::string& filename,
                               WeightsDocument& document, WeightsReport& report)
{
  return Dxxwjz1Parser::parse_file(
      filename,
      [](WeightsDocument& parsed, WeightsReport& findings) {
        return std::make_unique<Dxxwjz2SaxHandler>(parsed, findings);
      },
      document, report);
}

bool Dxxwjz2Parser::peek(const std::string& filename, WeightsDocument& rest,
                         bool& weights, WeightsReport& report)
{
  return peek(
      filename,
      [](WeightsDocument& parsed_rest, WeightsReport& findings) {
        return std::make_unique<Dxxwjz2SaxHandler>(parsed_rest, findings, true);
      },
      rest, weights, report);
}

bool Dxxwjz2Parser::peek(const std::string& filename,
                         const Dxxwjz1Parser::HandlerFactory& factory,
                         WeightsDocument& rest, bool& weights,
                         WeightsReport& report)
{
  const bool parsed =
      Dxxwjz1Parser::parse_file(filename, factory, rest, report);

  // the peek stops at the first weight of the layers
  weights = rest.floats() > 0U;

  rest.layers.clear();
  rest.trailing.clear();

  return parsed;
}

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2
