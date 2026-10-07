#include "src/lib/darknet-adaptor/adaptors/Layers/GruLayer.h"

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ConnectedLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

/// @brief The connected sub-layer of the mini batch of a single step.
ALayerAdaptorPtr sublayer(int batch, int steps, int inputs, int outputs,
                          int batch_normalize)
{
  CLOGTM("\t\t");

  ALayerAdaptorPtr sub = ConnectedLayer::make(batch, steps, inputs, outputs,
                                              LINEAR, batch_normalize);
  sub->get()->batch = batch;

  return sub;
}

}  // namespace

ALayerAdaptorPtr GruLayer::make(int batch, int inputs, int outputs, int steps,
                                int batch_normalize)
{
  CLOGI("GRU Layer: %d inputs, %d outputs", inputs, outputs);

  batch = batch / steps;

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.batch = batch;
  l.type = GRU;
  l.steps = steps;
  l.inputs = inputs;

  // the rand() order of the original: the weights of every sub-layer are
  // initialized as it is made
  adaptor->input_z_layer(
      sublayer(batch, steps, inputs, outputs, batch_normalize));
  adaptor->state_z_layer(
      sublayer(batch, steps, outputs, outputs, batch_normalize));
  adaptor->input_r_layer(
      sublayer(batch, steps, inputs, outputs, batch_normalize));
  adaptor->state_r_layer(
      sublayer(batch, steps, outputs, outputs, batch_normalize));
  adaptor->input_h_layer(
      sublayer(batch, steps, inputs, outputs, batch_normalize));
  adaptor->state_h_layer(
      sublayer(batch, steps, outputs, outputs, batch_normalize));

  l.batch_normalize = batch_normalize;

  l.outputs = outputs;
  adaptor->output(buffers::floats(buffers::count(outputs, batch, steps)));
  adaptor->delta(buffers::floats(buffers::count(outputs, batch, steps)));
  adaptor->state(buffers::floats(buffers::count(outputs, batch)));
  adaptor->prev_state(buffers::floats(buffers::count(outputs, batch)));
  adaptor->forgot_state(buffers::floats(buffers::count(outputs, batch)));
  adaptor->forgot_delta(buffers::floats(buffers::count(outputs, batch)));

  adaptor->r_cpu(buffers::floats(buffers::count(outputs, batch)));
  adaptor->z_cpu(buffers::floats(buffers::count(outputs, batch)));
  adaptor->h_cpu(buffers::floats(buffers::count(outputs, batch)));

  l.forward = forward_gru_layer;
  l.backward = backward_gru_layer;
  l.update = update_gru_layer;

  return adaptor;
}

}  // namespace darknet_adaptor::adaptors::layers
