#include "src/lib/darknet-adaptor/adaptors/Layers/ActivationLayer.h"

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

ALayerAdaptorPtr ActivationLayer::make(int batch, int inputs,
                                       ACTIVATION activation)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = ACTIVE;

  l.inputs = inputs;
  l.outputs = inputs;
  l.batch = batch;

  adaptor->output(LayersBuffers::floats(LayersBuffers::count(batch, inputs)));
  adaptor->delta(LayersBuffers::floats(LayersBuffers::count(batch, inputs)));

  l.forward = forward_activation_layer;
  l.backward = backward_activation_layer;
  l.activation = activation;

  CLOGI("Activation Layer: %d inputs", inputs);

  return adaptor;
}

}  // namespace darknet_adaptor::adaptors::layers
