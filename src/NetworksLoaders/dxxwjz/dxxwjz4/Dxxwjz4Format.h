#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_FORMAT_CLASS_H

#include <cstdint>

/**
 * @brief The dxxwjz4 (the darknetxx weights json zipped) network weights
 * files of the fourth version: the dxxwjz3 document (see the Dxxwjz3Format)
 * of the description text, the comments and the description of the network
 * and of it's data of any length the users write:
 *
 * @code
 * {
 *   "format": "dxxwjz4",
 *   "version": 4,
 *   "generator": "darknetxx 0.11.0",
 *   "description": "The cars of the parking lot cameras.\nTrained on ...",
 *   "network": {"net": {...}, "layers": [...], "classes": ["car", "bus"]},
 *   "training": {...},
 *   "header": {...},
 *   "trailing": [0, 1]
 * }
 * @endcode
 *
 * - The description is the JSON string of any length, the empty one of
 *   none, the writers write it before the network, so the peek reads it.
 * - The rest is the dxxwjz3 document: the network, the training, the header
 *   and the trailing bytes; the file of no header keeps no weights, it's the
 *   network to train.
 *
 * The dxxwjz4 file is the network the training of the dxxwjz4p project (see
 * the Dxxwjz4pFormat) saves beside the project.
 */
namespace darknet_adaptor::loaders::dxxwjz4::v4
{

/// @brief The format name and version constants of the dxxwjz4 files.
class Dxxwjz4Format
{
 public:
  /// @brief The value of the format key.
  inline static constexpr const char* const NAME = "dxxwjz4";
  /// @brief The value of the version key.
  inline static constexpr const int64_t VERSION = 4;
  /// @brief The file name extension of the dxxwjz4 files.
  inline static constexpr const char* const EXTENSION = ".dxxwjz4";
  /// @brief The JSON key beside the dxxwjz3 ones: the description text.
  inline static constexpr const char* const DESCRIPTION_KEY = "description";
};

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_FORMAT_CLASS_H
