#include "src/darknet-adaptor/adaptors/Layers/ConvLstmLayer.h"

#include <cassert>

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/Layers/ConvolutionalLayer.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

/// @brief The parameters every convolutional sub-layer gets.
struct SubParams
{
  int batch;
  int steps;
  int h;
  int w;
  int groups;
  int size;
  int stride;
  int dilation;
  int pad;
  ACTIVATION activation;
  int batch_normalize;
  int xnor;
  int train;
};

ALayerAdaptorPtr sublayer(layer& l, const SubParams& p, int c, int n)
{
  ALayerAdaptorPtr sub = ConvolutionalLayer::make(
      p.batch, p.steps, p.h, p.w, c, n, p.groups, p.size, p.stride, p.stride,
      p.dilation, p.pad, p.activation, p.batch_normalize, 0, p.xnor, 0, 0, 0, 0,
      nullptr, 0, 0, p.train);
  layer& s = *sub->get();

  s.batch = p.batch;
  if (l.workspace_size < s.workspace_size) l.workspace_size = s.workspace_size;

  return sub;
}

/// @brief The sub-layer the original allocated zeroed, but never made.
ALayerAdaptorPtr holder() { return ALayerAdaptor::create(); }

void resize_sublayer(layer& l, ALayerAdaptor& sub, int w, int h)
{
  ConvolutionalLayer::resize(sub, w, h);

  if (l.workspace_size < sub.get()->workspace_size) {
    l.workspace_size = sub.get()->workspace_size;
  }
}

}  // namespace

ALayerAdaptorPtr ConvLstmLayer::make(int batch, int h, int w, int c,
                                     int output_filters, int groups, int steps,
                                     int size, int stride, int dilation,
                                     int pad, ACTIVATION activation,
                                     int batch_normalize, int peephole,
                                     int xnor, int bottleneck, int train)
{
  CLOGI("CONV_LSTM Layer: %d x %d x %d image, %d filters", h, w, c,
        output_filters);

  batch = batch / steps;

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.train = train;
  l.batch = batch;
  l.type = CONV_LSTM;
  l.bottleneck = bottleneck;
  l.steps = steps;
  l.size = size;
  l.stride = stride;
  l.dilation = dilation;
  l.pad = pad;
  l.h = h;
  l.w = w;
  l.c = c;
  l.groups = groups;
  l.out_c = output_filters;
  l.inputs = h * w * c;
  l.xnor = xnor;
  l.peephole = peephole;

  const SubParams p{batch,  steps,    h,   w,          groups,          size,
                    stride, dilation, pad, activation, batch_normalize, xnor,
                    train};

  // U, the rand() order of the original: the weights of every sub-layer are
  // initialized as it is made
  adaptor->uf(sublayer(l, p, c, output_filters));
  adaptor->ui(sublayer(l, p, c, output_filters));
  adaptor->ug(sublayer(l, p, c, output_filters));
  adaptor->uo(sublayer(l, p, c, output_filters));

  if (l.bottleneck) {
    // bottleneck-conv with 2x channels
    adaptor->wf(sublayer(l, p, output_filters * 2, output_filters));
    adaptor->wi(holder());
    adaptor->wg(holder());
    adaptor->wo(holder());
  } else {
    // W
    adaptor->wf(sublayer(l, p, output_filters, output_filters));
    adaptor->wi(sublayer(l, p, output_filters, output_filters));
    adaptor->wg(sublayer(l, p, output_filters, output_filters));
    adaptor->wo(sublayer(l, p, output_filters, output_filters));
  }

  // V
  if (l.peephole) {
    adaptor->vf(sublayer(l, p, output_filters, output_filters));
    adaptor->vi(sublayer(l, p, output_filters, output_filters));
    adaptor->vo(sublayer(l, p, output_filters, output_filters));
  } else {
    adaptor->vf(holder());
    adaptor->vi(holder());
    adaptor->vo(holder());
  }

  l.batch_normalize = batch_normalize;

  l.out_h = l.uo->out_h;
  l.out_w = l.uo->out_w;
  l.outputs = l.uo->outputs;
  const int outputs = l.outputs;
  l.inputs = w * h * c;

  if (!l.bottleneck) assert(l.wo->outputs == l.uo->outputs);
  assert(l.wf->outputs == l.uf->outputs);

  adaptor->output(buffers::floats(buffers::count(outputs, batch, steps)));

  l.forward = forward_conv_lstm_layer;
  l.update = update_conv_lstm_layer;
  l.backward = backward_conv_lstm_layer;

  const size_t state_size = buffers::count(batch, outputs);

  adaptor->prev_state_cpu(buffers::floats(state_size));
  adaptor->prev_cell_cpu(buffers::floats(state_size));
  adaptor->cell_cpu(buffers::floats(buffers::count(batch, outputs, steps)));

  adaptor->f_cpu(buffers::floats(state_size));
  adaptor->i_cpu(buffers::floats(state_size));
  adaptor->g_cpu(buffers::floats(state_size));
  adaptor->o_cpu(buffers::floats(state_size));
  adaptor->c_cpu(buffers::floats(state_size));
  adaptor->stored_c_cpu(buffers::floats(state_size));
  adaptor->h_cpu(buffers::floats(state_size));
  adaptor->stored_h_cpu(buffers::floats(state_size));
  adaptor->temp_cpu(buffers::floats(state_size));
  adaptor->temp2_cpu(buffers::floats(state_size));
  adaptor->temp3_cpu(buffers::floats(state_size));
  adaptor->dc_cpu(buffers::floats(state_size));
  adaptor->dh_cpu(buffers::floats(state_size));

  l.bflops = l.uf->bflops + l.ui->bflops + l.ug->bflops + l.uo->bflops +
             l.wf->bflops + l.wi->bflops + l.wg->bflops + l.wo->bflops +
             l.vf->bflops + l.vi->bflops + l.vo->bflops;

  if (l.peephole) {
    l.bflops = static_cast<float>(static_cast<double>(l.bflops) +
                                  12 * l.outputs * l.batch / 1000000000.);
  } else {
    l.bflops = static_cast<float>(static_cast<double>(l.bflops) +
                                  9 * l.outputs * l.batch / 1000000000.);
  }

  return adaptor;
}

ALayerAdaptorPtr ConvLstmLayer::make_history(int batch, int h, int w, int c,
                                             int history_size, int steps,
                                             int train)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.train = train;
  l.batch = batch;
  l.type = HISTORY;
  l.steps = steps;
  l.history_size = history_size;
  l.h = h;
  l.w = w;
  l.c = c;
  l.out_h = h;
  l.out_w = w;
  l.out_c = c * history_size;
  l.inputs = h * w * c;
  l.outputs = h * w * c * history_size;

  l.forward = forward_history_layer;
  l.backward = backward_history_layer;

  CLOGI(
      "HISTORY b = %d, s = %2d, steps = %2d   %4d x%4d x%4d -> %4d x%4d "
      "x%4d",
      l.batch / l.steps, l.history_size, l.steps, w, h, c, l.out_w, l.out_h,
      l.out_c);

  adaptor->output(buffers::floats(buffers::count(l.batch, l.outputs)));
  adaptor->delta(buffers::floats(buffers::count(l.batch, l.outputs)));

  adaptor->prev_state_cpu(buffers::floats(buffers::count(l.batch, l.outputs)));

  return adaptor;
}

void ConvLstmLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  // the workspace of the resized sub-layers, as the layer was made (the
  // original kept the bigger one of the initial size as well)
  l.workspace_size = 0;

  if (l.peephole) {
    resize_sublayer(l, *adaptor.vf(), w, h);
    resize_sublayer(l, *adaptor.vi(), w, h);
    resize_sublayer(l, *adaptor.vo(), w, h);
  }

  resize_sublayer(l, *adaptor.wf(), w, h);

  // the original resized the never made wi, wg and wo holders of the
  // bottleneck layer as well, dividing by their zero stride
  if (!l.bottleneck) {
    resize_sublayer(l, *adaptor.wi(), w, h);
    resize_sublayer(l, *adaptor.wg(), w, h);
    resize_sublayer(l, *adaptor.wo(), w, h);
  }

  resize_sublayer(l, *adaptor.uf(), w, h);
  resize_sublayer(l, *adaptor.ui(), w, h);
  resize_sublayer(l, *adaptor.ug(), w, h);
  resize_sublayer(l, *adaptor.uo(), w, h);

  l.w = w;
  l.h = h;
  // the uo outputs, as the layer took them when made: the wo ones for the
  // bottleneck-less layer, which the original took here
  l.out_h = l.uo->out_h;
  l.out_w = l.uo->out_w;
  l.outputs = l.uo->outputs;
  const int outputs = l.outputs;
  l.inputs = w * h * l.c;
  const int steps = l.steps;
  const int batch = l.batch;

  if (!l.bottleneck) assert(l.wo->outputs == l.uo->outputs);

  adaptor.output(buffers::resized(adaptor.output(),
                                  buffers::count(outputs, batch, steps)));

  const size_t state_size = buffers::count(batch, outputs);

  adaptor.prev_state_cpu(
      buffers::resized(adaptor.prev_state_cpu(), state_size));
  adaptor.prev_cell_cpu(buffers::resized(adaptor.prev_cell_cpu(), state_size));
  adaptor.cell_cpu(buffers::resized(adaptor.cell_cpu(),
                                    buffers::count(batch, outputs, steps)));

  adaptor.f_cpu(buffers::resized(adaptor.f_cpu(), state_size));
  adaptor.i_cpu(buffers::resized(adaptor.i_cpu(), state_size));
  adaptor.g_cpu(buffers::resized(adaptor.g_cpu(), state_size));
  adaptor.o_cpu(buffers::resized(adaptor.o_cpu(), state_size));
  adaptor.c_cpu(buffers::resized(adaptor.c_cpu(), state_size));
  adaptor.h_cpu(buffers::resized(adaptor.h_cpu(), state_size));
  adaptor.temp_cpu(buffers::resized(adaptor.temp_cpu(), state_size));
  adaptor.temp2_cpu(buffers::resized(adaptor.temp2_cpu(), state_size));
  adaptor.temp3_cpu(buffers::resized(adaptor.temp3_cpu(), state_size));
  adaptor.dc_cpu(buffers::resized(adaptor.dc_cpu(), state_size));
  adaptor.dh_cpu(buffers::resized(adaptor.dh_cpu(), state_size));
  adaptor.stored_c_cpu(buffers::resized(adaptor.stored_c_cpu(), state_size));
  adaptor.stored_h_cpu(buffers::resized(adaptor.stored_h_cpu(), state_size));
}

}  // namespace darknet_adaptor::adaptors::layers
