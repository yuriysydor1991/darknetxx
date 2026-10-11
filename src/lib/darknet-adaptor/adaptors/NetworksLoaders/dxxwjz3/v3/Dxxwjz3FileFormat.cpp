#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3FileFormat.h"

#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Format.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Reader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Writer.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

const std::string& Dxxwjz3FileFormat::name() const
{
  static const std::string text{Dxxwjz3Format::NAME};
  return text;
}

const std::string& Dxxwjz3FileFormat::extension() const
{
  static const std::string text{Dxxwjz3Format::EXTENSION};
  return text;
}

const std::string& Dxxwjz3FileFormat::description() const
{
  static const std::string text{
      "the darknetxx weights JSON zipped file of the third version "
      "(*.dxxwjz3), the dxxwjz2 one of nothing but the JSON data: the "
      "network cfg of the JSON options of its sections, which keep the "
      "arrays of their layers"};
  return text;
}

bool Dxxwjz3FileFormat::keeps_structure() const { return true; }

bool Dxxwjz3FileFormat::keeps_network() const { return true; }

IWeightsReaderPtr Dxxwjz3FileFormat::create_reader() const
{
  return Dxxwjz3Reader::create();
}

IWeightsWriterPtr Dxxwjz3FileFormat::create_writer() const
{
  return Dxxwjz3Writer::create();
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
