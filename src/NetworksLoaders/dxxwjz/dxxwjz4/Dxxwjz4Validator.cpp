#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Validator.h"

#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Validator.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Parser.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

WeightsReport Dxxwjz4Validator::validate(const std::string& filename,
                                         const network* net)
{
  WeightsDocument document;

  return validate(filename, net, document);
}

WeightsReport Dxxwjz4Validator::validate(const std::string& filename,
                                         const network* net,
                                         WeightsDocument& document)
{
  WeightsReport report;

  if (Dxxwjz4Parser::parse_file(filename, document, report)) {
    report.merge(dxxwjz2::v2::Dxxwjz2Validator::validate(document, net));
  }

  return report;
}

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4
