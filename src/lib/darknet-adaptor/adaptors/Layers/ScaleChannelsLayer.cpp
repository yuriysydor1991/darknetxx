#include "src/lib/darknet-adaptor/adaptors/Layers/ScaleChannelsLayer.h"

#include <cassert>

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

ALayerAdaptorPtr ScaleChannelsLayer::make(int batch, int index, int w, int h,
                                          int c, int w2, int h2, int c2,
                                          int scale_wh)
{
  CLOGI("scale Layer: %d", index);

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = SCALE_CHANNELS;
  l.batch = batch;
  l.scale_wh = scale_wh;
  l.w = w;
  l.h = h;
  l.c = c;
  if (!l.scale_wh) {
    assert(w == 1 && h == 1);
  } else {
    assert(c == 1);
  }

  l.out_w = w2;
  l.out_h = h2;
  l.out_c = c2;
  if (!l.scale_wh) {
    assert(l.out_c == l.c);
  } else {
    assert(l.out_w == l.w && l.out_h == l.h);
  }

  l.outputs = l.out_w * l.out_h * l.out_c;
  l.inputs = l.outputs;
  l.index = index;

  adaptor->delta(LayersBuffers::floats(LayersBuffers::count(l.outputs, batch)));
  adaptor->output(
      LayersBuffers::floats(LayersBuffers::count(l.outputs, batch)));

  l.forward = forward_scale_channels_layer;
  l.backward = backward_scale_channels_layer;

  return adaptor;
}

void ScaleChannelsLayer::resize(ALayerAdaptor& adaptor, const network& net)
{
  layer& l = *adaptor.get();

  const layer& first = net.layers[l.index];
  l.out_w = first.out_w;
  l.out_h = first.out_h;
  l.outputs = l.out_w * l.out_h * l.out_c;
  l.inputs = l.outputs;

  const size_t size = LayersBuffers::count(l.outputs, l.batch);

  adaptor.delta(LayersBuffers::resized(adaptor.delta(), size));
  adaptor.output(LayersBuffers::resized(adaptor.output(), size));
}

}  // namespace darknet_adaptor::adaptors::layers
