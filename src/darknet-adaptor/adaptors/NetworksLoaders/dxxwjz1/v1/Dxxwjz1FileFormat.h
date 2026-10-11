#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_FILE_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_FILE_FORMAT_CLASS_H

#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

/// @brief The dxxwjz1 file format of the first version (see the
/// Dxxwjz1Format).
class Dxxwjz1FileFormat : public IWeightsFileFormat
{
 public:
  virtual ~Dxxwjz1FileFormat() = default;
  Dxxwjz1FileFormat() = default;

  virtual const std::string& name() const override;
  virtual const std::string& extension() const override;
  virtual const std::string& description() const override;
  virtual bool keeps_structure() const override;
  virtual bool keeps_network() const override;
  virtual IWeightsReaderPtr create_reader() const override;
  virtual IWeightsWriterPtr create_writer() const override;
};

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_FILE_FORMAT_CLASS_H
