#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERTER_CLASS_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsReader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsWriter.h"

namespace darknet_adaptor::loaders
{

/// @brief The network weights files formats.
enum class WeightsFormat
{
  /// the original darknet weights file (the *.weights one)
  Original,
  /// the dxxwjz1 file of the first version (see the Dxxwjz1Format)
  Dxxwjz1
};

/// @brief One conversion of a network weights file.
struct WeightsConversion
{
  WeightsFormat from{WeightsFormat::Original};
  /// the file to read
  std::string src;
  WeightsFormat to{WeightsFormat::Dxxwjz1};
  /// the file to write, replaced by the complete file alone
  std::string dst;
  /**
   * @brief The network cfg file of the weights: required to read the
   * original weights file, which keeps no structure of its own, the network
   * the dxxwjz1 file is validated against otherwise. Empty for none.
   */
  std::string cfg;
  /// @brief Asked between the steps of the conversion, true stops it (the
  /// stop request of the application, for example). Empty for none.
  std::function<bool()> stop_requested;
};

/**
 * @brief Converts the network weights files between the formats: the
 * original darknet weights files and the dxxwjz1 ones, either way and each
 * one into itself.
 *
 * The conversion is the read of the source file into the format neutral
 * WeightsDocument and the write of the document in the destination format,
 * which keeps every byte of the file: the header, every float bit by bit, the
 * truncated arrays and the bytes after the arrays. So the original weights
 * file converted to the dxxwjz1 one and back is the same file, byte to byte.
 *
 * The source is validated (see the OrigWeightsValidator and the
 * Dxxwjz1Validator): the errors fail the conversion, the warnings are logged.
 * The destination is written into a temporary file of its directory (the
 * directory is made if needed) and replaces the destination once complete, so
 * a failed conversion leaves no destination file behind.
 */
class WeightsConverter
{
 public:
  using WeightsConverterPtr = std::shared_ptr<WeightsConverter>;

  virtual ~WeightsConverter() = default;
  WeightsConverter() = default;

  static WeightsConverterPtr create();

  /// @brief The names of the formats: the "weights" and the "dxxwjz1" ones.
  static const std::vector<std::string>& format_names();

  /// @brief The name of the format.
  static const std::string& format_name(WeightsFormat format);

  /// @brief The format of the name, false for an unknown name.
  static bool parse_format(const std::string& name, WeightsFormat& format);

  /**
   * @brief Converts the file.
   *
   * @return False on any failure (logged), which leaves the destination
   * untouched.
   */
  virtual bool convert(const WeightsConversion& conversion);

 protected:
  virtual IWeightsReaderPtr create_reader(WeightsFormat format);
  virtual IWeightsWriterPtr create_writer(WeightsFormat format);

  /// @brief The network of the cfg file, throws on the failure.
  virtual adaptors::ANetworkAdaptorPtr create_network(const std::string& cfg);

 private:
  /// @brief Reads and validates the source into the document.
  bool read_source(const WeightsConversion& conversion,
                   const adaptors::ANetworkAdaptorPtr& net,
                   WeightsDocument& document);

  /// @brief Writes the document into the destination through the temporary
  /// file.
  bool write_destination(const WeightsConversion& conversion,
                         const WeightsDocument& document);
};

using WeightsConverterPtr = WeightsConverter::WeightsConverterPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERTER_CLASS_H
