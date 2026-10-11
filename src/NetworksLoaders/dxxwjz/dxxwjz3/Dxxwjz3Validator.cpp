#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3Validator.h"

#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Validator.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3Parser.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

WeightsReport Dxxwjz3Validator::validate(const std::string& filename,
                                         const network* net)
{
  WeightsDocument document;

  return validate(filename, net, document);
}

WeightsReport Dxxwjz3Validator::validate(const std::string& filename,
                                         const network* net,
                                         WeightsDocument& document)
{
  WeightsReport report;

  if (Dxxwjz3Parser::parse_file(filename, document, report)) {
    report.merge(dxxwjz2::v2::Dxxwjz2Validator::validate(document, net));
  }

  return report;
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
