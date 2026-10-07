#include "src/lib/darknet-adaptor/adaptors/Layers/ImplicitLayer.h"

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

ALayerAdaptorPtr ImplicitLayer::make(int batch, int index, float mean_init,
                                     float std_init, int filters, int atoms)
{
  CLOGI("implicit Layer: %d x %d \t mean=%.2f, std=%.2f", filters, atoms,
        static_cast<double>(mean_init), static_cast<double>(std_init));

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = IMPLICIT;
  l.batch = batch;
  l.w = 1;
  l.h = 1;
  l.c = 1;

  l.out_w = 1;
  l.out_h = atoms;
  l.out_c = filters;

  l.outputs = l.out_w * l.out_h * l.out_c;
  l.inputs = 1;
  l.index = index;

  l.nweights = l.out_w * l.out_h * l.out_c;

  adaptor->weight_updates(
      LayersBuffers::floats(LayersBuffers::count(l.nweights)));
  adaptor->weights(LayersBuffers::floats(LayersBuffers::count(l.nweights)));
  for (int i = 0; i < l.nweights; ++i) {
    l.weights[i] = mean_init + rand_uniform(-std_init, std_init);
  }

  adaptor->delta(LayersBuffers::floats(LayersBuffers::count(l.outputs, batch)));
  adaptor->output(
      LayersBuffers::floats(LayersBuffers::count(l.outputs, batch)));

  l.forward = forward_implicit_layer;
  l.backward = backward_implicit_layer;
  l.update = update_implicit_layer;

  return adaptor;
}

}  // namespace darknet_adaptor::adaptors::layers
