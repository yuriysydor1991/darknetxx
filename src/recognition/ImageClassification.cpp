#include "src/recognition/ImageClassification.h"

#include "tree.h"

namespace darknetxx_impl
{

ImageClassification::Objects ImageClassification::recognize(
    network& net, const image& im, const float thresh,
    const ClassNames& names) const
{
  image resized = resize_min(im, net.w);
  image cropped = crop_image(resized, (resized.w - net.w) / 2,
                             (resized.h - net.h) / 2, net.w, net.h);
  float* predictions = network_predict(net, cropped.data);

  // the image of the network width already is no copy
  if (resized.data != im.data) {
    free_image(resized);
  }

  free_image(cropped);

  if (net.hierarchy != nullptr) {
    hierarchy_predictions(predictions, net.outputs, net.hierarchy, 0);
  }

  Objects objects;

  for (int i = 0; i < net.outputs; ++i) {
    if (predictions[i] > thresh) {
      objects.push_back({class_name(names, i), predictions[i]});
    }
  }

  return objects;
}

}  // namespace darknetxx_impl
