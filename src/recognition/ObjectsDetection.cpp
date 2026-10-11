#include "src/recognition/ObjectsDetection.h"

#include <algorithm>

namespace darknetxx_impl
{

ObjectsDetection::Objects ObjectsDetection::recognize(
    network& net, const image& im, const float thresh,
    const ClassNames& names) const
{
  // the detect_objects_ctx() ones
  static constexpr const float NMS = .45f;
  static constexpr const float HIER_THRESH = .5f;

  const layer& l = *detection_layer(net);
  image sized = net.letter_box ? letterbox_image(im, net.w, net.h)
                               : resize_image(im, net.w, net.h);

  network_predict(net, sized.data);
  free_image(sized);

  int count = 0;
  detection* dets = get_network_boxes(&net, im.w, im.h, thresh, HIER_THRESH,
                                      nullptr, 1, &count, net.letter_box);

  if (l.nms_kind == DEFAULT_NMS) {
    do_nms_sort(dets, count, l.classes, NMS);
  } else {
    diounms_sort(dets, count, l.classes, NMS, l.nms_kind, l.beta_nms);
  }

  Objects objects;

  for (int i = 0; i < count; ++i) {
    const float* best =
        std::max_element(dets[i].prob, dets[i].prob + l.classes);

    if (*best <= thresh) {
      continue;
    }

    // the darknet draw_detections_v3() box pixels, inside the image
    const box& b = dets[i].bbox;
    const auto pixel = [](const float at, const int size) {
      return std::clamp(static_cast<int>(at * static_cast<float>(size)), 0,
                        size - 1);
    };
    const int left = pixel(b.x - b.w / 2, im.w);
    const int top = pixel(b.y - b.h / 2, im.h);

    objects.push_back({class_name(names, static_cast<int>(best - dets[i].prob)),
                       *best, left, top, pixel(b.x + b.w / 2, im.w) - left,
                       pixel(b.y + b.h / 2, im.h) - top});
  }

  free_detections(dets, count);

  return objects;
}

const layer* ObjectsDetection::detection_layer(const network& net)
{
  const layer* first = net.layers;
  const layer* last = first + net.n;
  const layer* found = std::find_if(first, last, [](const layer& l) {
    return l.type == YOLO || l.type == GAUSSIAN_YOLO || l.type == REGION ||
           l.type == DETECTION;
  });

  return found == last ? nullptr : found;
}

}  // namespace darknetxx_impl
