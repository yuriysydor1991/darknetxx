#ifndef DARKNETXX_PROJECT_OBJECTSDETECTION_CLASS_H
#define DARKNETXX_PROJECT_OBJECTSDETECTION_CLASS_H

#include "src/recognition/IRecognitionMethod.h"

namespace darknetxx_impl
{

/**
 * @brief The detection of the objects in the image by the network of the
 * YOLO, the Gaussian YOLO, the region or the detection layers: the boxes
 * left by the non-maximum suppression, the darknet test_detector_ctx() way.
 */
class ObjectsDetection : public IRecognitionMethod
{
 public:
  virtual Objects recognize(network& net, const image& im, float thresh,
                            const ClassNames& names) const override;

  /// @brief The first YOLO, Gaussian YOLO, region or detection layer of the
  /// network, the one of it's boxes, nullptr for none.
  static const layer* detection_layer(const network& net);
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_OBJECTSDETECTION_CLASS_H
