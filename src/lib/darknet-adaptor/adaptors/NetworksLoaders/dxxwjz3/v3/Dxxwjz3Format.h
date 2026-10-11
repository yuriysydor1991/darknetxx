#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_FORMAT_CLASS_H

#include <cstdint>

/**
 * @brief The dxxwjz3 (the darknetxx weights json zipped) network weights
 * files of the third version: the dxxwjz2 document (see the Dxxwjz2Format)
 * of nothing but the JSON data. The network cfg is the JSON structure of the
 * network member, it's layers keep their arrays, and the trailing bytes are
 * the JSON array of their values:
 *
 * @code
 * {
 *   "format": "dxxwjz3",
 *   "version": 3,
 *   "generator": "darknetxx 0.11.0",
 *   "network": {"net": {"batch": 64, "width": 416, "steps": [400,450]},
 *     "layers": [
 *     {"type": "convolutional", "options": {"filters": 32, "size": 3},
 *      "arrays": {
 *       "biases": [0.5, -0.25],
 *       "weights": [0.125, "0x7fc00000", ...]
 *     }},
 *     {"type": "maxpool", "options": {"size": 2, "stride": 2}},
 *     {"type": "rnn", "options": {"output": 16}, "sublayers": {
 *       "input_layer": {"arrays": {"biases": [...], "weights": [...]}},
 *       ...
 *     }},
 *     {"type": "yolo", "options": {"mask": [0,1,2], "jitter": 0.3}}
 *   ], "classes": [
 *     {"index": 0, "name": "person"}
 *   ]},
 *   "training": {"seconds": 3725,
 *     "loss": [812.5, 790.25, 701],
 *     "avg_loss": [812.5, 810.27, 799.34],
 *     "map_iteration": [3],
 *     "map": [0.0125]
 *   },
 *   "header": {"major": 0, "minor": 2, "revision": 5, "seen": 192},
 *   "trailing": [0,1]
 * }
 * @endcode
 *
 * - The net object keeps the options of the [net] section of the cfg (the
 *   [network] one), the layers array keeps the layer sections in the cfg
 *   order: the type (the section name without the brackets) and the options
 *   object, none for the section of no options.
 * - The option values are the JSON numbers of the darknet numbers, the
 *   strings of the rest, the arrays of the comma separated ones and the null
 *   of the option line without the '=' (see the Dxxwjz3ValueCodec). The
 *   comments and the white space of the cfg aren't kept, nor the repeated
 *   keys of a section, which darknet never reads.
 * - The layer keeps it's weights in the arrays (or the sublayers) object of
 *   the dxxwjz1 layers (see the Dxxwjz1Format): the layer of the index of
 *   the layer object, of the type the darknet parses the section into. The
 *   layer of no arrays keeps no weights (the layers of no weights and the
 *   ones beyond the cutoff of the save).
 * - The training is the dxxwjz2 one of no iteration column: the losses of
 *   an index are the ones of the iteration of the index + 1, the NaN ones of
 *   the iterations of none (see the Dxxwjz3Losses).
 * - The header is the dxxwjz2 one, the trailing bytes are the integers from
 *   0 to 255; the file of no header keeps no weights, it's the network to
 *   train.
 */
namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/// @brief The format name and version constants of the dxxwjz3 files.
class Dxxwjz3Format
{
 public:
  /// @brief The value of the format key.
  inline static constexpr const char* const NAME = "dxxwjz3";
  /// @brief The value of the version key.
  inline static constexpr const int64_t VERSION = 3;
  /// @brief The file name extension of the dxxwjz3 files.
  inline static constexpr const char* const EXTENSION = ".dxxwjz3";

  /// @brief The JSON keys beside the dxxwjz2 ones.
  inline static constexpr const char* const NET_KEY = "net";
  inline static constexpr const char* const OPTIONS_KEY = "options";
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_FORMAT_CLASS_H
