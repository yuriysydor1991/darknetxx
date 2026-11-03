#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_ARCHIVE_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_ARCHIVE_CLASS_H

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include "src/NetworksLoaders/weights-model/WeightsReport.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

/**
 * @brief The ZIP archive of the dxxwjz4p file (see the Dxxwjz4pFormat):
 * packs the JSON document and the files of the data folder into the archive
 * and unpacks the archive into a folder.
 */
class Dxxwjz4pArchive
{
 public:
  /// @brief The archive member of the file: the member path and the file.
  using Member = std::pair<std::string, std::filesystem::path>;

  /**
   * @brief Unpacks the archive into the folder.
   *
   * The members are the regular files and the directories of the relative
   * paths inside of the folder alone: the absolute paths, the ".." parts, the
   * links and the repeated members are the errors, so no file outside of the
   * folder is written. The archive of no project.json member, the encrypted
   * members, the ones of the compression methods other than the deflate and
   * the corrupted ones (see the Dxxwjz4pZipReader) are the errors too.
   *
   * @param directory The existing folder.
   * @param report Out: the errors and the warnings of the archive.
   *
   * @return False for the archive unable to be unpacked.
   */
  static bool unpack(const std::string& archive,
                     const std::filesystem::path& directory,
                     WeightsReport& report);

  /**
   * @brief Packs the JSON document file and the files of the members into
   * the archive: the project.json member first, the files of the data folder
   * after it in their order, each one the member of it's own.
   *
   * @param json The JSON document file, the gzip compressed one too.
   * @param level The zlib compression level of the members, the 0 stores
   * them.
   *
   * @return False for a file unable to be read or written (logged).
   */
  static bool pack(const std::filesystem::path& json,
                   const std::vector<Member>& members,
                   const std::string& archive, int level);

  /**
   * @brief The relative path of the archive member of the name, the '/'
   * separated one.
   *
   * @return False for the name of no path inside of the archive folder: the
   * empty one, the absolute one, the one of the ".." parts or of the
   * backslashes.
   */
  static bool member_path(const std::string& name,
                          std::filesystem::path& relative);
};

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_ARCHIVE_CLASS_H
