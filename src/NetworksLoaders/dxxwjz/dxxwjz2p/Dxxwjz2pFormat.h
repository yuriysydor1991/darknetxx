#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_FORMAT_CLASS_H

#include <cstdint>

/**
 * @brief The dxxwjz2p (the darknetxx weights json zipped project) files of the
 * first version: the dxxwjz2 document (see the Dxxwjz2Format) of the YOLO4
 * training project, which keeps everything of the YOLO4 folder but the
 * images: the network, the weights and the training record beside the
 * YOLO4 annotations of the images. The annotations member is the project
 * document of the ImagesAnnotator (it's 0.12.0 structure):
 *
 * @code
 * {
 *   "format": "dxxwjz2p",
 *   "version": 1,
 *   "generator": "darknetxx 0.11.0",
 *   "network": {"cfg": "[net]\n...", "classes": [
 *     {"index": 0, "name": "cat"}, {"index": 1, "name": "dog"}]},
 *   "training": {...},
 *   "annotations": {"ImagesAnnotator": {"version": "0.12.0"}, "annotations": [
 *     {"db": {"path": "data"}, "annotations": [
 *       {"rel_path": "park.jpg", "width": 640, "height": 480, "annotations": [
 *         {"name": "cat", "rect": {"x": 200, "y": 130, "width": 48, "height":
 * 52}}
 *       ]},
 *       {"rel_path": "street.png", "width": 640, "height": 400,
 *        "validation": true, "annotations": []}
 *     ]}
 *   ]},
 *   "header": {...},
 *   "layers": [...]
 * }
 * @endcode
 *
 * - The images are grouped by the db path, the directory of the images:
 *   relative to the directory of the dxxwjz2p file (the data directory of
 *   any name beside it) or absolute. The rel_path of an image is relative to
 *   the db path.
 * - The rect of a box is the top left corner and the size of it in the
 *   pixels of the image of the width and the height given, the name is the
 *   class of the network classes.
 * - The validation images are the ones of the mAP calculation alone, the
 *   rest are the training ones.
 */
namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

/// @brief The format name and version constants of the dxxwjz2p files.
class Dxxwjz2pFormat
{
 public:
  /// @brief The value of the format key.
  inline static constexpr const char* const NAME = "dxxwjz2p";
  /// @brief The value of the version key.
  inline static constexpr const int64_t VERSION = 1;
  /// @brief The file name extension of the dxxwjz2p files.
  inline static constexpr const char* const EXTENSION = ".dxxwjz2p";
  /// @brief The ImagesAnnotator project structure version of the
  /// annotations.
  inline static constexpr const char* const ANNOTATOR_VERSION = "0.12.0";
  /// @brief The JSON keys beside the dxxwjz2 ones.
  inline static constexpr const char* const ANNOTATIONS_KEY = "annotations";
  inline static constexpr const char* const ANNOTATOR_KEY = "ImagesAnnotator";
  inline static constexpr const char* const DB_KEY = "db";
  inline static constexpr const char* const PATH_KEY = "path";
  inline static constexpr const char* const REL_PATH_KEY = "rel_path";
  inline static constexpr const char* const WIDTH_KEY = "width";
  inline static constexpr const char* const HEIGHT_KEY = "height";
  inline static constexpr const char* const SCALE_FACTOR_KEY = "scale_factor";
  inline static constexpr const char* const VALIDATION_KEY = "validation";
  inline static constexpr const char* const RECT_KEY = "rect";
  inline static constexpr const char* const X_KEY = "x";
  inline static constexpr const char* const Y_KEY = "y";
};

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_FORMAT_CLASS_H
