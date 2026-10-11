#ifndef DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_READER_CLASS_H

#include <istream>
#include <memory>
#include <string>

#include "src/NetworksLoaders/IWeightsReader.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The original darknet weights file reader: the C++ port of the
 * load_weights() and the load_weights_upto() functions of the original
 * darknet parser.c with the per layer kind functions of theirs.
 *
 * The file is the int32 major, minor and revision version triple, the count
 * of the images the network has seen (the uint64 one since the 0.2 version,
 * the uint32 one before), followed by the float arrays of the layers with the
 * weights in the network order (see the WeightsLayout). The class reads
 * exactly the bytes the original functions did:
 * - The loader skips the dontload layers and the batch normalization arrays
 *   of the dontloadscales ones, transposes the weights of the flipped
 *   convolutional layers and the connected weights of the files of the major
 *   or minor version above 1000.
 * - A truncated file fills the arrays with the whole floats it holds and the
 *   loader stops after the layer reading past the end of the file.
 *
 * The file keeps no structure of its own, so the read() of it into the
 * document takes the network of the file, which tells the arrays of the file
 * (see the OrigWeightsValidator::inspect()).
 *
 * The functions work with the network struct: the adaptors' view or any copy
 * of it. The OrigWeightsWriter writes the files.
 */
class OrigWeightsReader : public IWeightsReader
{
 public:
  using OrigWeightsReaderPtr = std::shared_ptr<OrigWeightsReader>;
  using IWeightsReader::load;

  virtual ~OrigWeightsReader() = default;
  OrigWeightsReader() = default;

  static OrigWeightsReaderPtr create();

  /**
   * @brief The load_weights_upto(): loads the weights of the layers before
   * the cutoff from the file into the network arrays, sets the seen and the
   * current iteration of the network.
   *
   * @return False for the file unable to open, where the original ended the
   * process.
   */
  virtual bool load(network& net, const std::string& filename,
                    int cutoff) override;

  /**
   * @brief Reads the file into the document: the header, the arrays of the
   * network layers the file holds (the whole ones and the part of the array
   * the truncated file ends in) and the bytes after them.
   *
   * The arrays are the ones the save_weights() writes, unless the file is of
   * the size of the arrays the load_weights() reads (the file made for the
   * network of the dontload and the dontloadscales layers), see the
   * OrigWeightsValidator::inspect(). The document keeps the arrays as the
   * file does: not transposed.
   *
   * @param net The network of the file, required.
   *
   * @return False for no network, the file unable to open or shorter than
   * the header.
   */
  virtual bool read(const std::string& filename, const network* net,
                    WeightsDocument& document) override;

  /// @brief Reads the header, false for the file shorter than the header.
  static bool read_header(std::istream& file, WeightsHeader& header);

  /// @brief Transposes the row major rows x cols matrix in place.
  static void transpose_matrix(float* a, int rows, int cols);

  /*
   * The per layer kind parts of the file, as the original functions read
   * them.
   */

  static void load_convolutional_weights(const layer& l, std::istream& file);
  static void load_convolutional_weights_binary(const layer& l,
                                                std::istream& file);
  static void load_shortcut_weights(const layer& l, std::istream& file);
  static void load_implicit_weights(const layer& l, std::istream& file);
  static void load_batchnorm_weights(const layer& l, std::istream& file);
  static void load_connected_weights(const layer& l, std::istream& file,
                                     bool transpose);

 private:
  /**
   * @brief The fread(buffer, sizeof(float), count, file): reads up to the
   * count of the floats, a part of the last one at the end of the file
   * included, and returns the count of the whole floats read.
   */
  static size_t read_floats(std::istream& file, float* buffer, size_t count);
};

using OrigWeightsReaderPtr = OrigWeightsReader::OrigWeightsReaderPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_READER_CLASS_H
