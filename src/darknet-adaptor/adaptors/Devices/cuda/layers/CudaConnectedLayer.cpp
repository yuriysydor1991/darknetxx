#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaConnectedLayer.h"

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/CudnnDescriptors.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

void CudaConnectedLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                               [[maybe_unused]] int index) const
{
  layer& l = *adaptor.get();
  const CudaArrays arrays{adaptor};
  const size_t weights = LayersBuffers::count(l.outputs, l.inputs);
  const size_t outputs = LayersBuffers::count(l.outputs);
  // the outputs of every step of the recurrent layer sub-layer
  const size_t size = adaptor.output().size();

  l.forward_gpu = forward_connected_layer_gpu;
  l.backward_gpu = backward_connected_layer_gpu;
  l.update_gpu = update_connected_layer_gpu;

  arrays.once(l.weights_gpu, l.weights, weights);
  arrays.once(l.biases_gpu, l.biases, outputs);
  arrays.once(l.weight_updates_gpu, l.weight_updates, weights);
  arrays.once(l.bias_updates_gpu, l.bias_updates, outputs);
  arrays.make(l.output_gpu, l.output, size);
  arrays.make(l.delta_gpu, l.delta, size);

  if (l.batch_normalize) {
    arrays.once(l.scales_gpu, l.scales, outputs);
    arrays.once(l.scale_updates_gpu, l.scale_updates, outputs);
    arrays.once(l.mean_gpu, l.mean, outputs);
    arrays.once(l.variance_gpu, l.variance, outputs);
    arrays.once(l.rolling_mean_gpu, l.rolling_mean, outputs);
    arrays.once(l.rolling_variance_gpu, l.rolling_variance, outputs);
    arrays.once(l.mean_delta_gpu, l.mean_delta, outputs);
    arrays.once(l.variance_delta_gpu, l.variance_delta, outputs);
    arrays.make(l.x_gpu, l.output, size);
    arrays.make(l.x_norm_gpu, l.output, size);
  }

  describe(l, net);
}

void CudaConnectedLayer::describe([[maybe_unused]] layer& l,
                                  [[maybe_unused]] ANetworkAdaptor& net) const
{
#ifdef CUDNN
  if (!l.srcTensorDesc) {
    create_convolutional_cudnn_tensors(&l);
    CudnnDescriptors::keep(net, l);
  }

  cudnn_convolutional_setup(&l, cudnn_fastest, 0);
  l.workspace_size = get_connected_workspace_size(l);
#endif  // CUDNN
}

void CudaConnectedLayer::push(const layer& l) const { push_connected_layer(l); }

void CudaConnectedLayer::pull(const layer& l) const { pull_connected_layer(l); }

}  // namespace darknet_adaptor::adaptors::devices::cuda
