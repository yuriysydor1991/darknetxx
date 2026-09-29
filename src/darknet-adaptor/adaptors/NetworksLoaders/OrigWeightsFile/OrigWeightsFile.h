#ifndef DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_FILE_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_FILE_CLASS_H

#include <istream>
#include <ostream>
#include <string>

#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The original darknet weights file: the C++ port of the
 * load_weights(), load_weights_upto(), save_weights() and save_weights_upto()
 * functions of the original darknet parser.c with the per layer kind
 * functions of theirs.
 *
 * The file is the int32 major, minor and revision version triple, the count
 * of the images the network has seen (the uint64 one since the 0.2 version,
 * the uint32 one before), followed by the float arrays of the layers with the
 * weights in the network order. The class reads and writes exactly the bytes
 * the original functions did:
 * - The loader skips the dontload layers and the batch normalization arrays
 *   of the dontloadscales ones, transposes the weights of the flipped
 *   convolutional layers and the connected weights of the files of the major
 *   or minor version above 1000.
 * - A truncated file fills the arrays with the whole floats it holds and the
 *   loader stops after the layer reading past the end of the file.
 * - The saver writes the current version and the seen of the current
 *   iteration (updating the seen of the network), the EMA arrays of the
 *   convolutional layers for the save_ema flavour.
 *
 * The functions work with the network struct: the adaptors' view or any copy
 * of it.
 */
class OrigWeightsFile
{
 public:
  /**
   * @brief The load_weights_upto(): loads the weights of the layers before
   * the cutoff from the file into the network arrays, sets the seen and the
   * current iteration of the network.
   *
   * @return False for the file unable to open, where the original ended the
   * process.
   */
  static bool load(network& net, const std::string& filename, int cutoff);

  /// @brief The load_weights(): load() of every layer.
  static bool load(network& net, const std::string& filename);

  /// @brief The load() of the adaptor's network.
  static bool load(adaptors::ANetworkAdaptor& net, const std::string& filename);

  /**
   * @brief The save_weights_upto(): saves the weights of the layers before
   * the cutoff, the EMA ones of the convolutional layers for the save_ema.
   *
   * @return False for the file unable to write, where the original ended the
   * process.
   */
  static bool save(network& net, const std::string& filename, int cutoff,
                   bool save_ema);

  /// @brief The save_weights(): save() of every layer.
  static bool save(network& net, const std::string& filename);

  /// @brief The save() of the adaptor's network.
  static bool save(adaptors::ANetworkAdaptor& net, const std::string& filename);

  /// @brief Transposes the row major rows x cols matrix in place.
  static void transpose_matrix(float* a, int rows, int cols);

  /*
   * The per layer kind parts of the file, as the original functions wrote
   * and read them.
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

  static void write_floats(std::ostream& file, const float* buffer,
                           size_t count);
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_FILE_CLASS_H
