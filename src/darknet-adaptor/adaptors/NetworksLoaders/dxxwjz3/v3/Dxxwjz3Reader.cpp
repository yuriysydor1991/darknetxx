#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Reader.h"

#include <memory>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Parser.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Validator.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

Dxxwjz3Reader::Dxxwjz3ReaderPtr Dxxwjz3Reader::create()
{
  return std::make_shared<Dxxwjz3Reader>();
}

bool Dxxwjz3Reader::read(const std::string& filename, const network* net,
                         WeightsDocument& document)
{
  const WeightsReport report =
      Dxxwjz3Validator::validate(filename, net, document);

  report.log(filename);

  if (!report.ok()) {
    CLOGE("Couldn't read the dxxwjz3 file: %s", filename.c_str());
    return false;
  }

  if (document.floats() == 0U) {
    CLOGI("The %s file holds no weights, it's the network to train",
          filename.c_str());
  }

  return true;
}

bool Dxxwjz3Reader::peek(const std::string& filename, WeightsDocument& rest,
                         bool& weights)
{
  WeightsReport report;

  const bool parsed = Dxxwjz3Parser::peek(filename, rest, weights, report);

  if (!parsed) {
    report.log(filename);
    CLOGE("Couldn't read the dxxwjz3 file: %s", filename.c_str());
  }

  return parsed;
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
