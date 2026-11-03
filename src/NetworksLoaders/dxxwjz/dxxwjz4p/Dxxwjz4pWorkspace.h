#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_WORKSPACE_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_WORKSPACE_CLASS_H

#include <filesystem>
#include <string>

#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/NetworksLoaders/weights-model/Yolo4Annotations.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

/**
 * @brief The folders the dxxwjz4p projects (see the Dxxwjz4pFormat) are
 * unpacked into: the darknetxx-dxxwjz4p-<name>-<random> folders of the
 * temporary directory (the TMPDIR one, the /tmp of no TMPDIR).
 *
 * The project is unpacked the first time the process reads it and once the
 * file changed (of the other size or the modification time), so the reads
 * of the same project share the folder of it: the training reads the JSON
 * document and the images of the folder. The folders are removed at the end
 * of the process. Thread safe.
 */
class Dxxwjz4pWorkspace
{
 public:
  /**
   * @brief The folder of the unpacked project, unpacked by the call when
   * needed.
   *
   * @param report Out: the errors and the warnings of the unpack.
   *
   * @return The folder, the empty path for the project unable to be unpacked
   * (the errors of the report).
   */
  static std::filesystem::path of(const std::string& archive,
                                  WeightsReport& report);

  /// @brief Tells the path is inside of a folder of the unpacked projects.
  static bool owns(const std::filesystem::path& path);

  /**
   * @brief Copies the images of the annotations of the unpacked projects
   * into the folder of the file the annotations are written into, the ones of
   * the paths of the project folders, and makes the annotations of the
   * copies: the data/park.jpg of the project is the data/park.jpg beside the
   * file. So the project converted into the dxxwjz3p or the dxxwjz2p one
   * refers to the images of no temporary folder.
   *
   * The existing file of another content isn't replaced, it's the error.
   *
   * @param directory The folder of the file of the annotations.
   * @param error Out: the error, empty for none.
   *
   * @return False for an image unable to be copied.
   */
  static bool export_images(Yolo4Annotations& annotations,
                            const std::filesystem::path& directory,
                            std::string& error);
};

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_WORKSPACE_CLASS_H
