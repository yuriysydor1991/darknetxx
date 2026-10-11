#ifndef DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_FILE_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_FILE_FORMAT_CLASS_H

#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders
{

/// @brief The original darknet weights file format: the float arrays of the
/// network alone.
class OrigWeightsFileFormat : public IWeightsFileFormat
{
 public:
  virtual ~OrigWeightsFileFormat() = default;
  OrigWeightsFileFormat() = default;

  virtual const std::string& name() const override;
  virtual const std::string& extension() const override;
  virtual const std::string& description() const override;
  virtual bool keeps_structure() const override;
  virtual bool keeps_network() const override;
  virtual IWeightsReaderPtr create_reader() const override;
  virtual IWeightsWriterPtr create_writer() const override;
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_FILE_FORMAT_CLASS_H
