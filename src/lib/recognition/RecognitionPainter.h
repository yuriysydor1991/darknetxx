#ifndef DARKNETXX_PROJECT_RECOGNITIONPAINTER_CLASS_H
#define DARKNETXX_PROJECT_RECOGNITIONPAINTER_CLASS_H

#include <opencv2/core.hpp>
#include <vector>

#include "RecognizedObject.h"

namespace darknetxx_impl
{

/**
 * @brief Draws the boxes of the recognized objects and the labels of their
 * classes over the image, the colors of the class names.
 */
class RecognitionPainter
{
 public:
  /**
   * @brief The PNG image of the BGR or the gray image with the objects
   * boxed. The small image is enlarged first, so the boxes and the labels
   * stay readable.
   *
   * @return The empty image on the failure, which is logged.
   */
  static std::vector<unsigned char> paint(
      const cv::Mat& image,
      const std::vector<darknetxx011::RecognizedObject>& objects);
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_RECOGNITIONPAINTER_CLASS_H
