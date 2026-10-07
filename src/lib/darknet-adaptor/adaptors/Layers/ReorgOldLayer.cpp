#include "src/lib/darknet-adaptor/adaptors/Layers/ReorgOldLayer.h"

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

}  // namespace

ALayerAdaptorPtr ReorgOldLayer::make(int batch, int w, int h, int c, int stride,
                                     int reverse)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = REORG_OLD;
  l.batch = batch;
  l.stride = stride;
  l.h = h;
  l.w = w;
  l.c = c;

  if (reverse) {
    l.out_w = w * stride;
    l.out_h = h * stride;
    l.out_c = c / (stride * stride);
  } else {
    l.out_w = w / stride;
    l.out_h = h / stride;
    l.out_c = c * (stride * stride);
  }

  l.reverse = static_cast<float>(reverse);

  CLOGI("reorg_old              /%2d  %4d x%4d x%4d   ->  %4d x%4d x%4d",
        stride, w, h, c, l.out_w, l.out_h, l.out_c);

  l.outputs = l.out_h * l.out_w * l.out_c;
  l.inputs = h * w * c;

  const size_t output_size =
      buffers::count(l.out_h, l.out_w) * buffers::count(l.out_c, batch);

  adaptor->output(buffers::floats(output_size));
  adaptor->delta(buffers::floats(output_size));

  l.forward = forward_reorg_old_layer;
  l.backward = backward_reorg_old_layer;

  return adaptor;
}

void ReorgOldLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  const int stride = l.stride;
  const int c = l.c;

  l.h = h;
  l.w = w;

  if (l.reverse) {
    l.out_w = w * stride;
    l.out_h = h * stride;
    l.out_c = c / (stride * stride);
  } else {
    l.out_w = w / stride;
    l.out_h = h / stride;
    l.out_c = c * (stride * stride);
  }

  l.outputs = l.out_h * l.out_w * l.out_c;
  // the input of the layer, as the layer was made (the original took the
  // outputs, which the stride not dividing the input size shrinks)
  l.inputs = l.h * l.w * l.c;

  const size_t output_size = buffers::count(l.outputs, l.batch);

  adaptor.output(buffers::resized(adaptor.output(), output_size));
  adaptor.delta(buffers::resized(adaptor.delta(), output_size));
}

}  // namespace darknet_adaptor::adaptors::layers
