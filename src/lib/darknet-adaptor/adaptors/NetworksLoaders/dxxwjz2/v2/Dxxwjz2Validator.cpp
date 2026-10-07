#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Validator.h"

#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Parser.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocumentValidator.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

WeightsReport Dxxwjz2Validator::validate(const std::string& filename,
                                         const network* net)
{
  WeightsDocument document;

  return validate(filename, net, document);
}

WeightsReport Dxxwjz2Validator::validate(const std::string& filename,
                                         const network* net,
                                         WeightsDocument& document)
{
  WeightsReport report;

  if (!Dxxwjz2Parser::parse_file(filename, document, report)) {
    return report;
  }

  report.merge(validate(document, net));

  return report;
}

WeightsReport Dxxwjz2Validator::validate(const WeightsDocument& document,
                                         const network* net)
{
  WeightsReport report;

  if (document.network.empty()) {
    report.error("the network description has no cfg text");
  }

  if (net == nullptr || document.floats() == 0U) {
    report.merge(WeightsDocumentValidator::validate(document));
    return report;
  }

  report.merge(WeightsDocumentValidator::validate(document, *net));

  check_classes(document, *net, report);

  return report;
}

void Dxxwjz2Validator::check_classes(const WeightsDocument& document,
                                     const network& net, WeightsReport& report)
{
  const std::size_t names = document.network.classes().size();
  const int classes = net.n > 0 ? net.layers[net.n - 1].classes : 0;

  if (names > 0U && classes > 0 && names != static_cast<std::size_t>(classes)) {
    report.warning("the " + std::to_string(names) +
                   " class names aren't the classes=" +
                   std::to_string(classes) + " of the network");
  }
}

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2
