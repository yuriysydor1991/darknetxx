#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Reader.h"

#include <memory>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Parser.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Validator.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

Dxxwjz2Reader::Dxxwjz2ReaderPtr Dxxwjz2Reader::create()
{
  return std::make_shared<Dxxwjz2Reader>();
}

bool Dxxwjz2Reader::read(const std::string& filename, const network* net,
                         WeightsDocument& document)
{
  const WeightsReport report =
      Dxxwjz2Validator::validate(filename, net, document);

  report.log(filename);

  if (!report.ok()) {
    CLOGE("Couldn't read the dxxwjz2 file: %s", filename.c_str());
    return false;
  }

  if (document.floats() == 0U) {
    CLOGI("The %s file holds no weights, it's the network to train",
          filename.c_str());
  }

  return true;
}

bool Dxxwjz2Reader::network_to_train(WeightsDocument& document) const
{
  if (!document.network_alone()) {
    return false;
  }

  document.training.clear();

  return true;
}

bool Dxxwjz2Reader::peek(const std::string& filename, WeightsDocument& rest,
                         bool& weights)
{
  WeightsReport report;

  const bool parsed = Dxxwjz2Parser::peek(filename, rest, weights, report);

  if (!parsed) {
    report.log(filename);
    CLOGE("Couldn't read the dxxwjz2 file: %s", filename.c_str());
  }

  return parsed;
}

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2
