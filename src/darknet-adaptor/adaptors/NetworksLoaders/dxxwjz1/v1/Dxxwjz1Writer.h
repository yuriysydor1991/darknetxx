#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_WRITER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_WRITER_CLASS_H

#include <memory>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsWriter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"
#include "src/zlib/ZlibController.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

/**
 * @brief The dxxwjz1 file writer (see the Dxxwjz1Format).
 *
 * The save follows the one of the original weights file: the current version
 * and the seen of the current iteration (updating the seen of the network),
 * every layer before the cutoff (the ones without the weights included), the
 * EMA arrays of the convolutional layers for the save_ema flavour. The JSON
 * text is compressed into the file as it's written (see the
 * Dxxwjz1JsonEmitter and the zlibi::ZlibController), a layer at a time.
 *
 * The text is compressed with the fastest zlib level by default: the JSON of
 * the floats takes about 6 times the time of it to compress with the zlib
 * default level for about 12% smaller files (and 30 times the time with the
 * best level for about 14% smaller ones), while the readers take the file of
 * any level.
 */
class Dxxwjz1Writer : public IWeightsWriter
{
 public:
  using Dxxwjz1WriterPtr = std::shared_ptr<Dxxwjz1Writer>;
  using IWeightsWriter::save;

  virtual ~Dxxwjz1Writer() = default;

  /// @brief The compression level of the files written by default.
  static constexpr const int DEFAULT_LEVEL = zlibi::ZlibController::BEST_SPEED;

  /// @param level The gzip compression level (see the zlibi::ZlibController).
  explicit Dxxwjz1Writer(int level = DEFAULT_LEVEL);

  static Dxxwjz1WriterPtr create();

  /**
   * @brief The save_weights_upto() of the dxxwjz1 file.
   *
   * @return False for the file unable to be written.
   */
  virtual bool save(network& net, const std::string& filename, int cutoff,
                    bool save_ema) override;

  /**
   * @brief Writes the document.
   *
   * @return False for the document of the errors (see the
   * WeightsDocumentValidator) and the file unable to be written.
   */
  virtual bool write(const WeightsDocument& document,
                     const std::string& filename) override;

 private:
  int mlevel;
};

using Dxxwjz1WriterPtr = Dxxwjz1Writer::Dxxwjz1WriterPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_WRITER_CLASS_H
