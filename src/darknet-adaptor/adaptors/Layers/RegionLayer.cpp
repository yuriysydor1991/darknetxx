#include "src/darknet-adaptor/adaptors/Layers/RegionLayer.h"

#include <cstdlib>
#include <ctime>

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

}  // namespace

ALayerAdaptorPtr RegionLayer::make(int batch, int w, int h, int n, int classes,
                                   int coords, int max_boxes)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = REGION;

  l.n = n;
  l.batch = batch;
  l.h = h;
  l.w = w;
  l.c = n * (classes + coords + 1);
  l.out_w = l.w;
  l.out_h = l.h;
  l.out_c = l.c;
  l.classes = classes;
  l.coords = coords;
  adaptor->cost(buffers::floats(1U));
  adaptor->biases(buffers::floats(buffers::count(n, 2)));
  adaptor->bias_updates(buffers::floats(buffers::count(n, 2)));
  l.outputs = h * w * n * (classes + coords + 1);
  l.inputs = l.outputs;
  l.max_boxes = max_boxes;
  l.truth_size = 4 + 2;
  l.truths = max_boxes * l.truth_size;
  adaptor->delta(buffers::floats(buffers::count(batch, l.outputs)));
  adaptor->output(buffers::floats(buffers::count(batch, l.outputs)));
  for (int i = 0; i < n * 2; ++i) {
    l.biases[i] = .5;
  }

  l.forward = forward_region_layer;
  l.backward = backward_region_layer;

  CLOGIM("detection");

  srand(static_cast<unsigned int>(time(nullptr)));

  return adaptor;
}

void RegionLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  l.w = w;
  l.h = h;
  // the output of the layer is it's input, as the parser made it (the
  // original kept the out_w and the out_h of the initial size, which the
  // resize of the next layer took)
  l.out_w = l.w;
  l.out_h = l.h;

  l.outputs = h * w * l.n * (l.classes + l.coords + 1);
  l.inputs = l.outputs;

  const size_t size = buffers::count(l.batch, l.outputs);

  adaptor.output(buffers::resized(adaptor.output(), size));
  adaptor.delta(buffers::resized(adaptor.delta(), size));
}

}  // namespace darknet_adaptor::adaptors::layers
