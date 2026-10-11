#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pReader.h"

#include <memory>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pParser.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pValidator.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

Dxxwjz2pReader::Dxxwjz2pReaderPtr Dxxwjz2pReader::create()
{
  return std::make_shared<Dxxwjz2pReader>();
}

bool Dxxwjz2pReader::read(const std::string& filename, const network* net,
                          WeightsDocument& document)
{
  const WeightsReport report =
      Dxxwjz2pValidator::validate(filename, net, document);

  report.log(filename);

  if (!report.ok()) {
    CLOGE("Couldn't read the dxxwjz2p file: %s", filename.c_str());
    return false;
  }

  if (document.floats() == 0U) {
    CLOGI("The %s file holds no weights, it's the network to train",
          filename.c_str());
  }

  return true;
}

bool Dxxwjz2pReader::peek(const std::string& filename, WeightsDocument& rest,
                          bool& weights)
{
  WeightsReport report;

  const bool parsed = Dxxwjz2pParser::peek(filename, rest, weights, report);

  if (!parsed) {
    report.log(filename);
    CLOGE("Couldn't read the dxxwjz2p file: %s", filename.c_str());
  }

  return parsed;
}

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1
