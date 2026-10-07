#include "src/lib/darknet-adaptor/adaptors/Layers/DetectionLayer.h"

#include <cassert>
#include <cstdlib>
#include <ctime>

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

ALayerAdaptorPtr DetectionLayer::make(int batch, int inputs, int n, int side,
                                      int classes, int coords, int rescore)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = DETECTION;

  l.n = n;
  l.batch = batch;
  l.inputs = inputs;
  l.classes = classes;
  l.coords = coords;
  l.rescore = rescore;
  l.side = side;
  l.w = side;
  l.h = side;
  assert(side * side * ((1 + l.coords) * l.n + l.classes) == inputs);
  adaptor->cost(LayersBuffers::floats(1U));
  l.outputs = l.inputs;
  l.truths = l.side * l.side * (1 + l.coords + l.classes);
  adaptor->output(
      LayersBuffers::floats(LayersBuffers::count(batch, l.outputs)));
  adaptor->delta(LayersBuffers::floats(LayersBuffers::count(batch, l.outputs)));

  l.forward = forward_detection_layer;
  l.backward = backward_detection_layer;

  CLOGIM("Detection Layer");
  srand(static_cast<unsigned int>(time(nullptr)));

  return adaptor;
}

}  // namespace darknet_adaptor::adaptors::layers
