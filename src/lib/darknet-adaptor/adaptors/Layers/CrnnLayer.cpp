#include "src/lib/darknet-adaptor/adaptors/Layers/CrnnLayer.h"

#include <cassert>

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ConvolutionalLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

void keep_workspace(layer& l, const layer& sub)
{
  if (l.workspace_size < sub.workspace_size) {
    l.workspace_size = sub.workspace_size;
  }
}

}  // namespace

ALayerAdaptorPtr CrnnLayer::make(int batch, int h, int w, int c,
                                 int hidden_filters, int output_filters,
                                 int groups, int steps, int size, int stride,
                                 int dilation, int pad, ACTIVATION activation,
                                 int batch_normalize, int xnor, int train)
{
  CLOGI("CRNN Layer: %d x %d x %d image, %d filters", h, w, c, output_filters);

  batch = batch / steps;

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.train = train;
  l.batch = batch;
  l.type = CRNN;
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
  l.hidden = h * w * hidden_filters;
  l.xnor = xnor;

  adaptor->state(buffers::floats(buffers::count(l.hidden, l.batch) *
                                 buffers::count(l.steps + 1)));

  adaptor->input_layer(ConvolutionalLayer::make(
      batch, steps, h, w, c, hidden_filters, groups, size, stride, stride,
      dilation, pad, activation, batch_normalize, 0, xnor, 0, 0, 0, 0, nullptr,
      0, 0, train));
  l.input_layer->batch = batch;
  keep_workspace(l, *l.input_layer);

  adaptor->self_layer(ConvolutionalLayer::make(
      batch, steps, h, w, hidden_filters, hidden_filters, groups, size, stride,
      stride, dilation, pad, activation, batch_normalize, 0, xnor, 0, 0, 0, 0,
      nullptr, 0, 0, train));
  l.self_layer->batch = batch;
  keep_workspace(l, *l.self_layer);

  adaptor->output_layer(ConvolutionalLayer::make(
      batch, steps, h, w, hidden_filters, output_filters, groups, size, stride,
      stride, dilation, pad, activation, batch_normalize, 0, xnor, 0, 0, 0, 0,
      nullptr, 0, 0, train));
  l.output_layer->batch = batch;
  keep_workspace(l, *l.output_layer);

  l.out_h = l.output_layer->out_h;
  l.out_w = l.output_layer->out_w;
  l.outputs = l.output_layer->outputs;

  assert(l.input_layer->outputs == l.self_layer->outputs);
  assert(l.input_layer->outputs == l.output_layer->inputs);

  adaptor->borrow_output(l.output_layer->output);
  adaptor->borrow_delta(l.output_layer->delta);

  l.forward = forward_crnn_layer;
  l.backward = backward_crnn_layer;
  l.update = update_crnn_layer;

  l.bflops =
      l.input_layer->bflops + l.self_layer->bflops + l.output_layer->bflops;

  return adaptor;
}

void CrnnLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  // the workspace of the resized sub-layers, as the layer was made (the
  // original kept the bigger one of the initial size as well)
  l.workspace_size = 0;

  ConvolutionalLayer::resize(*adaptor.input_layer(), w, h);
  keep_workspace(l, *l.input_layer);

  ConvolutionalLayer::resize(*adaptor.self_layer(), w, h);
  keep_workspace(l, *l.self_layer);

  ConvolutionalLayer::resize(*adaptor.output_layer(), w, h);
  keep_workspace(l, *l.output_layer);

  adaptor.borrow_output(l.output_layer->output);
  adaptor.borrow_delta(l.output_layer->delta);

  const int hidden_filters = l.self_layer->c;
  l.w = w;
  l.h = h;
  l.inputs = h * w * l.c;
  l.hidden = h * w * hidden_filters;

  l.out_h = l.output_layer->out_h;
  l.out_w = l.output_layer->out_w;
  l.outputs = l.output_layer->outputs;

  assert(l.input_layer->inputs == l.inputs);
  assert(l.self_layer->inputs == l.hidden);
  assert(l.input_layer->outputs == l.self_layer->outputs);
  assert(l.input_layer->outputs == l.output_layer->inputs);

  adaptor.state(buffers::resized(
      adaptor.state(),
      buffers::count(l.batch, l.hidden) * buffers::count(l.steps + 1)));
}

}  // namespace darknet_adaptor::adaptors::layers
