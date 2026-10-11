#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pValidator.h"

#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Validator.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pParser.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

WeightsReport Dxxwjz3pValidator::validate(const std::string& filename,
                                          const network* net)
{
  WeightsDocument document;

  return validate(filename, net, document);
}

WeightsReport Dxxwjz3pValidator::validate(const std::string& filename,
                                          const network* net,
                                          WeightsDocument& document)
{
  WeightsReport report;

  if (Dxxwjz3pParser::parse_file(filename, document, report)) {
    report.merge(dxxwjz2::v2::Dxxwjz2Validator::validate(document, net));
  }

  return report;
}

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1
