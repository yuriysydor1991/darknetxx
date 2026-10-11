#include "src/darknet-adaptor/adaptors/Layers/SoftmaxLayer.h"

#include <cassert>

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/darknet-adaptor/adaptors/NetworkError.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

}  // namespace

ALayerAdaptorPtr SoftmaxLayer::make(int batch, int inputs, int groups)
{
  assert(inputs % groups == 0);
  CLOGI("softmax                                        %4d", inputs);

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = SOFTMAX;
  l.batch = batch;
  l.groups = groups;
  l.inputs = inputs;
  l.outputs = inputs;
  adaptor->loss(buffers::floats(buffers::count(inputs, batch)));
  adaptor->output(buffers::floats(buffers::count(inputs, batch)));
  adaptor->delta(buffers::floats(buffers::count(inputs, batch)));
  adaptor->cost(buffers::floats(1U));

  l.forward = forward_softmax_layer;
  l.backward = backward_softmax_layer;

  return adaptor;
}

ALayerAdaptorPtr SoftmaxLayer::make_contrastive(int batch, int w, int h, int c,
                                                int classes, int inputs,
                                                layer* yolo_layer)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = CONTRASTIVE;
  l.batch = batch;
  l.inputs = inputs;
  l.w = w;
  l.h = h;
  l.c = c;
  l.temperature = 1;

  l.max_boxes = 0;
  if (yolo_layer) {
    l.detection = 1;
    l.max_boxes = yolo_layer->max_boxes;
    adaptor->borrow_labels(yolo_layer->labels);        // track id
    adaptor->borrow_class_ids(yolo_layer->class_ids);  // class_ids
    l.n = yolo_layer->n;  // num of embeddings per cell = num of anchors
    l.classes = yolo_layer->classes;  // num of classes
    classes = l.classes;
    if (l.n * l.h * l.w == 0) {
      throw NetworkError(
          "The [contrastive] layer needs the image input of the [yolo] "
          "anchors");
    }
    l.embedding_size = l.inputs / (l.n * l.h * l.w);
    l.truths = yolo_layer->truths;
    if (l.embedding_size != yolo_layer->embedding_size) {
      CLOGE(
          " Error: [contrastive] embedding_size=%d isn't equal to [yolo] "
          "embedding_size=%d. They should use the same [convolutional] layer",
          l.embedding_size, yolo_layer->embedding_size);
      throw NetworkError(
          "The [contrastive] and the [yolo] embedding sizes differ");
    }
    if (l.inputs % (l.n * l.h * l.w) != 0) {
      CLOGW(
          " Warning: filters= number in the previous (embedding) layer isn't "
          "divisable by number of anchors %d",
          l.n);
    }
  } else {
    l.detection = 0;
    adaptor->labels(buffers::ints(buffers::count(l.batch)));  // labels
    l.n = 1;              // num of embeddings per cell
    l.classes = classes;  // num of classes
    l.embedding_size = l.c;
  }
  l.outputs = inputs;

  adaptor->loss(buffers::floats(1U));
  adaptor->output(buffers::floats(buffers::count(inputs, batch)));
  adaptor->delta(buffers::floats(buffers::count(inputs, batch)));
  adaptor->cost(buffers::floats(1U));

  const size_t step = buffers::count(l.batch, l.n) * buffers::count(l.h, l.w);
  if (!l.detection) {
    adaptor->cos_sim(buffers::floats(step * step));
    adaptor->exp_cos_sim(buffers::floats(step * step));
    adaptor->p_constrastive(buffers::floats(step * step));
  }

  l.forward = forward_contrastive_layer;
  l.backward = backward_contrastive_layer;
  CLOGI(
      "contrastive %4d x%4d x%4d x emb_size %4d x batch: %4d  classes = "
      "%4d, step = %4zu",
      w, h, l.n, l.embedding_size, batch, l.classes, step);
  if (l.detection) CLOGIM(" detection");

  return adaptor;
}

}  // namespace darknet_adaptor::adaptors::layers
