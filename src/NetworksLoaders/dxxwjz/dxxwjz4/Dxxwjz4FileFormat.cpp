#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4FileFormat.h"

#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Format.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Reader.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Writer.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

const std::string& Dxxwjz4FileFormat::name() const
{
  static const std::string text{Dxxwjz4Format::NAME};
  return text;
}

const std::string& Dxxwjz4FileFormat::extension() const
{
  static const std::string text{Dxxwjz4Format::EXTENSION};
  return text;
}

const std::string& Dxxwjz4FileFormat::description() const
{
  static const std::string text{
      "the darknetxx weights JSON zipped file of the fourth version "
      "(*.dxxwjz4), the dxxwjz3 one of the description text of any length, "
      "the network the training of the dxxwjz4p project saves"};
  return text;
}

bool Dxxwjz4FileFormat::keeps_structure() const { return true; }

bool Dxxwjz4FileFormat::keeps_network() const { return true; }

bool Dxxwjz4FileFormat::keeps_description() const { return true; }

IWeightsReaderPtr Dxxwjz4FileFormat::create_reader() const
{
  return Dxxwjz4Reader::create();
}

IWeightsWriterPtr Dxxwjz4FileFormat::create_writer() const
{
  return Dxxwjz4Writer::create();
}

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4
