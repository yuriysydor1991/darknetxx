#include "src/lib/darknet-adaptor/adaptors/Layers/ShortcutLayer.h"

#include <cassert>
#include <utility>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

bool keeps_activation_input(ACTIVATION activation)
{
  return activation == SWISH || activation == MISH;
}

}  // namespace

ALayerAdaptorPtr ShortcutLayer::make(
    int batch, std::vector<int> input_layers, std::vector<int> input_sizes,
    int w, int h, int c, vrfloats layers_output, vrfloats layers_delta,
    WEIGHTS_TYPE_T weights_type, WEIGHTS_NORMALIZATION_T weights_normalization,
    ACTIVATION activation, int train)
{
  CLOGIM("Shortcut Layer: ");

  const int n = static_cast<int>(input_layers.size());

  for (int i = 0; i < n; ++i) {
    CLOGI("input_layers[%d] = %d, ", i, input_layers[static_cast<size_t>(i)]);
  }

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.train = train;
  l.type = SHORTCUT;
  l.batch = batch;
  l.activation = activation;
  l.n = n;
  adaptor->input_layers(std::move(input_layers));
  adaptor->input_sizes(std::move(input_sizes));
  adaptor->layers_output(std::move(layers_output));
  adaptor->layers_delta(std::move(layers_delta));
  l.weights_type = weights_type;
  l.weights_normalization = weights_normalization;
  l.learning_rate_scale = 1;  // not necessary

  l.w = l.out_w = w;
  l.h = l.out_h = h;
  l.c = l.out_c = c;
  l.outputs = w * h * c;
  l.inputs = l.outputs;

  l.index = l.input_layers[0];

  if (train) {
    adaptor->delta(buffers::floats(buffers::count(l.outputs, batch)));
  }
  adaptor->output(buffers::floats(buffers::count(l.outputs, batch)));

  l.nweights = 0;
  if (l.weights_type == PER_FEATURE) {
    l.nweights = (l.n + 1);
  } else if (l.weights_type == PER_CHANNEL) {
    l.nweights = (l.n + 1) * l.c;
  }

  if (l.nweights > 0) {
    adaptor->weights(buffers::floats(buffers::count(l.nweights), 1.f));

    if (train) {
      adaptor->weight_updates(buffers::floats(buffers::count(l.nweights)));
    }
    l.update = update_shortcut_layer;
  }

  l.forward = forward_shortcut_layer;
  l.backward = backward_shortcut_layer;
  if (keeps_activation_input(l.activation)) {
    adaptor->activation_input(
        buffers::floats(buffers::count(l.batch, l.outputs)));
  }

  l.bflops =
      static_cast<float>(l.out_w * l.out_h * l.out_c * l.n / 1000000000.);
  if (l.weights_type) l.bflops *= 2;
  CLOGI(" wt = %d, wn = %d, outputs:%4d x%4d x%4d %5.3f BF", l.weights_type,
        l.weights_normalization, l.out_w, l.out_h, l.out_c,
        static_cast<double>(l.bflops));

  return adaptor;
}

void ShortcutLayer::resize(ALayerAdaptor& adaptor, int w, int h,
                           const network& net)
{
  layer& l = *adaptor.get();

  l.w = l.out_w = w;
  l.h = l.out_h = h;
  l.outputs = w * h * l.out_c;
  l.inputs = l.outputs;

  const size_t size = buffers::count(l.outputs, l.batch);

  if (l.train) {
    adaptor.delta(buffers::resized(adaptor.delta(), size));
  }
  adaptor.output(buffers::resized(adaptor.output(), size));

  for (int i = 0; i < l.n; ++i) {
    const int index = l.input_layers[i];
    l.input_sizes[i] = net.layers[index].outputs;
    l.layers_output[i] = net.layers[index].output;
    l.layers_delta[i] = net.layers[index].delta;

    assert(l.w == net.layers[index].out_w && l.h == net.layers[index].out_h);
  }

  if (keeps_activation_input(l.activation)) {
    adaptor.activation_input(
        buffers::resized(adaptor.activation_input(), size));
  }
}

}  // namespace darknet_adaptor::adaptors::layers
