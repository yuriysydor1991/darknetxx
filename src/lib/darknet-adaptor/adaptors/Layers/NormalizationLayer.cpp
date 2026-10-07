#include "src/lib/darknet-adaptor/adaptors/Layers/NormalizationLayer.h"

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

}  // namespace

ALayerAdaptorPtr NormalizationLayer::make(int batch, int w, int h, int c,
                                          int size, float alpha, float beta,
                                          float kappa)
{
  CLOGI("Local Response Normalization Layer: %d x %d x %d image, %d size", w, h,
        c, size);

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = NORMALIZATION;
  l.batch = batch;
  l.h = l.out_h = h;
  l.w = l.out_w = w;
  l.c = l.out_c = c;
  l.kappa = kappa;
  l.size = size;
  l.alpha = alpha;
  l.beta = beta;

  const size_t size_all = buffers::count(h, w, c, batch);

  adaptor->output(buffers::floats(size_all));
  adaptor->delta(buffers::floats(size_all));
  adaptor->squared(buffers::floats(size_all));
  adaptor->norms(buffers::floats(size_all));
  l.inputs = w * h * c;
  l.outputs = l.inputs;

  l.forward = forward_normalization_layer;
  l.backward = backward_normalization_layer;

  return adaptor;
}

void NormalizationLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  const int c = l.c;
  const int batch = l.batch;
  l.h = h;
  l.w = w;
  l.out_h = h;
  l.out_w = w;
  l.inputs = w * h * c;
  l.outputs = l.inputs;

  const size_t size_all = buffers::count(h, w, c, batch);

  adaptor.output(buffers::resized(adaptor.output(), size_all));
  adaptor.delta(buffers::resized(adaptor.delta(), size_all));
  adaptor.squared(buffers::resized(adaptor.squared(), size_all));
  adaptor.norms(buffers::resized(adaptor.norms(), size_all));
}

}  // namespace darknet_adaptor::adaptors::layers
