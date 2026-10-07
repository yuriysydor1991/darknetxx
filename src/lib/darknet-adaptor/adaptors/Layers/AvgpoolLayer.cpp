#include "src/lib/darknet-adaptor/adaptors/Layers/AvgpoolLayer.h"

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

ALayerAdaptorPtr AvgpoolLayer::make(int batch, int w, int h, int c)
{
  CLOGI("avg                          %4d x%4d x%4d ->   %4d", w, h, c, c);

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = AVGPOOL;
  l.batch = batch;
  l.h = h;
  l.w = w;
  l.c = c;
  l.out_w = 1;
  l.out_h = 1;
  l.out_c = c;
  l.outputs = l.out_c;
  l.inputs = h * w * c;

  const size_t output_size = LayersBuffers::count(l.outputs, batch);

  adaptor->output(LayersBuffers::floats(output_size));
  adaptor->delta(LayersBuffers::floats(output_size));

  l.forward = forward_avgpool_layer;
  l.backward = backward_avgpool_layer;

  return adaptor;
}

void AvgpoolLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  l.w = w;
  l.h = h;
  l.inputs = h * w * l.c;
}

}  // namespace darknet_adaptor::adaptors::layers
