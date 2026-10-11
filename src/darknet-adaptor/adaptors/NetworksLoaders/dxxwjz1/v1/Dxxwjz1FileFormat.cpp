#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1FileFormat.h"

#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Format.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Reader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Writer.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

const std::string& Dxxwjz1FileFormat::name() const
{
  static const std::string text{Dxxwjz1Format::NAME};
  return text;
}

const std::string& Dxxwjz1FileFormat::extension() const
{
  static const std::string text{Dxxwjz1Format::EXTENSION};
  return text;
}

const std::string& Dxxwjz1FileFormat::description() const
{
  static const std::string text{
      "the darknetxx weights JSON zipped file of the first version "
      "(*.dxxwjz1)"};
  return text;
}

bool Dxxwjz1FileFormat::keeps_structure() const { return true; }

bool Dxxwjz1FileFormat::keeps_network() const { return false; }

IWeightsReaderPtr Dxxwjz1FileFormat::create_reader() const
{
  return Dxxwjz1Reader::create();
}

IWeightsWriterPtr Dxxwjz1FileFormat::create_writer() const
{
  return Dxxwjz1Writer::create();
}

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1
