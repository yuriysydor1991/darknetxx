#ifndef DARKNET_CXX_PORT_PROJECT_YOLO4_FOLDER_LAYOUT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_YOLO4_FOLDER_LAYOUT_CLASS_H

#include <filesystem>
#include <string>

namespace darknet_adaptor::loaders::yolo4
{

/**
 * @brief The files of the YOLO4 training folder, the one the ImagesAnnotator
 * exports: the network cfg/ *.cfg file, the data/obj.names class names, the
 * lists of the images, the label files beside the images (see the darknet
 * replace_image_to_label()) and the backup folder of the weights. The train=,
 * the valid= and the backup= keys of the [net] section of the cfg name the
 * lists and the backup folder relative to the folder, the working directory
 * of the training.
 */
class Yolo4FolderLayout
{
 public:
  /// @brief The files the writer makes, relative to the folder.
  inline static const std::string CFG_DIR{"cfg"};
  inline static const std::string IMAGES_DIR{"data/obj"};
  inline static const std::string NAMES{"data/obj.names"};
  inline static const std::string DATA{"data/obj.data"};
  inline static const std::string TRAIN{"data/train.txt"};
  inline static const std::string VALID{"data/val.txt"};
  inline static const std::string BACKUP{"backup"};

  /// @brief The network cfg file: the first cfg/ *.cfg one by the name,
  /// empty for none.
  static std::string cfg(const std::filesystem::path& root);

  /// @brief The data/obj.names class names file, empty for none.
  static std::string names(const std::filesystem::path& root);

  /// @brief The weights a training resumes from: the newest last weights of
  /// the backup folder or the newest other ones of the cfg base name, of
  /// every weights file format, empty for none.
  static std::string latest_weights(const std::filesystem::path& root,
                                    const std::string& cfg);
};

}  // namespace darknet_adaptor::loaders::yolo4

#endif  // DARKNET_CXX_PORT_PROJECT_YOLO4_FOLDER_LAYOUT_CLASS_H
