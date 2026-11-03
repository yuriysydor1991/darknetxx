#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Reader.h"

#include <memory>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Parser.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Validator.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

Dxxwjz4Reader::Dxxwjz4ReaderPtr Dxxwjz4Reader::create()
{
  return std::make_shared<Dxxwjz4Reader>();
}

bool Dxxwjz4Reader::read(const std::string& filename, const network* net,
                         WeightsDocument& document)
{
  const WeightsReport report =
      Dxxwjz4Validator::validate(filename, net, document);

  report.log(filename);

  if (!report.ok()) {
    CLOGE("Couldn't read the dxxwjz4 file: %s", filename.c_str());
    return false;
  }

  if (document.floats() == 0U) {
    CLOGI("The %s file holds no weights, it's the network to train",
          filename.c_str());
  }

  return true;
}

bool Dxxwjz4Reader::peek(const std::string& filename, WeightsDocument& rest,
                         bool& weights)
{
  WeightsReport report;

  const bool parsed = Dxxwjz4Parser::peek(filename, rest, weights, report);

  if (!parsed) {
    report.log(filename);
    CLOGE("Couldn't read the dxxwjz4 file: %s", filename.c_str());
  }

  return parsed;
}

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4
