#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaShortcutLayer.h"

#include <vector>

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

void CudaShortcutLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                              [[maybe_unused]] int index) const
{
  layer& l = *adaptor.get();
  const network& view = *net.get();
  const CudaArrays arrays{adaptor};
  const size_t size = LayersBuffers::count(l.outputs, l.batch);

  l.forward_gpu = forward_shortcut_layer_gpu;
  l.backward_gpu = backward_shortcut_layer_gpu;

  if (l.nweights > 0) {
    const size_t weights = LayersBuffers::count(l.nweights);

    l.update_gpu = update_shortcut_layer_gpu;
    arrays.once(l.weights_gpu, l.weights, weights);
    if (l.weight_updates) {
      arrays.once(l.weight_updates_gpu, l.weight_updates, weights);
    }
  }

  if (l.delta) arrays.make(l.delta_gpu, l.delta, size);
  arrays.make(l.output_gpu, l.output, size);

  if (l.activation_input) {
    arrays.make(l.activation_input_gpu, l.activation_input, size);
  }

  std::vector<float*> outputs;
  std::vector<float*> deltas;

  for (int i = 0; i < l.n; ++i) {
    outputs.push_back(view.layers[l.input_layers[i]].output_gpu);
    deltas.push_back(view.layers[l.input_layers[i]].delta_gpu);
  }

  arrays.make(l.input_sizes_gpu, l.input_sizes, outputs.size());
  arrays.make(l.layers_output_gpu, outputs.data(), outputs.size());
  arrays.make(l.layers_delta_gpu, deltas.data(), deltas.size());
}

void CudaShortcutLayer::push(const layer& l) const
{
  if (l.nweights > 0) push_shortcut_layer(l);
}

void CudaShortcutLayer::pull(const layer& l) const
{
  if (l.nweights > 0) pull_shortcut_layer(l);
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
