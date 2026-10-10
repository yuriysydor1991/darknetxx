#ifndef DARKNETXX_011_RECOGNIZEDOBJECT_CLASS_H
#define DARKNETXX_011_RECOGNIZEDOBJECT_CLASS_H

#include <string>

#include "LibraryAPI.h"

namespace darknetxx011
{

/**
 * @brief An object a network recognizes in an image: the name of it's class,
 * the probability of it and it's box in the image pixels, the empty one for
 * the class of the whole image.
 */
class DARKNETXX_LIB_API RecognizedObject
{
 public:
  /// @brief Tells the object has a box, the classified image has none.
  bool boxed() const;

  /// @brief The class, the probability and the box the darknet way, e.g.
  /// "dog: 97.5% (left_x: 12 top_y: 40 width: 128 height: 96)".
  std::string text() const;

  std::string name;
  float probability{0.f};
  int x{0};
  int y{0};
  int width{0};
  int height{0};
};

}  // namespace darknetxx011

#endif  // DARKNETXX_011_RECOGNIZEDOBJECT_CLASS_H
