#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_FILE_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_FILE_FORMAT_CLASS_H

#include <string>

#include "src/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

/// @brief The dxxwjz4 network weights file format of the fourth version (see
/// the Dxxwjz4Format).
class Dxxwjz4FileFormat : public IWeightsFileFormat
{
 public:
  virtual ~Dxxwjz4FileFormat() = default;
  Dxxwjz4FileFormat() = default;

  virtual const std::string& name() const override;
  virtual const std::string& extension() const override;
  virtual const std::string& description() const override;
  virtual bool keeps_structure() const override;
  virtual bool keeps_network() const override;
  virtual bool keeps_description() const override;
  virtual IWeightsReaderPtr create_reader() const override;
  virtual IWeightsWriterPtr create_writer() const override;
};

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_FILE_FORMAT_CLASS_H
