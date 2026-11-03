#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_FILE_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_FILE_FORMAT_CLASS_H

#include <string>

#include "src/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

/// @brief The dxxwjz4p project file format of the first version (see the
/// Dxxwjz4pFormat).
class Dxxwjz4pFileFormat : public IWeightsFileFormat
{
 public:
  virtual ~Dxxwjz4pFileFormat() = default;
  Dxxwjz4pFileFormat() = default;

  virtual const std::string& name() const override;
  virtual const std::string& extension() const override;
  virtual const std::string& description() const override;
  virtual bool keeps_structure() const override;
  virtual bool keeps_network() const override;
  virtual bool keeps_annotations() const override;
  virtual bool keeps_images() const override;
  virtual bool keeps_description() const override;
  virtual IWeightsReaderPtr create_reader() const override;
  virtual IWeightsWriterPtr create_writer() const override;
};

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_FILE_FORMAT_CLASS_H
