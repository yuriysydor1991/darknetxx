#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaBatchnormLayer.h"

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/CudnnDescriptors.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

void CudaBatchnormLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                               [[maybe_unused]] int index) const
{
  layer& l = *adaptor.get();
  const CudaArrays arrays{adaptor};
  const size_t size = LayersBuffers::count(l.outputs, l.batch);
  const size_t channels = LayersBuffers::count(l.c);

  l.forward_gpu = forward_batchnorm_layer_gpu;
  l.backward_gpu = backward_batchnorm_layer_gpu;
  l.update_gpu = update_batchnorm_layer_gpu;

  arrays.make(l.output_gpu, l.output, size);
  arrays.once(l.biases_gpu, l.biases, channels);
  arrays.once(l.scales_gpu, l.scales, channels);
  arrays.once(l.mean_gpu, l.mean, channels);
  arrays.once(l.variance_gpu, l.variance, channels);
  arrays.once(l.rolling_mean_gpu, l.rolling_mean, channels);
  arrays.once(l.rolling_variance_gpu, l.rolling_variance, channels);

  arrays.once(l.bias_updates_gpu, l.bias_updates, channels);
  arrays.once(l.scale_updates_gpu, l.scale_updates, channels);
  arrays.once(l.mean_delta_gpu, l.mean_delta, channels);
  arrays.once(l.variance_delta_gpu, l.variance_delta, channels);
  arrays.make(l.delta_gpu, l.delta, size);

  // the training arrays of the layer made for the training
  if (l.x) {
    arrays.make(l.x_gpu, l.output, size);
#ifndef CUDNN
    arrays.make(l.x_norm_gpu, l.output, size);
#endif
  }

  describe(l, net);
}

void CudaBatchnormLayer::describe([[maybe_unused]] layer& l,
                                  [[maybe_unused]] ANetworkAdaptor& net) const
{
#ifdef CUDNN
  if (!l.normTensorDesc) {
    CHECK_CUDNN(cudnnCreateTensorDescriptor(&l.normTensorDesc));
    CHECK_CUDNN(cudnnCreateTensorDescriptor(&l.normDstTensorDesc));
    CudnnDescriptors::keep(net, l);
  }

  CHECK_CUDNN(cudnnSetTensor4dDescriptor(l.normDstTensorDesc, CUDNN_TENSOR_NCHW,
                                         CUDNN_DATA_FLOAT, l.batch, l.out_c,
                                         l.out_h, l.out_w));
  CHECK_CUDNN(cudnnSetTensor4dDescriptor(l.normTensorDesc, CUDNN_TENSOR_NCHW,
                                         CUDNN_DATA_FLOAT, 1, l.out_c, 1, 1));
#endif  // CUDNN
}

void CudaBatchnormLayer::push(const layer& l) const { push_batchnorm_layer(l); }

void CudaBatchnormLayer::pull(const layer& l) const { pull_batchnorm_layer(l); }

}  // namespace darknet_adaptor::adaptors::devices::cuda
