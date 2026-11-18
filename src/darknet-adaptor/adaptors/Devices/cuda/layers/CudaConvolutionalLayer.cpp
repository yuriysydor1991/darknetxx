#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaConvolutionalLayer.h"

#include <algorithm>

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudnnDescriptors.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

void CudaConvolutionalLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                                   int index) const
{
  layer& l = *adaptor.get();
  const CudaArrays arrays{adaptor};
  const size_t weights = LayersBuffers::count(l.nweights);
  const size_t filters = LayersBuffers::count(l.n);
  // the outputs of every step of the recurrent layer sub-layer
  const size_t size = adaptor.output().size();

  l.forward_gpu = forward_convolutional_layer_gpu;
  l.backward_gpu = backward_convolutional_layer_gpu;
  l.update_gpu = update_convolutional_layer_gpu;

  place_weights(arrays, l);

  arrays.make(l.output_gpu, l.output, size);
  if (l.delta) arrays.make(l.delta_gpu, l.delta, size);

  if (l.activation_input) {
    arrays.make(l.activation_input_gpu, l.activation_input, size);
  }

  if (l.deform) arrays.once(l.weight_deform_gpu, weights);

  if (l.adam) {
    arrays.once(l.m_gpu, l.m, weights);
    arrays.once(l.v_gpu, l.v, weights);
    arrays.once(l.bias_m_gpu, l.bias_m, filters);
    arrays.once(l.bias_v_gpu, l.bias_v, filters);
    arrays.once(l.scale_m_gpu, l.scale_m, filters);
    arrays.once(l.scale_v_gpu, l.scale_v, filters);
  }

  if (l.binary || l.xnor) arrays.once(l.binary_weights_gpu, l.weights, weights);

  if (l.xnor) {
    arrays.once(l.mean_arr_gpu, filters);
    arrays.make(l.binary_input_gpu, LayersBuffers::count(l.inputs, l.batch));
  }

  if (l.batch_normalize) place_batchnorm(arrays, l, size);

  if (l.assisted_excitation) {
    const size_t cells = LayersBuffers::count(l.out_w, l.out_h, l.batch);
    arrays.make(l.gt_gpu, cells);
    arrays.make(l.a_avg_gpu, cells);
  }

  if (l.align_bit_weights) place_bits(arrays, l);

  describe(l, net);

  // the blur sub-layer of the antialiasing computes in it's own arrays
  if (l.antialiasing) {
    arrays.make(l.input_antialiasing_gpu,
                LayersBuffers::count(l.batch, l.outputs));
    l.workspace_size = std::max(l.workspace_size,
                                place_all({adaptor.input_layer()}, net, index));
  }
}

void CudaConvolutionalLayer::describe(
    layer& l, [[maybe_unused]] ANetworkAdaptor& net) const
{
#ifdef CUDNN
  if (!l.srcTensorDesc) {
    create_convolutional_cudnn_tensors(&l);
    CudnnDescriptors::keep(net, l);
  }

  cudnn_convolutional_setup(&l, cudnn_fastest, 0);
#endif  // CUDNN

  l.workspace_size = get_convolutional_workspace_size(l);
}

void CudaConvolutionalLayer::push(const layer& l) const
{
  if (!l.share_layer) push_convolutional_layer(l);
}

void CudaConvolutionalLayer::pull(const layer& l) const
{
  if (!l.share_layer) pull_convolutional_layer(l);
}

void CudaConvolutionalLayer::place_weights(const CudaArrays& arrays, layer& l)
{
  if (l.share_layer) {
    const layer& shared = *l.share_layer;

    l.weights_gpu = shared.weights_gpu;
    l.weight_updates_gpu = shared.weight_updates_gpu;
    l.weights_gpu16 = shared.weights_gpu16;
    l.weight_updates_gpu16 = shared.weight_updates_gpu16;
    l.biases_gpu = shared.biases_gpu;
    l.bias_updates_gpu = shared.bias_updates_gpu;
    return;
  }

  const size_t weights = LayersBuffers::count(l.nweights);
  const size_t filters = LayersBuffers::count(l.n);

  arrays.once(l.weights_gpu, l.weights, weights);
  arrays.once(l.biases_gpu, l.biases, filters);

  // the training arrays of the layers made for the training, the frozen
  // ones included
  if (l.weight_updates) {
    arrays.once(l.weight_updates_gpu, l.weight_updates, weights);
    arrays.once(l.bias_updates_gpu, l.bias_updates, filters);
  }

#ifdef CUDNN_HALF
  const size_t halves = LayersBuffers::count(l.nweights / 2 + 1);

  arrays.once(l.weights_gpu16, halves);
  if (l.weight_updates) arrays.once(l.weight_updates_gpu16, halves);
#endif  // CUDNN_HALF
}

void CudaConvolutionalLayer::place_batchnorm(const CudaArrays& arrays, layer& l,
                                             size_t size)
{
  if (l.share_layer) {
    const layer& shared = *l.share_layer;

    l.scales_gpu = shared.scales_gpu;
    l.scale_updates_gpu = shared.scale_updates_gpu;
    l.mean_gpu = shared.mean_gpu;
    l.variance_gpu = shared.variance_gpu;
    l.rolling_mean_gpu = shared.rolling_mean_gpu;
    l.rolling_variance_gpu = shared.rolling_variance_gpu;
    l.mean_delta_gpu = shared.mean_delta_gpu;
    l.variance_delta_gpu = shared.variance_delta_gpu;
  } else {
    const size_t filters = LayersBuffers::count(l.n);

    arrays.once(l.scales_gpu, l.scales, filters);
    arrays.once(l.rolling_mean_gpu, l.rolling_mean, filters);
    arrays.once(l.rolling_variance_gpu, l.rolling_variance, filters);

    if (l.mean) {
      arrays.once(l.scale_updates_gpu, l.scale_updates, filters);
      arrays.once(l.mean_gpu, l.mean, filters);
      arrays.once(l.variance_gpu, l.variance, filters);
      arrays.once(l.m_cbn_avg_gpu, l.mean, filters);
      arrays.once(l.v_cbn_avg_gpu, l.variance, filters);
#ifndef CUDNN
      arrays.once(l.mean_delta_gpu, l.mean_delta, filters);
      arrays.once(l.variance_delta_gpu, l.variance_delta, filters);
#endif
    }
  }

  if (l.x) {
    arrays.make(l.x_gpu, l.output, size);
#ifndef CUDNN
    arrays.make(l.x_norm_gpu, l.output, size);
#endif
  }
}

void CudaConvolutionalLayer::place_bits(const CudaArrays& arrays, layer& l)
{
  l.align_workspace_size = l.bit_align * l.size * l.size * l.c;

  const size_t workspace = LayersBuffers::count(l.align_workspace_size);

  arrays.make(l.align_workspace_gpu, workspace);
  arrays.make(l.transposed_align_workspace_gpu, workspace);
  arrays.make(l.align_bit_weights_gpu, l.align_bit_weights,
              LayersBuffers::count(l.align_bit_weights_size));

  cuda_push_array(l.binary_weights_gpu, l.binary_weights,
                  LayersBuffers::count(l.nweights));
  cuda_push_array(l.mean_arr_gpu, l.mean_arr, LayersBuffers::count(l.n));
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
