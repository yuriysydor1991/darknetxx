#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_FORMAT_CLASS_H

#include <cstdint>

/**
 * @brief The dxxwjz4p (the darknetxx weights json zipped project) files of
 * the first version: the whole training project in one file, the ZIP
 * archive of the single JSON document of everything the training needs and
 * of the files of it's annotations in the data folder (the images, the
 * videos of the later versions), each one the file of it's own, which any
 * zip tool lists and extracts:
 *
 * @code
 * project.json          the JSON document below
 * data/park.jpg         the copies of the images of the annotations
 * data/street.png
 * @endcode
 *
 * The JSON document is the dxxwjz3p one (see the Dxxwjz3pFormat) of the
 * description text of the dxxwjz4 files (see the Dxxwjz4Format), the db
 * paths of the annotations are relative to the root of the archive:
 *
 * @code
 * {
 *   "format": "dxxwjz4p",
 *   "version": 1,
 *   "generator": "darknetxx 0.11.0",
 *   "description": "The cars of the parking lot cameras, autumn 2026.",
 *   "network": {"net": {...}, "layers": [...], "classes": ["car", "bus"]},
 *   "training": {...},
 *   "annotations": {"ImagesAnnotator": {"version": "0.12.0"}, "annotations": [
 *     {"db": {"path": "data"}, "annotations": [
 *       {"rel_path": "park.jpg", "width": 640, "height": 480, "annotations": [
 *         {"name": "car", "rect": {"x": 200, "y": 130, "width": 48,
 *          "height": 52}}
 *       ]}
 *     ]}
 *   ]},
 *   "header": {...}
 * }
 * @endcode
 *
 * The members are deflated (stored of the compression level 0), the Zip64
 * records keep the members and the archives of 4 GiB and more (see the
 * Dxxwjz4pZipWriter). The training of the project unpacks it into a folder
 * of the temporary directory (the /tmp one) and trains the images of there,
 * the network it trains is saved into the dxxwjz4 file of the name of the
 * project beside the project file (see the Dxxwjz4pWorkspace).
 */
namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

/// @brief The format name and version constants of the dxxwjz4p files.
class Dxxwjz4pFormat
{
 public:
  /// @brief The value of the format key.
  inline static constexpr const char* const NAME = "dxxwjz4p";
  /// @brief The value of the version key.
  inline static constexpr const int64_t VERSION = 1;
  /// @brief The file name extension of the dxxwjz4p files.
  inline static constexpr const char* const EXTENSION = ".dxxwjz4p";
  /// @brief The archive member of the JSON document.
  inline static constexpr const char* const PROJECT_MEMBER = "project.json";
  /// @brief The archive folder the writers put the files of the annotations
  /// into: the images (the videos of the later versions).
  inline static constexpr const char* const DATA_FOLDER = "data";
};

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_FORMAT_CLASS_H
