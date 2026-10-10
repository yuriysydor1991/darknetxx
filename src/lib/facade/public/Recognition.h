#ifndef DARKNETXX_011_RECOGNITION_CLASS_H
#define DARKNETXX_011_RECOGNITION_CLASS_H

#include <string>
#include <vector>

#include "LibraryAPI.h"
#include "RecognizedObject.h"

namespace darknetxx011
{

/**
 * @brief The recognition of an image by a network: the objects it detects in
 * the image or the classes of the whole image, the most probable first, the
 * image with the objects boxed and the time it took, or the reason it failed.
 */
class DARKNETXX_LIB_API Recognition
{
 public:
  bool succeeded() const;

  /// @brief The count of the objects and the time, e.g. "3 objects
  /// recognized in 0.042 s", or the error.
  std::string summary_text() const;

  /// @brief The reason the recognition failed, empty for the succeeded one.
  std::string error;
  std::vector<RecognizedObject> objects;
  /// @brief The PNG image of the recognized one, the objects boxed.
  std::vector<unsigned char> png;
  double seconds{0.0};
};

}  // namespace darknetxx011

#endif  // DARKNETXX_011_RECOGNITION_CLASS_H
