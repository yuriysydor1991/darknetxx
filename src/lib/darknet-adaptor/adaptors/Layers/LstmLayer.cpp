#include "src/lib/darknet-adaptor/adaptors/Layers/LstmLayer.h"

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
ALayerAdaptorPtr sublayer(layer& l, int batch, int steps, int inputs,
                          int outputs, int batch_normalize)
{
  CLOGTM("\t\t");

  ALayerAdaptorPtr sub = ConnectedLayer::make(batch, steps, inputs, outputs,
                                              LINEAR, batch_normalize);
  layer& s = *sub->get();

  s.batch = batch;
  if (l.workspace_size < s.workspace_size) l.workspace_size = s.workspace_size;

  return sub;
}

}  // namespace

ALayerAdaptorPtr LstmLayer::make(int batch, int inputs, int outputs, int steps,
                                 int batch_normalize)
{
  CLOGI("LSTM Layer: %d inputs, %d outputs", inputs, outputs);

  batch = batch / steps;

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.batch = batch;
  l.type = LSTM;
  l.steps = steps;
  l.inputs = inputs;
  l.out_w = 1;
  l.out_h = 1;
  l.out_c = outputs;

  // the rand() order of the original: the weights of every sub-layer are
  // initialized as it is made
  adaptor->uf(sublayer(l, batch, steps, inputs, outputs, batch_normalize));
  adaptor->ui(sublayer(l, batch, steps, inputs, outputs, batch_normalize));
  adaptor->ug(sublayer(l, batch, steps, inputs, outputs, batch_normalize));
  adaptor->uo(sublayer(l, batch, steps, inputs, outputs, batch_normalize));

  adaptor->wf(sublayer(l, batch, steps, outputs, outputs, batch_normalize));
  adaptor->wi(sublayer(l, batch, steps, outputs, outputs, batch_normalize));
  adaptor->wg(sublayer(l, batch, steps, outputs, outputs, batch_normalize));
  adaptor->wo(sublayer(l, batch, steps, outputs, outputs, batch_normalize));

  l.batch_normalize = batch_normalize;
  l.outputs = outputs;

  adaptor->output(buffers::floats(buffers::count(outputs, batch, steps)));
  adaptor->state(buffers::floats(buffers::count(outputs, batch)));

  l.forward = forward_lstm_layer;
  l.update = update_lstm_layer;
  l.backward = backward_lstm_layer;

  const size_t state_size = buffers::count(batch, outputs);

  adaptor->prev_state_cpu(buffers::floats(state_size));
  adaptor->prev_cell_cpu(buffers::floats(state_size));
  adaptor->cell_cpu(buffers::floats(buffers::count(batch, outputs, steps)));

  adaptor->f_cpu(buffers::floats(state_size));
  adaptor->i_cpu(buffers::floats(state_size));
  adaptor->g_cpu(buffers::floats(state_size));
  adaptor->o_cpu(buffers::floats(state_size));
  adaptor->c_cpu(buffers::floats(state_size));
  adaptor->h_cpu(buffers::floats(state_size));
  adaptor->temp_cpu(buffers::floats(state_size));
  adaptor->temp2_cpu(buffers::floats(state_size));
  adaptor->temp3_cpu(buffers::floats(state_size));
  adaptor->dc_cpu(buffers::floats(state_size));
  adaptor->dh_cpu(buffers::floats(state_size));

  return adaptor;
}

}  // namespace darknet_adaptor::adaptors::layers
