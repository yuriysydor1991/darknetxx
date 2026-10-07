#include "layers/representation_layer.h"

#include <assert.h>
#include <stdio.h>

#include "blas.h"
#include "dark_cuda.h"
#include "src/clog/clog.h"
#include "utils.h"

void forward_implicit_layer(const layer l, network_state state)
{
  int i;
#pragma omp parallel for
  for (i = 0; i < l.nweights * l.batch; ++i) {
    l.output[i] = l.weights[i % l.nweights];
  }
}

void backward_implicit_layer(const layer l, network_state state)
{
  int i;
  for (i = 0; i < l.nweights * l.batch; ++i) {
    l.weight_updates[i % l.nweights] += l.delta[i];
  }
}

void update_implicit_layer(layer l, int batch, float learning_rate_init,
                           float momentum, float decay)
{
  float learning_rate = learning_rate_init * l.learning_rate_scale;
  // float momentum = a.momentum;
  // float decay = a.decay;
  // int batch = a.batch;

  axpy_cpu(l.nweights, -decay * batch, l.weights, 1, l.weight_updates, 1);
  axpy_cpu(l.nweights, learning_rate / batch, l.weight_updates, 1, l.weights,
           1);
  scal_cpu(l.nweights, momentum, l.weight_updates, 1);
}

#ifdef GPU
void forward_implicit_layer_gpu(const layer l, network_state state)
{
  forward_implicit_gpu(l.batch, l.nweights, l.weights_gpu, l.output_gpu);
}

void backward_implicit_layer_gpu(const layer l, network_state state)
{
  backward_implicit_gpu(l.batch, l.nweights, l.weight_updates_gpu, l.delta_gpu);
}

void update_implicit_layer_gpu(layer l, int batch, float learning_rate_init,
                               float momentum, float decay, float loss_scale)
{
  // Loss scale for Mixed-Precision on Tensor-Cores
  float learning_rate = learning_rate_init * l.learning_rate_scale / loss_scale;
  // float momentum = a.momentum;
  // float decay = a.decay;
  // int batch = a.batch;

  reset_nan_and_inf(l.weight_updates_gpu, l.nweights);
  fix_nan_and_inf(l.weights_gpu, l.nweights);

  if (l.adam) {
    // adam_update_gpu(l.weights_gpu, l.weight_updates_gpu, l.m_gpu, l.v_gpu,
    // a.B1, a.B2, a.eps, decay, learning_rate, l.nweights, batch, a.t);
    adam_update_gpu(l.weights_gpu, l.weight_updates_gpu, l.m_gpu, l.v_gpu, l.B1,
                    l.B2, l.eps, decay, learning_rate, l.nweights, batch, l.t);
  } else {
    // axpy_ongpu(l.nweights, -decay*batch, l.weights_gpu, 1,
    // l.weight_updates_gpu, 1); axpy_ongpu(l.nweights, learning_rate / batch,
    // l.weight_updates_gpu, 1, l.weights_gpu, 1); scal_ongpu(l.nweights,
    // momentum, l.weight_updates_gpu, 1);

    axpy_ongpu(l.nweights, -decay * batch * loss_scale, l.weights_gpu, 1,
               l.weight_updates_gpu, 1);
    axpy_ongpu(l.nweights, learning_rate / batch, l.weight_updates_gpu, 1,
               l.weights_gpu, 1);

    scal_ongpu(l.nweights, momentum, l.weight_updates_gpu, 1);
  }

  if (l.clip) {
    constrain_ongpu(l.nweights, l.clip, l.weights_gpu, 1);
  }
}

void pull_implicit_layer(layer l)
{
  cuda_pull_array_async(l.weights_gpu, l.weights, l.nweights);
  cuda_pull_array_async(l.weight_updates_gpu, l.weight_updates, l.nweights);

  if (l.adam) {
    cuda_pull_array_async(l.m_gpu, l.m, l.nweights);
    cuda_pull_array_async(l.v_gpu, l.v, l.nweights);
  }
  CHECK_CUDA(cudaPeekAtLastError());
  cudaStreamSynchronize(get_cuda_stream());
}

void push_implicit_layer(layer l)
{
  cuda_push_array(l.weights_gpu, l.weights, l.nweights);

  if (l.train) {
    cuda_push_array(l.weight_updates_gpu, l.weight_updates, l.nweights);
  }
  if (l.adam) {
    cuda_push_array(l.m_gpu, l.m, l.nweights);
    cuda_push_array(l.v_gpu, l.v, l.nweights);
  }
  CHECK_CUDA(cudaPeekAtLastError());
}
#endif
