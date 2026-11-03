#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_PARSER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_PARSER_CLASS_H

#include <filesystem>
#include <istream>
#include <string>

#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

/**
 * @brief Reads the dxxwjz4p project (see the Dxxwjz4pFormat): unpacks the
 * archive into the folder of the Dxxwjz4pWorkspace and reads the JSON
 * document of the project.json member there by the dxxwjz4p handler, the
 * paths of the images are the ones of the folder.
 *
 * The project keeps it's images: the image of a path outside of the archive
 * and the image the archive has no member of are the errors.
 */
class Dxxwjz4pParser
{
 public:
  /// @brief Reads the JSON text of the document.
  /// @param directory The folder of the unpacked project, of the relative db
  /// paths; the images aren't checked.
  static bool parse(std::istream& json, WeightsDocument& document,
                    WeightsReport& report,
                    const std::filesystem::path& directory = {});

  /// @brief Reads the project file.
  static bool parse_file(const std::string& filename, WeightsDocument& document,
                         WeightsReport& report);

  /// @brief The parse_file() of the rest of the document up to it's weights
  /// (see the Dxxwjz3pParser::peek()).
  static bool peek(const std::string& filename, WeightsDocument& rest,
                   bool& weights, WeightsReport& report);

 private:
  /// @brief Reads the JSON document of the unpacked project.
  static bool read(const std::string& filename, bool peek,
                   WeightsDocument& document, WeightsReport& report);

  /// @brief Checks the images of the annotations are the files of the
  /// folder.
  static bool contained(const WeightsDocument& document,
                        const std::filesystem::path& directory,
                        WeightsReport& report);
};

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_PARSER_CLASS_H
