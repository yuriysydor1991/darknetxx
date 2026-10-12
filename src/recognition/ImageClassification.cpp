#include "src/recognition/ImageClassification.h"

#include "tree.h"

namespace darknetxx_impl
{

ImageClassification::Objects ImageClassification::recognize(
    network& net, const image& im, const float thresh,
    const ClassNames& names) const
{
  image sized = net.letter_box ? letterbox_image(im, net.w, net.h)
                               : resize_image(im, net.w, net.h);
  float* predictions = network_predict(net, sized.data);

  free_image(sized);

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
