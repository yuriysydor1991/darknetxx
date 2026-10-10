#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pValidator.h"

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Validator.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pParser.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

WeightsReport Dxxwjz2pValidator::validate(const std::string& filename,
                                          const network* net)
{
  WeightsDocument document;

  return validate(filename, net, document);
}

WeightsReport Dxxwjz2pValidator::validate(const std::string& filename,
                                          const network* net,
                                          WeightsDocument& document)
{
  WeightsReport report;

  if (Dxxwjz2pParser::parse_file(filename, document, report)) {
    report.merge(dxxwjz2::v2::Dxxwjz2Validator::validate(document, net));
  }

  return report;
}

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1
