#include "src/darknet-adaptor/adaptors/Layers/ConnectedLayer.h"

#include <cmath>

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

}  // namespace

ALayerAdaptorPtr ConnectedLayer::make(int batch, int steps, int inputs,
                                      int outputs, ACTIVATION activation,
                                      int batch_normalize)
{
  const int total_batch = batch * steps;

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = CONNECTED;

  l.inputs = inputs;
  l.outputs = outputs;
  l.batch = batch;
  l.batch_normalize = batch_normalize;
  l.h = 1;
  l.w = 1;
  l.c = inputs;
  l.out_h = 1;
  l.out_w = 1;
  l.out_c = outputs;
  l.n = l.out_c;
  l.size = 1;
  l.stride = l.stride_x = l.stride_y = 1;
  l.pad = 0;
  l.activation = activation;
  l.learning_rate_scale = 1;
  l.groups = 1;
  l.dilation = 1;

  adaptor->output(buffers::floats(buffers::count(total_batch, outputs)));
  adaptor->delta(buffers::floats(buffers::count(total_batch, outputs)));

  adaptor->weight_updates(buffers::floats(buffers::count(inputs, outputs)));
  adaptor->bias_updates(buffers::floats(buffers::count(outputs)));

  adaptor->weights(buffers::floats(buffers::count(outputs, inputs)));
  adaptor->biases(buffers::floats(buffers::count(outputs)));

  l.forward = forward_connected_layer;
  l.backward = backward_connected_layer;
  l.update = update_connected_layer;

  // the double precision sqrt() of the original C call
  const float scale = static_cast<float>(
      std::sqrt(static_cast<double>(2.f / static_cast<float>(inputs))));
  for (int i = 0; i < outputs * inputs; ++i) {
    l.weights[i] = scale * rand_uniform(-1, 1);
  }

  if (batch_normalize) {
    adaptor->scales(buffers::floats(buffers::count(outputs), 1.f));
    adaptor->scale_updates(buffers::floats(buffers::count(outputs)));

    adaptor->mean(buffers::floats(buffers::count(outputs)));
    adaptor->mean_delta(buffers::floats(buffers::count(outputs)));
    adaptor->variance(buffers::floats(buffers::count(outputs)));
    adaptor->variance_delta(buffers::floats(buffers::count(outputs)));

    adaptor->rolling_mean(buffers::floats(buffers::count(outputs)));
    adaptor->rolling_variance(buffers::floats(buffers::count(outputs)));

    adaptor->x(buffers::floats(buffers::count(total_batch, outputs)));
    adaptor->x_norm(buffers::floats(buffers::count(total_batch, outputs)));
  }

  CLOGI("connected                            %4d  ->  %4d\n", inputs, outputs);

  return adaptor;
}

}  // namespace darknet_adaptor::adaptors::layers
