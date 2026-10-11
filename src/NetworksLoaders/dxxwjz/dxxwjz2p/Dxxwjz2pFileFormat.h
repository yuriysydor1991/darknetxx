#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_FILE_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_FILE_FORMAT_CLASS_H

#include <string>

#include "src/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

/// @brief The dxxwjz2p project file format of the first version (see the
/// Dxxwjz2pFormat).
class Dxxwjz2pFileFormat : public IWeightsFileFormat
{
 public:
  virtual ~Dxxwjz2pFileFormat() = default;
  Dxxwjz2pFileFormat() = default;

  virtual const std::string& name() const override;
  virtual const std::string& extension() const override;
  virtual const std::string& description() const override;
  virtual bool keeps_structure() const override;
  virtual bool keeps_network() const override;
  virtual IWeightsReaderPtr create_reader() const override;
  virtual IWeightsWriterPtr create_writer() const override;
};

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_FILE_FORMAT_CLASS_H
