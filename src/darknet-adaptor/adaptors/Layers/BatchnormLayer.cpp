#include "src/darknet-adaptor/adaptors/Layers/BatchnormLayer.h"

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

}  // namespace

ALayerAdaptorPtr BatchnormLayer::make(int batch, int w, int h, int c, int train)
{
  CLOGI("Batch Normalization Layer: %d x %d x %d image", w, h, c);

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = BATCHNORM;
  l.batch = batch;
  l.train = train;
  l.h = l.out_h = h;
  l.w = l.out_w = w;
  l.c = l.out_c = c;

  l.n = l.c;
  adaptor->output(buffers::floats(buffers::count(h, w, c, batch)));
  adaptor->delta(buffers::floats(buffers::count(h, w, c, batch)));
  l.inputs = w * h * c;
  l.outputs = l.inputs;

  adaptor->biases(buffers::floats(buffers::count(c)));
  adaptor->bias_updates(buffers::floats(buffers::count(c)));

  adaptor->scales(buffers::floats(buffers::count(c), 1.f));
  adaptor->scale_updates(buffers::floats(buffers::count(c)));

  adaptor->mean(buffers::floats(buffers::count(c)));
  adaptor->variance(buffers::floats(buffers::count(c)));

  adaptor->rolling_mean(buffers::floats(buffers::count(c)));
  adaptor->rolling_variance(buffers::floats(buffers::count(c)));

  adaptor->mean_delta(buffers::floats(buffers::count(c)));
  adaptor->variance_delta(buffers::floats(buffers::count(c)));

  adaptor->x(buffers::floats(buffers::count(l.batch, l.outputs)));
  adaptor->x_norm(buffers::floats(buffers::count(l.batch, l.outputs)));

  l.forward = forward_batchnorm_layer;
  l.backward = backward_batchnorm_layer;
  l.update = update_batchnorm_layer;

  return adaptor;
}

void BatchnormLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  l.out_h = l.h = h;
  l.out_w = l.w = w;
  l.outputs = l.inputs = h * w * l.c;

  const size_t output_size = buffers::count(l.outputs, l.batch);

  adaptor.output(buffers::resized(adaptor.output(), output_size));
  adaptor.delta(buffers::resized(adaptor.delta(), output_size));

  // the original resized the x and the x_norm of the GPU builds only, which
  // the CPU training forward of the bigger input overflowed
  if (!adaptor.x().empty()) {
    adaptor.x(buffers::resized(adaptor.x(), output_size));
  }
  if (!adaptor.x_norm().empty()) {
    adaptor.x_norm(buffers::resized(adaptor.x_norm(), output_size));
  }
}

}  // namespace darknet_adaptor::adaptors::layers
