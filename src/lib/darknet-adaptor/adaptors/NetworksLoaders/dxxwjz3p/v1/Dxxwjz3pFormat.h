#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_FORMAT_CLASS_H

#include <cstdint>

/**
 * @brief The dxxwjz3p (the darknetxx weights json zipped project) files of the
 * first version: the dxxwjz3 document (see the Dxxwjz3Format) of the YOLO4
 * training project, of nothing but the JSON data. The annotations member is
 * the dxxwjz2p one (see the Dxxwjz2pFormat), the ImagesAnnotator project
 * document of the images:
 *
 * @code
 * {
 *   "format": "dxxwjz3p",
 *   "version": 1,
 *   "generator": "darknetxx 0.11.0",
 *   "network": {"net": {"batch": 64, ...}, "layers": [...], "classes": [
 *     {"index": 0, "name": "cat"}, {"index": 1, "name": "dog"}]},
 *   "training": {...},
 *   "annotations": {"ImagesAnnotator": {"version": "0.12.0"}, "annotations": [
 *     {"db": {"path": "data"}, "annotations": [
 *       {"rel_path": "park.jpg", "width": 640, "height": 480, "annotations": [
 *         {"name": "cat", "rect": {"x": 200, "y": 130, "width": 48, "height":
 * 52}}
 *       ]}
 *     ]}
 *   ]},
 *   "header": {...},
 *   "layers": [...]
 * }
 * @endcode
 */
namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

/// @brief The format name and version constants of the dxxwjz3p files.
class Dxxwjz3pFormat
{
 public:
  /// @brief The value of the format key.
  inline static constexpr const char* const NAME = "dxxwjz3p";
  /// @brief The value of the version key.
  inline static constexpr const int64_t VERSION = 1;
  /// @brief The file name extension of the dxxwjz3p files.
  inline static constexpr const char* const EXTENSION = ".dxxwjz3p";
};

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_FORMAT_CLASS_H
