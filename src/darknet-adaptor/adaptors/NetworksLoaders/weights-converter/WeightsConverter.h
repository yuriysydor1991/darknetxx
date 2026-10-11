#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERTER_CLASS_H

#include <memory>
#include <string>

#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConversion.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief Converts the network weights files between the formats (see the
 * WeightsFileFormats): the original darknet weights files, the dxxwjz1, the
 * dxxwjz2 and the dxxwjz3 ones and the projects, either way and each one
 * into itself.
 *
 * The conversion is the read of the source file into the format neutral
 * WeightsDocument and the write of the document in the destination format,
 * which keeps every byte of the weights: the header, every float bit by bit,
 * the truncated arrays and the bytes after the arrays. So the original
 * weights file converted to the dxxwjz1, dxxwjz2 or dxxwjz3 one and back is the
 * same file, byte to byte. The network description and the training record
 * are kept by the formats which keep them alone: the cfg and the class names
 * files of the conversion describe the network of the destination, over the
 * description of the source.
 *
 * The conversion of no source file makes the network to train of the cfg
 * file: the dxxwjz2 (dxxwjz3) file of the network description alone, of no
 * layers.
 * Such file converts into the formats of the weights alone into nothing but
 * a header, so that conversion fails.
 *
 * The source is validated (see the OrigWeightsValidator, the
 * Dxxwjz1Validator and the Dxxwjz2Validator): the errors fail the
 * conversion, the warnings are logged. The destination is written into a
 * temporary file of its directory (the directory is made if needed) and
 * replaces the destination once complete, so a failed conversion leaves no
 * destination file behind.
 */
class WeightsConverter
{
 public:
  using WeightsConverterPtr = std::shared_ptr<WeightsConverter>;

  virtual ~WeightsConverter() = default;
  WeightsConverter() = default;

  static WeightsConverterPtr create();

  /**
   * @brief Converts the file.
   *
   * @return False on any failure (logged), which leaves the destination
   * untouched.
   */
  virtual bool convert(const WeightsConversion& conversion);

 protected:
  /// @brief The network of the cfg file, throws on the failure.
  virtual adaptors::ANetworkAdaptorPtr create_network(const std::string& cfg);

 private:
  /// @brief Checks the conversion can start: the formats, the files.
  static bool check(const WeightsConversion& conversion);

  /// @brief Reads and validates the source into the document.
  bool read_source(const WeightsConversion& conversion,
                   const adaptors::ANetworkAdaptorPtr& net,
                   WeightsDocument& document);

  /// @brief The network description of the destination: the cfg and the
  /// class names files of the conversion over the ones of the source.
  static bool describe(const WeightsConversion& conversion,
                       const adaptors::ANetworkAdaptorPtr& net,
                       WeightsDocument& document);

  /// @brief The class names file of the conversion, the names= one of the
  /// network cfg for the document of no classes, empty for neither.
  static std::string names_of(const WeightsConversion& conversion,
                              const adaptors::ANetworkAdaptorPtr& net,
                              const WeightsDocument& document);

  /// @brief Writes the document into the destination through the temporary
  /// file.
  bool write_destination(const WeightsConversion& conversion,
                         const WeightsDocument& document);
};

using WeightsConverterPtr = WeightsConverter::WeightsConverterPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERTER_CLASS_H
