#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pFileFormat.h"

#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pFormat.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pWriter.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

const std::string& Dxxwjz2pFileFormat::name() const
{
  static const std::string text{Dxxwjz2pFormat::NAME};
  return text;
}

const std::string& Dxxwjz2pFileFormat::extension() const
{
  static const std::string text{Dxxwjz2pFormat::EXTENSION};
  return text;
}

const std::string& Dxxwjz2pFileFormat::description() const
{
  static const std::string text{
      "the darknetxx weights JSON zipped project file (*.dxxwjz2p), the "
      "dxxwjz2 one of the YOLO4 training project, which keeps the YOLO4 "
      "annotations of the images too"};
  return text;
}

bool Dxxwjz2pFileFormat::keeps_structure() const { return true; }

bool Dxxwjz2pFileFormat::keeps_network() const { return true; }

IWeightsReaderPtr Dxxwjz2pFileFormat::create_reader() const
{
  return Dxxwjz2pReader::create();
}

IWeightsWriterPtr Dxxwjz2pFileFormat::create_writer() const
{
  return Dxxwjz2pWriter::create();
}

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1
