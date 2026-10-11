#include "src/darknet-adaptor/adaptors/Layers/EmptyLayer.h"

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

extern "C" {

/// @brief The pass of the [empty] layer: the output and the delta are the
/// previous layer ones, so there is nothing to compute.
static void empty_func(layer l, network_state state)
{
  (void)l;
  (void)state;
}
}

namespace darknet_adaptor::adaptors::layers
{

ALayerAdaptorPtr EmptyLayer::make(int batch, int w, int h, int c, int inputs,
                                  float* output, float* delta)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = EMPTY;
  l.w = l.out_w = w;
  l.h = l.out_h = h;
  l.c = l.out_c = c;
  l.batch = batch;
  l.inputs = l.outputs = inputs;
  adaptor->borrow_output(output);
  adaptor->borrow_delta(delta);
  l.forward = empty_func;
  l.backward = empty_func;

  CLOGWM("empty");

  return adaptor;
}

}  // namespace darknet_adaptor::adaptors::layers
