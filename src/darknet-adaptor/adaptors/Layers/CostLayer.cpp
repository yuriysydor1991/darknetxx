#include "src/darknet-adaptor/adaptors/Layers/CostLayer.h"

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

ALayerAdaptorPtr CostLayer::make(int batch, int inputs, COST_TYPE cost_type,
                                 float scale)
{
  CLOGI("cost                                           %4d", inputs);

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = COST;

  l.scale = scale;
  l.batch = batch;
  l.inputs = inputs;
  l.outputs = inputs;
  l.cost_type = cost_type;
  adaptor->delta(LayersBuffers::floats(LayersBuffers::count(inputs, batch)));
  adaptor->output(LayersBuffers::floats(LayersBuffers::count(inputs, batch)));
  adaptor->cost(LayersBuffers::floats(1U));

  l.forward = forward_cost_layer;
  l.backward = backward_cost_layer;

  return adaptor;
}

void CostLayer::resize(ALayerAdaptor& adaptor, int inputs)
{
  layer& l = *adaptor.get();

  l.inputs = inputs;
  l.outputs = inputs;

  const size_t size = LayersBuffers::count(inputs, l.batch);

  adaptor.delta(LayersBuffers::resized(adaptor.delta(), size));
  adaptor.output(LayersBuffers::resized(adaptor.output(), size));
}

}  // namespace darknet_adaptor::adaptors::layers
