#ifndef DARKNET_CXX_PORT_PROJECT_I_WEIGHTS_FILE_FORMAT_ABSTRACT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_I_WEIGHTS_FILE_FORMAT_ABSTRACT_CLASS_H

#include <memory>
#include <string>

#include "src/NetworksLoaders/IWeightsReader.h"
#include "src/NetworksLoaders/IWeightsWriter.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief A network weights file format: its name, file name extension and
 * description, what its files keep and the reader and the writer of them.
 */
class IWeightsFileFormat
{
 public:
  using IWeightsFileFormatPtr = std::shared_ptr<const IWeightsFileFormat>;

  virtual ~IWeightsFileFormat() = default;
  IWeightsFileFormat() = default;

  /// @brief The name of the command line: "weights", "dxxwjz1", ...
  virtual const std::string& name() const = 0;

  /// @brief The file name extension with the dot: ".weights", ...
  virtual const std::string& extension() const = 0;

  virtual const std::string& description() const = 0;

  /// @brief Tells the files keep the layers and the names of their arrays,
  /// so they are read without the network.
  virtual bool keeps_structure() const = 0;

  /// @brief Tells the files keep the network description and the training
  /// record beside the weights.
  virtual bool keeps_network() const = 0;

  virtual IWeightsReaderPtr create_reader() const = 0;
  virtual IWeightsWriterPtr create_writer() const = 0;
};

using IWeightsFileFormatPtr = IWeightsFileFormat::IWeightsFileFormatPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_I_WEIGHTS_FILE_FORMAT_ABSTRACT_CLASS_H
