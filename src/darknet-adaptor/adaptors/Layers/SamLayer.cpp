#include "src/darknet-adaptor/adaptors/Layers/SamLayer.h"

#include <cassert>

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

ALayerAdaptorPtr SamLayer::make(int batch, int index, int w, int h, int c,
                                int w2, int h2, int c2)
{
  CLOGI("scale Layer: %d", index);

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = SAM;
  l.batch = batch;
  l.w = w;
  l.h = h;
  l.c = c;

  l.out_w = w2;
  l.out_h = h2;
  l.out_c = c2;
  assert(l.out_c == l.c);
  assert(l.w == l.out_w && l.h == l.out_h);

  l.outputs = l.out_w * l.out_h * l.out_c;
  l.inputs = l.outputs;
  l.index = index;

  adaptor->delta(LayersBuffers::floats(LayersBuffers::count(l.outputs, batch)));
  adaptor->output(
      LayersBuffers::floats(LayersBuffers::count(l.outputs, batch)));

  l.forward = forward_sam_layer;
  l.backward = backward_sam_layer;

  return adaptor;
}

void SamLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  // the input size as well, as the layer was made (the original kept the
  // initial one)
  l.w = w;
  l.h = h;
  l.out_w = w;
  l.out_h = h;
  l.outputs = l.out_w * l.out_h * l.out_c;
  l.inputs = l.outputs;

  const size_t size = LayersBuffers::count(l.outputs, l.batch);

  adaptor.delta(LayersBuffers::resized(adaptor.delta(), size));
  adaptor.output(LayersBuffers::resized(adaptor.output(), size));
}

}  // namespace darknet_adaptor::adaptors::layers
