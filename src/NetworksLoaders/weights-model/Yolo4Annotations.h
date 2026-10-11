#ifndef DARKNET_CXX_PORT_PROJECT_YOLO4_ANNOTATIONS_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_YOLO4_ANNOTATIONS_CLASS_H

#include <string>
#include <vector>

#include "src/NetworksLoaders/weights-model/Yolo4Image.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The YOLO4 annotations of the training project the dxxwjz2p and the
 * dxxwjz3p files keep: the annotated images of the training and of the
 * validation, the boxes of their classes, everything of the YOLO4 folder but
 * the images.
 */
class Yolo4Annotations
{
 public:
  bool empty() const;

  /// @brief The paths of the training images, the darknet train= list.
  std::vector<std::string> training_paths() const;

  /// @brief The paths of the validation images, the darknet valid= list:
  /// the training ones for no validation image, the darknet way.
  std::vector<std::string> validation_paths() const;

  std::vector<Yolo4Image> images;

 private:
  std::vector<std::string> paths(bool validation) const;
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_YOLO4_ANNOTATIONS_CLASS_H
