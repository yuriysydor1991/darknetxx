#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_FORMAT_CLASS_H

#include <cstdint>

/**
 * @brief The dxxwjz1 (the darknetxx weights json zipped) network weights
 * files of the first version.
 *
 * The file is the gzip compressed (RFC 1952) UTF-8 JSON document of the
 * network weights:
 *
 * @code
 * {
 *   "format": "dxxwjz1",
 *   "version": 1,
 *   "generator": "darknetxx 0.11.0",
 *   "header": {"major": 0, "minor": 2, "revision": 5, "seen": 64},
 *   "layers": [
 *     {"index": 0, "type": "convolutional", "arrays": {
 *       "biases": [0.5, -0.25],
 *       "weights": [0.125, "0x7fc00000", ...]
 *     }},
 *     {"index": 1, "type": "maxpool"},
 *     {"index": 2, "type": "rnn", "sublayers": {
 *       "input_layer": {"arrays": {"biases": [...], "weights": [...]}},
 *       ...
 *     }}
 *   ],
 *   "trailing": "AAE="
 * }
 * @endcode
 *
 * - The header is the one of the original darknet weights file, the version
 *   tells the way the file loads the same way: the connected weights of the
 *   major or minor > 1000 versions are transposed on the load, the 32 bits
 *   seen of the (major * 10 + minor) < 2 versions.
 * - The layers are the layers of the network in the ascending index order,
 *   named after the cfg sections. The arrays of the layers are named after
 *   the layer fields (see the WeightsSchema), the recurrent layers keep them
 *   in their sub-layers. The arrays are the ones of the original darknet
 *   weights file, which the loaders read the same way: the dontload layers
 *   are skipped, the batch normalization arrays of the dontloadscales ones
 *   are skipped, the flipped convolutional weights are transposed. An array
 *   shorter than the network one fills its beginning, an absent array keeps
 *   the network values.
 * - Every float is the JSON number of the shortest decimal text the float
 *   round trips of, the non finite ones are the "0x" bit pattern strings
 *   (every float may be written that way), which keeps every float bit by
 *   bit, the NaN payloads included.
 * - The optional trailing is the base64 text of the bytes the original file
 *   keeps after the arrays, which the loaders ignore.
 * - The readers ignore the unknown keys (the generator, for example), the
 *   format and the version keys are required.
 *
 * The files are converted to the original darknet weights files and back
 * byte to byte (see the WeightsDocument).
 */
namespace darknet_adaptor::loaders::dxxwjz1::v1
{

/// @brief The format name and version constants of the dxxwjz1 files.
class Dxxwjz1Format
{
 public:
  /// @brief The value of the format key.
  inline static constexpr const char* const NAME = "dxxwjz1";
  /// @brief The value of the version key.
  inline static constexpr const int64_t VERSION = 1;
  /// @brief The file name extension of the dxxwjz1 files.
  inline static constexpr const char* const EXTENSION = ".dxxwjz1";

  /// @brief The JSON keys.
  inline static constexpr const char* const FORMAT_KEY = "format";
  inline static constexpr const char* const VERSION_KEY = "version";
  inline static constexpr const char* const GENERATOR_KEY = "generator";
  inline static constexpr const char* const HEADER_KEY = "header";
  inline static constexpr const char* const MAJOR_KEY = "major";
  inline static constexpr const char* const MINOR_KEY = "minor";
  inline static constexpr const char* const REVISION_KEY = "revision";
  inline static constexpr const char* const SEEN_KEY = "seen";
  inline static constexpr const char* const LAYERS_KEY = "layers";
  inline static constexpr const char* const INDEX_KEY = "index";
  inline static constexpr const char* const TYPE_KEY = "type";
  inline static constexpr const char* const ARRAYS_KEY = "arrays";
  inline static constexpr const char* const SUBLAYERS_KEY = "sublayers";
  inline static constexpr const char* const TRAILING_KEY = "trailing";
};

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_FORMAT_CLASS_H
