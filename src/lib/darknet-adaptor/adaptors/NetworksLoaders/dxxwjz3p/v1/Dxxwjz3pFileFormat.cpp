#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pFileFormat.h"

#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pFormat.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pWriter.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

const std::string& Dxxwjz3pFileFormat::name() const
{
  static const std::string text{Dxxwjz3pFormat::NAME};
  return text;
}

const std::string& Dxxwjz3pFileFormat::extension() const
{
  static const std::string text{Dxxwjz3pFormat::EXTENSION};
  return text;
}

const std::string& Dxxwjz3pFileFormat::description() const
{
  static const std::string text{
      "the darknetxx weights JSON zipped project file of the third version "
      "(*.dxxwjz3p), the dxxwjz3 one of the YOLO4 training project, which "
      "keeps the YOLO4 annotations of the images too"};
  return text;
}

bool Dxxwjz3pFileFormat::keeps_structure() const { return true; }

bool Dxxwjz3pFileFormat::keeps_network() const { return true; }

IWeightsReaderPtr Dxxwjz3pFileFormat::create_reader() const
{
  return Dxxwjz3pReader::create();
}

IWeightsWriterPtr Dxxwjz3pFileFormat::create_writer() const
{
  return Dxxwjz3pWriter::create();
}

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1
