#ifndef DARKNET_CXX_PORT_PROJECT_YOLO4_FOLDER_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_YOLO4_FOLDER_READER_CLASS_H

#include <filesystem>
#include <string>
#include <vector>

#include "src/NetworksLoaders/IWeightsReader.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::yolo4
{

/**
 * @brief Reads the YOLO4 training folder (see the Yolo4FolderLayout) into the
 * document of the dxxwjz2p project the way the training reads it: the
 * network of the cfg, the latest weights with the rest of them, the class
 * names of the data/obj.names (or of the weights, of the names= key), the
 * images of the lists of the absolute paths, their sizes and the boxes of
 * their label files.
 */
class Yolo4FolderReader : public IWeightsReader
{
 public:
  using IWeightsReader::load;

  virtual ~Yolo4FolderReader() = default;
  Yolo4FolderReader() = default;

  /// @brief None, the folder is converted alone.
  virtual bool load(network& net, const std::string& filename,
                    int cutoff) override;

  /// @param net The network of the latest weights of no structure, the one of
  /// the folder cfg for nullptr.
  virtual bool read(const std::string& filename, const network* net,
                    WeightsDocument& document) override;

 private:
  static bool read_weights(const std::string& weights, const std::string& cfg,
                           const network* net, WeightsDocument& document);

  /// @brief The class names of the folder over the ones of the weights.
  static bool read_classes(const std::filesystem::path& root,
                           const std::string& cfg, WeightsDocument& document);

  /// @brief Appends the images of the list file of the folder.
  static bool read_list(const std::filesystem::path& root,
                        const std::string& list, bool validation,
                        Yolo4Annotations& annotations);

  /// @brief The image of the path of the list with it's size and boxes.
  static Yolo4Image read_image(const std::filesystem::path& root,
                               const std::string& path, bool validation);

  /// @brief The boxes of the label file, the darknet read_boxes() way.
  static std::vector<Yolo4Box> read_boxes(const std::filesystem::path& label);
};

}  // namespace darknet_adaptor::loaders::yolo4

#endif  // DARKNET_CXX_PORT_PROJECT_YOLO4_FOLDER_READER_CLASS_H
