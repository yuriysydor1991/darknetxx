#ifndef DARKNET_CXX_PORT_PROJECT_YOLO4_FOLDER_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_YOLO4_FOLDER_FORMAT_CLASS_H

#include <string>

#include "src/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders::yolo4
{

/**
 * @brief The YOLO4 training folder (see the Yolo4FolderLayout) of the
 * conversion: the folder of the files of the dxxwjz2p project and of the
 * copies of it's images. It's of no file name extension, so it's no format of
 * the weights files of the names (see the WeightsFileFormats::convertible()).
 */
class Yolo4FolderFormat : public IWeightsFileFormat
{
 public:
  virtual ~Yolo4FolderFormat() = default;
  Yolo4FolderFormat() = default;

  virtual const std::string& name() const override;
  virtual const std::string& extension() const override;
  virtual const std::string& description() const override;
  virtual bool keeps_structure() const override;
  virtual bool keeps_network() const override;
  virtual IWeightsReaderPtr create_reader() const override;
  virtual IWeightsWriterPtr create_writer() const override;
};

}  // namespace darknet_adaptor::loaders::yolo4

#endif  // DARKNET_CXX_PORT_PROJECT_YOLO4_FOLDER_FORMAT_CLASS_H
