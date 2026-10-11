#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Validator.h"

#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Parser.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsDocumentValidator.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

WeightsReport Dxxwjz1Validator::validate(const std::string& filename,
                                         const network* net)
{
  WeightsDocument document;

  return validate(filename, net, document);
}

WeightsReport Dxxwjz1Validator::validate(const std::string& filename,
                                         const network* net,
                                         WeightsDocument& document)
{
  WeightsReport report;

  if (!Dxxwjz1Parser::parse_file(filename, document, report)) {
    return report;
  }

  report.merge(validate(document, net));

  return report;
}

WeightsReport Dxxwjz1Validator::validate(const WeightsDocument& document,
                                         const network* net)
{
  return net == nullptr ? WeightsDocumentValidator::validate(document)
                        : WeightsDocumentValidator::validate(document, *net);
}

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1
