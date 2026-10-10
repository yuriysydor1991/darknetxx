#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_FORMAT_CLASS_H

#include <cstdint>

/**
 * @brief The dxxwjz3 (the darknetxx weights json zipped) network weights
 * files of the third version: the dxxwjz2 document (see the Dxxwjz2Format)
 * of nothing but the JSON data. The network cfg is the JSON structure of the
 * network member and the trailing bytes are the JSON array of their values:
 *
 * @code
 * {
 *   "format": "dxxwjz3",
 *   "version": 3,
 *   "generator": "darknetxx 0.11.0",
 *   "network": {"net": {"batch": 64, "width": 416, "steps": [400,450]},
 *     "layers": [
 *     {"type": "convolutional", "options": {"filters": 32, "size": 3}},
 *     {"type": "maxpool", "options": {"size": 2, "stride": 2}},
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
 *   "layers": [...],
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
 * - The training is the dxxwjz2 one of no iteration column: the losses of
 *   an index are the ones of the iteration of the index + 1, the NaN ones of
 *   the iterations of none (see the Dxxwjz3Losses).
 * - The header and the layers are the dxxwjz2 ones, the trailing bytes are
 *   the integers from 0 to 255.
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
