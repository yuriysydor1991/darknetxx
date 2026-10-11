#ifndef DARKNET_CXX_PORT_PROJECT_I_WEIGHTS_FILE_FORMAT_ABSTRACT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_I_WEIGHTS_FILE_FORMAT_ABSTRACT_CLASS_H

#include <memory>
#include <string>

namespace darknet_adaptor::loaders
{

class IWeightsFileFormat
{
 public:
  using IWeightsFileFormatPtr = std::shared_ptr<const IWeightsFileFormat>;

  virtual ~IWeightsFileFormat() = default;

  virtual const std::string& name() const = 0;
};

using IWeightsFileFormatPtr = IWeightsFileFormat::IWeightsFileFormatPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_I_WEIGHTS_FILE_FORMAT_ABSTRACT_CLASS_H
