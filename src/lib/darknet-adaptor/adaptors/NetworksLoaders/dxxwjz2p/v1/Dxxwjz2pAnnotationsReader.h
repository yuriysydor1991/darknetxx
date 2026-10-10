#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_ANNOTATIONS_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_ANNOTATIONS_READER_CLASS_H

#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2MetadataReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/Yolo4Annotations.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

/**
 * @brief Reads the annotations member of the dxxwjz2p document (see the
 * Dxxwjz2pFormat) of it's JSON tree into the YOLO4 annotations: the images of
 * the db paths of the directory of the file and the boxes of the rects of the
 * network classes. Checks the structure, skips the unknown keys with a
 * warning.
 */
class Dxxwjz2pAnnotationsReader : public dxxwjz2::v2::Dxxwjz2MetadataReader
{
 public:
  virtual ~Dxxwjz2pAnnotationsReader() = default;

  /// @param ndirectory The directory of the file, of the relative db paths.
  Dxxwjz2pAnnotationsReader(WeightsReport& nreport,
                            std::filesystem::path ndirectory);

  /// @brief The value of the annotations key of the boxes of the classes,
  /// false for the wrong one (see the error()).
  bool annotations(const json& value, const std::vector<std::string>& classes,
                   Yolo4Annotations& annotations);

 private:
  bool group(const json& value, const std::string& path,
             Yolo4Annotations& annotations);
  bool image(const json& value, const std::string& path,
             const std::filesystem::path& db, Yolo4Image& image);
  bool box(const json& value, const std::string& path, const Yolo4Image& image,
           Yolo4Box& box);

  /// @brief The optional count of the pixels of the key, zero for none.
  bool pixels(const json& value, const std::string& path, const char* name,
              int& count);

  const std::filesystem::path directory;
  /// @brief The class indexes of the class names, the first of the same.
  std::map<std::string, int> mclasses;
};

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_ANNOTATIONS_READER_CLASS_H
