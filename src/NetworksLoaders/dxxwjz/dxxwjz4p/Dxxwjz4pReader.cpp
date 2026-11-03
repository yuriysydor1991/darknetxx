#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pReader.h"

#include <memory>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pParser.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pValidator.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

Dxxwjz4pReader::Dxxwjz4pReaderPtr Dxxwjz4pReader::create()
{
  return std::make_shared<Dxxwjz4pReader>();
}

bool Dxxwjz4pReader::read(const std::string& filename, const network* net,
                          WeightsDocument& document)
{
  const WeightsReport report =
      Dxxwjz4pValidator::validate(filename, net, document);

  report.log(filename);

  if (!report.ok()) {
    CLOGE("Couldn't read the dxxwjz4p project: %s", filename.c_str());
    return false;
  }

  if (document.floats() == 0U) {
    CLOGI("The %s file holds no weights, it's the network to train",
          filename.c_str());
  }

  return true;
}

bool Dxxwjz4pReader::peek(const std::string& filename, WeightsDocument& rest,
                          bool& weights)
{
  WeightsReport report;

  const bool parsed = Dxxwjz4pParser::peek(filename, rest, weights, report);

  if (!parsed) {
    report.log(filename);
    CLOGE("Couldn't read the dxxwjz4p project: %s", filename.c_str());
  }

  return parsed;
}

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1
