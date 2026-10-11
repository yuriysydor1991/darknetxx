#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_FILE_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_FILE_FORMAT_CLASS_H

#include <string>

#include "src/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

/// @brief The dxxwjz2 file format of the second version (see the
/// Dxxwjz2Format).
class Dxxwjz2FileFormat : public IWeightsFileFormat
{
 public:
  virtual ~Dxxwjz2FileFormat() = default;
  Dxxwjz2FileFormat() = default;

  virtual const std::string& name() const override;
  virtual const std::string& extension() const override;
  virtual const std::string& description() const override;
  virtual bool keeps_structure() const override;
  virtual bool keeps_network() const override;
  virtual IWeightsReaderPtr create_reader() const override;
  virtual IWeightsWriterPtr create_writer() const override;
};

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_FILE_FORMAT_CLASS_H
