#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pFileFormat.h"

#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pFormat.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pReader.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pWriter.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

const std::string& Dxxwjz4pFileFormat::name() const
{
  static const std::string text{Dxxwjz4pFormat::NAME};
  return text;
}

const std::string& Dxxwjz4pFileFormat::extension() const
{
  static const std::string text{Dxxwjz4pFormat::EXTENSION};
  return text;
}

const std::string& Dxxwjz4pFileFormat::description() const
{
  static const std::string text{
      "the darknetxx weights JSON zipped project file of the fourth version "
      "(*.dxxwjz4p), the whole training project in one file: the ZIP "
      "archive of the single JSON document of the network, the annotations "
      "and the description text and of the images of the annotations in "
      "it's data folder"};
  return text;
}

bool Dxxwjz4pFileFormat::keeps_structure() const { return true; }

bool Dxxwjz4pFileFormat::keeps_network() const { return true; }

bool Dxxwjz4pFileFormat::keeps_annotations() const { return true; }

bool Dxxwjz4pFileFormat::keeps_images() const { return true; }

bool Dxxwjz4pFileFormat::keeps_description() const { return true; }

IWeightsReaderPtr Dxxwjz4pFileFormat::create_reader() const
{
  return Dxxwjz4pReader::create();
}

IWeightsWriterPtr Dxxwjz4pFileFormat::create_writer() const
{
  return Dxxwjz4pWriter::create();
}

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1
