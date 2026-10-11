#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2FileFormat.h"

#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Format.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Reader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Writer.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

const std::string& Dxxwjz2FileFormat::name() const
{
  static const std::string text{Dxxwjz2Format::NAME};
  return text;
}

const std::string& Dxxwjz2FileFormat::extension() const
{
  static const std::string text{Dxxwjz2Format::EXTENSION};
  return text;
}

const std::string& Dxxwjz2FileFormat::description() const
{
  static const std::string text{
      "the darknetxx weights JSON zipped file of the second version "
      "(*.dxxwjz2), which keeps the network cfg, the class names and the "
      "training record too"};
  return text;
}

bool Dxxwjz2FileFormat::keeps_structure() const { return true; }

bool Dxxwjz2FileFormat::keeps_network() const { return true; }

IWeightsReaderPtr Dxxwjz2FileFormat::create_reader() const
{
  return Dxxwjz2Reader::create();
}

IWeightsWriterPtr Dxxwjz2FileFormat::create_writer() const
{
  return Dxxwjz2Writer::create();
}

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2
