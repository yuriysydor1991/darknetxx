#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/yolo4-folder/Yolo4FolderFormat.h"

#include <memory>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/yolo4-folder/Yolo4FolderReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/yolo4-folder/Yolo4FolderWriter.h"

namespace darknet_adaptor::loaders::yolo4
{

const std::string& Yolo4FolderFormat::name() const
{
  static const std::string text{"yolo4"};
  return text;
}

const std::string& Yolo4FolderFormat::extension() const
{
  static const std::string text;
  return text;
}

const std::string& Yolo4FolderFormat::description() const
{
  static const std::string text{
      "the YOLO4 training folder (the conversion alone): the cfg/*.cfg "
      "network, the data/obj.names classes, the image lists and labels and "
      "the latest backup/ weights of the dxxwjz2p project, the images copied "
      "into it's data/obj/ folder"};
  return text;
}

bool Yolo4FolderFormat::keeps_structure() const { return true; }

bool Yolo4FolderFormat::keeps_network() const { return true; }

IWeightsReaderPtr Yolo4FolderFormat::create_reader() const
{
  return std::make_shared<Yolo4FolderReader>();
}

IWeightsWriterPtr Yolo4FolderFormat::create_writer() const
{
  return std::make_shared<Yolo4FolderWriter>();
}

}  // namespace darknet_adaptor::loaders::yolo4
