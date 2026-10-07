#include "src/lib/darknet-adaptor/adaptors/Layers/UpsampleLayer.h"

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

ALayerAdaptorPtr UpsampleLayer::make(int batch, int w, int h, int c, int stride)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = UPSAMPLE;
  l.batch = batch;
  l.w = w;
  l.h = h;
  l.c = c;
  l.out_w = w * stride;
  l.out_h = h * stride;
  l.out_c = c;
  if (stride < 0) {
    stride = -stride;
    l.reverse = 1;
    l.out_w = w / stride;
    l.out_h = h / stride;
  }
  l.stride = stride;
  l.outputs = l.out_w * l.out_h * l.out_c;
  l.inputs = l.w * l.h * l.c;
  adaptor->delta(LayersBuffers::floats(LayersBuffers::count(l.outputs, batch)));
  adaptor->output(
      LayersBuffers::floats(LayersBuffers::count(l.outputs, batch)));

  l.forward = forward_upsample_layer;
  l.backward = backward_upsample_layer;

  if (l.reverse) {
    CLOGI("downsample              %2dx  %4d x%4d x%4d -> %4d x%4d x%4d",
          stride, w, h, c, l.out_w, l.out_h, l.out_c);
  } else {
    CLOGI("upsample                %2dx  %4d x%4d x%4d -> %4d x%4d x%4d",
          stride, w, h, c, l.out_w, l.out_h, l.out_c);
  }

  return adaptor;
}

void UpsampleLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  l.w = w;
  l.h = h;
  l.out_w = w * l.stride;
  l.out_h = h * l.stride;
  if (l.reverse) {
    l.out_w = w / l.stride;
    l.out_h = h / l.stride;
  }
  l.outputs = l.out_w * l.out_h * l.out_c;
  l.inputs = l.h * l.w * l.c;

  const size_t size = LayersBuffers::count(l.outputs, l.batch);

  adaptor.delta(LayersBuffers::resized(adaptor.delta(), size));
  adaptor.output(LayersBuffers::resized(adaptor.output(), size));
}

}  // namespace darknet_adaptor::adaptors::layers
