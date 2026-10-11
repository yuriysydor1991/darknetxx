#ifndef DARKNETXX_PROJECT_IMAGECLASSIFICATION_CLASS_H
#define DARKNETXX_PROJECT_IMAGECLASSIFICATION_CLASS_H

#include "src/recognition/IRecognitionMethod.h"

namespace darknetxx_impl
{

/**
 * @brief The classification of the whole image by the network of no
 * detection layers: the classes of it's output, of the center of the image
 * scaled to the network width, the darknet predict_classifier() way.
 */
class ImageClassification : public IRecognitionMethod
{
 public:
  virtual Objects recognize(network& net, const image& im, float thresh,
                            const ClassNames& names) const override;
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_IMAGECLASSIFICATION_CLASS_H
