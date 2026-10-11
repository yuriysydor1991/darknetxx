#ifndef DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_WRITER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_WRITER_CLASS_H

#include <memory>
#include <ostream>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsWriter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The original darknet weights file writer: the C++ port of the
 * save_weights() and the save_weights_upto() functions of the original
 * darknet parser.c with the per layer kind functions of theirs.
 *
 * The class writes exactly the bytes the original functions did (see the
 * OrigWeightsReader for the file): the current version and the seen of the
 * current iteration (updating the seen of the network), the EMA arrays of the
 * convolutional layers for the save_ema flavour.
 *
 * The functions work with the network struct: the adaptors' view or any copy
 * of it.
 */
class OrigWeightsWriter : public IWeightsWriter
{
 public:
  using OrigWeightsWriterPtr = std::shared_ptr<OrigWeightsWriter>;
  using IWeightsWriter::save;

  virtual ~OrigWeightsWriter() = default;
  OrigWeightsWriter() = default;

  static OrigWeightsWriterPtr create();

  /**
   * @brief The save_weights_upto(): saves the weights of the layers before
   * the cutoff, the EMA ones of the convolutional layers for the save_ema.
   *
   * @return False for the file unable to write, where the original ended the
   * process.
   */
  virtual bool save(network& net, const std::string& filename, int cutoff,
                    bool save_ema) override;

  /**
   * @brief Writes the document: the header, the arrays in the order of the
   * layers and of the WeightsSchema::rank() and the trailing bytes, which
   * makes the file the document was read of.
   *
   * @return False for the document of the errors (see the
   * WeightsDocumentValidator) and the file unable to write.
   */
  virtual bool write(const WeightsDocument& document,
                     const std::string& filename) override;

  /// @brief Writes the header: the 64 or the 32 bits seen of the version.
  static void write_header(std::ostream& file, const WeightsHeader& header);

  /*
   * The per layer kind parts of the file, as the original functions wrote
   * them.
   */

  static void save_convolutional_weights(const layer& l, std::ostream& file);
  static void save_convolutional_weights_ema(const layer& l,
                                             std::ostream& file);
  static void save_convolutional_weights_binary(const layer& l,
                                                std::ostream& file);
  static void save_shortcut_weights(const layer& l, std::ostream& file);
  static void save_implicit_weights(const layer& l, std::ostream& file);
  static void save_batchnorm_weights(const layer& l, std::ostream& file);
  static void save_connected_weights(const layer& l, std::ostream& file);

 private:
  static void write_floats(std::ostream& file, const float* buffer,
                           size_t count);
};

using OrigWeightsWriterPtr = OrigWeightsWriter::OrigWeightsWriterPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_WRITER_CLASS_H
