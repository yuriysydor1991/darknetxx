#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_FILE_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_FILE_FORMAT_CLASS_H

#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/// @brief The dxxwjz3 file format of the third version (see the
/// Dxxwjz3Format).
class Dxxwjz3FileFormat : public IWeightsFileFormat
{
 public:
  virtual ~Dxxwjz3FileFormat() = default;
  Dxxwjz3FileFormat() = default;

  virtual const std::string& name() const override;
  virtual const std::string& extension() const override;
  virtual const std::string& description() const override;
  virtual bool keeps_structure() const override;
  virtual bool keeps_network() const override;
  virtual IWeightsReaderPtr create_reader() const override;
  virtual IWeightsWriterPtr create_writer() const override;
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_FILE_FORMAT_CLASS_H
