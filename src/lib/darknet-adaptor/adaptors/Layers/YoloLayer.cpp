#include "src/lib/darknet-adaptor/adaptors/Layers/YoloLayer.h"

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

ALayerAdaptorPtr YoloLayer::make(int batch, int w, int h, int n, int total,
                                 std::vector<int> mask, int classes,
                                 int max_boxes)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = YOLO;

  l.n = n;
  l.total = total;
  l.batch = batch;
  l.h = h;
  l.w = w;
  l.c = n * (classes + 4 + 1);
  l.out_w = l.w;
  l.out_h = l.h;
  l.out_c = l.c;
  l.classes = classes;
  adaptor->cost(buffers::floats(1U));
  adaptor->biases(buffers::floats(buffers::count(total, 2)));
  l.nbiases = total * 2;

  if (mask.empty()) {
    mask = buffers::ints(buffers::count(n));
    for (int i = 0; i < n; ++i) {
      mask[static_cast<size_t>(i)] = i;
    }
  }
  adaptor->mask(std::move(mask));

  adaptor->bias_updates(buffers::floats(buffers::count(n, 2)));
  l.outputs = h * w * n * (classes + 4 + 1);
  l.inputs = l.outputs;
  l.max_boxes = max_boxes;
  l.truth_size = 4 + 2;
  l.truths = l.max_boxes * l.truth_size;  // 90*(4 + 1);

  const size_t cells = buffers::count(batch, l.w, l.h, l.n);

  adaptor->labels(buffers::ints(cells, -1));
  adaptor->class_ids(buffers::ints(cells, -1));

  adaptor->delta(buffers::floats(buffers::count(batch, l.outputs)));
  adaptor->output(buffers::floats(buffers::count(batch, l.outputs)));
  for (int i = 0; i < total * 2; ++i) {
    l.biases[i] = .5;
  }

  l.forward = forward_yolo_layer;
  l.backward = backward_yolo_layer;

  CLOGIM("yolo");
  srand(static_cast<unsigned int>(time(nullptr)));

  return adaptor;
}

void YoloLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  l.w = w;
  l.h = h;
  // the output of the layer is it's input, as the parser made it (the
  // original kept the out_w and the out_h of the initial size, which the
  // resize of the next layer took)
  l.out_w = l.w;
  l.out_h = l.h;

  l.outputs = h * w * l.n * (l.classes + 4 + 1);
  l.inputs = l.outputs;

  // the original reallocated the output into the embedding_output here, so
  // this resizes the embedding output the parser sized for the embeddings of
  // every cell instead
  if (l.embedding_output) {
    adaptor.embedding_output(
        buffers::resized(adaptor.embedding_output(),
                         buffers::count(l.batch, l.embedding_size, l.n) *
                             buffers::count(l.h, l.w)));
  }

  const size_t cells = buffers::count(l.batch, l.n, l.h, l.w);

  if (l.labels) {
    adaptor.labels(buffers::resized(adaptor.labels(), cells));
  }
  if (l.class_ids) {
    adaptor.class_ids(buffers::resized(adaptor.class_ids(), cells));
  }

  const size_t size = buffers::count(l.batch, l.outputs);

  if (!l.output_pinned) {
    adaptor.output(buffers::resized(adaptor.output(), size));
  }
  if (!l.delta_pinned) {
    adaptor.delta(buffers::resized(adaptor.delta(), size));
  }
}

}  // namespace darknet_adaptor::adaptors::layers
