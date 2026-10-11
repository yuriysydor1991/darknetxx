#ifndef DARKNET_CXX_PORT_PROJECT_YOLO4_IMAGE_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_YOLO4_IMAGE_CLASS_H

#include <cstddef>
#include <string>
#include <vector>

#include "src/NetworksLoaders/weights-model/Yolo4Box.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief An annotated image of the training: the absolute path of it, it's
 * size in the pixels (zero for unknown, the one of the boxes in the pixels
 * otherwise), it's boxes (none for the image of no object) and the set it
 * belongs to.
 */
class Yolo4Image
{
 public:
  /// @brief The longest image path of the darknet label path buffers.
  static constexpr const std::size_t MAX_PATH = 4095U;

  /// @brief The darknet label file path of the image path (see the
  /// replace_image_to_label()), empty for the path longer than the MAX_PATH.
  static std::string label_of(const std::string& path);

  std::string path;
  int width{0};
  int height{0};
  std::vector<Yolo4Box> boxes;
  /// @brief The image of the mAP calculation alone, the validation one.
  bool validation{false};
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_YOLO4_IMAGE_CLASS_H
