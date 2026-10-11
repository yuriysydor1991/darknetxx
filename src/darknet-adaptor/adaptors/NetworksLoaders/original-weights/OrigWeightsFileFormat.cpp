#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsFileFormat.h"

#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h"

namespace darknet_adaptor::loaders
{

const std::string& OrigWeightsFileFormat::name() const
{
  static const std::string text{"weights"};
  return text;
}

const std::string& OrigWeightsFileFormat::extension() const
{
  static const std::string text{".weights"};
  return text;
}

const std::string& OrigWeightsFileFormat::description() const
{
  static const std::string text{
      "the original darknet weights file (*.weights), which requires the "
      "network cfg file"};
  return text;
}

bool OrigWeightsFileFormat::keeps_structure() const { return false; }

bool OrigWeightsFileFormat::keeps_network() const { return false; }

IWeightsReaderPtr OrigWeightsFileFormat::create_reader() const
{
  return OrigWeightsReader::create();
}

IWeightsWriterPtr OrigWeightsFileFormat::create_writer() const
{
  return OrigWeightsWriter::create();
}

}  // namespace darknet_adaptor::loaders
