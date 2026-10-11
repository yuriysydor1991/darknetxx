#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pReader.h"

#include <memory>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pParser.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pValidator.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

Dxxwjz3pReader::Dxxwjz3pReaderPtr Dxxwjz3pReader::create()
{
  return std::make_shared<Dxxwjz3pReader>();
}

bool Dxxwjz3pReader::read(const std::string& filename, const network* net,
                          WeightsDocument& document)
{
  const WeightsReport report =
      Dxxwjz3pValidator::validate(filename, net, document);

  report.log(filename);

  if (!report.ok()) {
    CLOGE("Couldn't read the dxxwjz3p file: %s", filename.c_str());
    return false;
  }

  if (document.floats() == 0U) {
    CLOGI("The %s file holds no weights, it's the network to train",
          filename.c_str());
  }

  return true;
}

bool Dxxwjz3pReader::peek(const std::string& filename, WeightsDocument& rest,
                          bool& weights)
{
  WeightsReport report;

  const bool parsed = Dxxwjz3pParser::peek(filename, rest, weights, report);

  if (!parsed) {
    report.log(filename);
    CLOGE("Couldn't read the dxxwjz3p file: %s", filename.c_str());
  }

  return parsed;
}

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1
