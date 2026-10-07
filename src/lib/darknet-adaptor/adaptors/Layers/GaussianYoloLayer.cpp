#include "src/lib/darknet-adaptor/adaptors/Layers/GaussianYoloLayer.h"

#include <cstdlib>
#include <ctime>
#include <utility>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

}  // namespace

ALayerAdaptorPtr GaussianYoloLayer::make(int batch, int w, int h, int n,
                                         int total, std::vector<int> mask,
                                         int classes, int max_boxes)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = GAUSSIAN_YOLO;

  l.n = n;
  l.total = total;
  l.batch = batch;
  l.h = h;
  l.w = w;
  l.c = n * (classes + 8 + 1);
  l.out_w = l.w;
  l.out_h = l.h;
  l.out_c = l.c;
  l.classes = classes;
  adaptor->cost(buffers::floats(1U));
  adaptor->biases(buffers::floats(buffers::count(total, 2)));

  if (mask.empty()) {
    mask = buffers::ints(buffers::count(n));
    for (int i = 0; i < n; ++i) {
      mask[static_cast<size_t>(i)] = i;
    }
  }
  adaptor->mask(std::move(mask));

  adaptor->bias_updates(buffers::floats(buffers::count(n, 2)));
  l.outputs = h * w * n * (classes + 8 + 1);
  l.inputs = l.outputs;
  l.max_boxes = max_boxes;
  l.truth_size = 4 + 2;
  l.truths = l.max_boxes * l.truth_size;
  adaptor->delta(buffers::floats(buffers::count(batch, l.outputs)));
  adaptor->output(buffers::floats(buffers::count(batch, l.outputs)));
  for (int i = 0; i < total * 2; ++i) {
    l.biases[i] = .5;
  }

  l.forward = forward_gaussian_yolo_layer;
  l.backward = backward_gaussian_yolo_layer;

  CLOGTM("Gaussian_yolo");
  srand(static_cast<unsigned int>(time(nullptr)));

  return adaptor;
}

void GaussianYoloLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  l.w = w;
  l.h = h;
  // the output of the layer is it's input, as the parser made it (the
  // original kept the out_w and the out_h of the initial size, which the
  // resize of the next layer took)
  l.out_w = l.w;
  l.out_h = l.h;

  l.outputs = h * w * l.n * (l.classes + 8 + 1);
  l.inputs = l.outputs;

  const size_t size = buffers::count(l.batch, l.outputs);

  if (!l.output_pinned) {
    adaptor.output(buffers::resized(adaptor.output(), size));
  }
  if (!l.delta_pinned) {
    adaptor.delta(buffers::resized(adaptor.delta(), size));
  }
}

}  // namespace darknet_adaptor::adaptors::layers
