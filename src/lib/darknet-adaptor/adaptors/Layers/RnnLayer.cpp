#include "src/lib/darknet-adaptor/adaptors/Layers/RnnLayer.h"

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ConnectedLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

/// @brief The connected sub-layer of the mini batch of a single step.
ALayerAdaptorPtr sublayer(layer& l, int batch, int steps, int inputs,
                          int outputs, ACTIVATION activation,
                          int batch_normalize)
{
  CLOGIM("\t\t");

  ALayerAdaptorPtr sub = ConnectedLayer::make(batch, steps, inputs, outputs,
                                              activation, batch_normalize);
  layer& s = *sub->get();

  s.batch = batch;
  if (l.workspace_size < s.workspace_size) l.workspace_size = s.workspace_size;

  return sub;
}

}  // namespace

ALayerAdaptorPtr RnnLayer::make(int batch, int inputs, int hidden, int outputs,
                                int steps, ACTIVATION activation,
                                int batch_normalize, int log)
{
  CLOGI("RNN Layer: %d inputs, %d outputs", inputs, outputs);

  batch = batch / steps;

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.batch = batch;
  l.type = RNN;
  l.steps = steps;
  l.hidden = hidden;
  l.inputs = inputs;
  l.out_w = 1;
  l.out_h = 1;
  l.out_c = outputs;

  adaptor->state(LayersBuffers::floats(LayersBuffers::count(batch, hidden) *
                                       LayersBuffers::count(steps + 1)));

  adaptor->input_layer(
      sublayer(l, batch, steps, inputs, hidden, activation, batch_normalize));
  adaptor->self_layer(
      sublayer(l, batch, steps, hidden, hidden,
               (log == 2) ? LOGGY : (log == 1 ? LOGISTIC : activation),
               batch_normalize));
  adaptor->output_layer(
      sublayer(l, batch, steps, hidden, outputs, activation, batch_normalize));

  l.outputs = outputs;
  adaptor->borrow_output(l.output_layer->output);
  adaptor->borrow_delta(l.output_layer->delta);

  l.forward = forward_rnn_layer;
  l.backward = backward_rnn_layer;
  l.update = update_rnn_layer;

  return adaptor;
}

}  // namespace darknet_adaptor::adaptors::layers
