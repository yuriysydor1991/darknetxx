#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_ANNOTATIONS_JSON_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_ANNOTATIONS_JSON_CLASS_H

#include <filesystem>
#include <functional>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkDescription.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/Yolo4Annotations.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

/**
 * @brief Writes the JSON text of the annotations member of the projects (see
 * the Dxxwjz2pFormat): the images of a directory in a group of it, an image
 * on a line, the boxes in the pixels of the class names of the network.
 */
class Dxxwjz2pAnnotationsJson
{
 public:
  /// @brief Takes the text written.
  using Sink = std::function<void(const std::string&)>;

  /// @param network The network of the class names of the boxes, which
  /// outlives the writer.
  /// @param annotations The annotations written, which outlive the writer.
  /// @param ndirectory The directory of the file, of the relative db paths.
  Dxxwjz2pAnnotationsJson(const NetworkDescription& network,
                          const Yolo4Annotations& annotations,
                          std::filesystem::path ndirectory);

  /// @brief Writes the member after the ",\n  " text into the sink.
  void write(const Sink& sink) const;

 private:
  void image(const Yolo4Image& image, bool first, const Sink& sink) const;

  /// @brief The db path of the directory of the image: the one relative to
  /// the directory of the file below it, the absolute one otherwise.
  std::string db_of(const std::string& path) const;

  const NetworkDescription& mnetwork;
  const Yolo4Annotations& mannotations;
  const std::filesystem::path directory;
};

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_ANNOTATIONS_JSON_CLASS_H
