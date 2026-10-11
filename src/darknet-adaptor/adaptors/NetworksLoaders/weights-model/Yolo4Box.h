#ifndef DARKNET_CXX_PORT_PROJECT_YOLO4_BOX_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_YOLO4_BOX_CLASS_H

namespace darknet_adaptor::loaders
{

/**
 * @brief A box of an image the way the YOLO4 label files keep it: the index
 * of the class and the center and the size of the box, the fractions of the
 * width and the height of the image.
 */
class Yolo4Box
{
 public:
  int id{0};
  float x{0.f};
  float y{0.f};
  float w{0.f};
  float h{0.f};

  /// @brief The box of the same class and the same values.
  bool operator==(const Yolo4Box& other) const
  {
    return id == other.id && x == other.x && y == other.y && w == other.w &&
           h == other.h;
  }
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_YOLO4_BOX_CLASS_H
