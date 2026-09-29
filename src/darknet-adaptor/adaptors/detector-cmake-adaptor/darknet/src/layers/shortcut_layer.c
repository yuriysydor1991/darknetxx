#include "layers/shortcut_layer.h"

#include <assert.h>
#include <stdio.h>

#include "blas.h"
#include "dark_cuda.h"
#include "gemm.h"
#include "layers/convolutional_layer.h"
#include "src/clog/clog.h"
#include "utils.h"

void forward_shortcut_layer(const layer l, network_state state)
{
  int from_w = state.net.layers[l.index].w;
  int from_h = state.net.layers[l.index].h;
  int from_c = state.net.layers[l.index].c;

  if (l.nweights == 0 && l.n == 1 && from_w == l.w && from_h == l.h &&
      from_c == l.c) {
    int size = l.batch * l.w * l.h * l.c;
    int i;
#pragma omp parallel for
    for (i = 0; i < size; ++i)
      l.output[i] = state.input[i] + state.net.layers[l.index].output[i];
  } else {
    shortcut_multilayer_cpu(l.outputs * l.batch, l.outputs, l.batch, l.n,
                            l.input_sizes, l.layers_output, l.output,
                            state.input, l.weights, l.nweights,
                            l.weights_normalization);
  }

  // copy_cpu(l.outputs*l.batch, state.input, 1, l.output, 1);
  // shortcut_cpu(l.batch, from_w, from_h, from_c,
  // state.net.layers[l.index].output, l.out_w, l.out_h, l.out_c, l.output);

  // activate_array(l.output, l.outputs*l.batch, l.activation);
  if (l.activation == SWISH)
    activate_array_swish(l.output, l.outputs * l.batch, l.activation_input,
                         l.output);
  else if (l.activation == MISH)
    activate_array_mish(l.output, l.outputs * l.batch, l.activation_input,
                        l.output);
  else
    activate_array_cpu_custom(l.output, l.outputs * l.batch, l.activation);
}

void backward_shortcut_layer(const layer l, network_state state)
{
  if (l.activation == SWISH)
    gradient_array_swish(l.output, l.outputs * l.batch, l.activation_input,
                         l.delta);
  else if (l.activation == MISH)
    gradient_array_mish(l.outputs * l.batch, l.activation_input, l.delta);
  else
    gradient_array(l.output, l.outputs * l.batch, l.activation, l.delta);

  backward_shortcut_multilayer_cpu(
      l.outputs * l.batch, l.outputs, l.batch, l.n, l.input_sizes,
      l.layers_delta, state.delta, l.delta, l.weights, l.weight_updates,
      l.nweights, state.input, l.layers_output, l.weights_normalization);

  // axpy_cpu(l.outputs*l.batch, 1, l.delta, 1, state.delta, 1);
  // shortcut_cpu(l.batch, l.out_w, l.out_h, l.out_c, l.delta, l.w, l.h, l.c,
  // state.net.layers[l.index].delta);
}

void update_shortcut_layer(layer l, int batch, float learning_rate_init,
                           float momentum, float decay)
{
  if (l.nweights > 0) {
    float learning_rate = learning_rate_init * l.learning_rate_scale;
    // float momentum = a.momentum;
    // float decay = a.decay;
    // int batch = a.batch;

    axpy_cpu(l.nweights, -decay * batch, l.weights, 1, l.weight_updates, 1);
    axpy_cpu(l.nweights, learning_rate / batch, l.weight_updates, 1, l.weights,
             1);
    scal_cpu(l.nweights, momentum, l.weight_updates, 1);
  }
}

#ifdef GPU
void forward_shortcut_layer_gpu(const layer l, network_state state)
{
  // copy_ongpu(l.outputs*l.batch, state.input, 1, l.output_gpu, 1);
  // simple_copy_ongpu(l.outputs*l.batch, state.input, l.output_gpu);
  // shortcut_gpu(l.batch, l.w, l.h, l.c, state.net.layers[l.index].output_gpu,
  // l.out_w, l.out_h, l.out_c, l.output_gpu);

  // input_shortcut_gpu(state.input, l.batch, l.w, l.h, l.c,
  // state.net.layers[l.index].output_gpu, l.out_w, l.out_h, l.out_c,
  // l.output_gpu);

  //-----------
  // if (l.outputs == l.input_sizes[0])
  // if(l.n == 1 && l.nweights == 0)
  //{
  //    input_shortcut_gpu(state.input, l.batch, state.net.layers[l.index].w,
  //    state.net.layers[l.index].h, state.net.layers[l.index].c,
  //        state.net.layers[l.index].output_gpu, l.out_w, l.out_h, l.out_c,
  //        l.output_gpu);
  //}
  // else
  {
    shortcut_multilayer_gpu(l.outputs, l.batch, l.n, l.input_sizes_gpu,
                            l.layers_output_gpu, l.output_gpu, state.input,
                            l.weights_gpu, l.nweights, l.weights_normalization);
  }

  if (l.activation == SWISH)
    activate_array_swish_ongpu(l.output_gpu, l.outputs * l.batch,
                               l.activation_input_gpu, l.output_gpu);
  else if (l.activation == MISH)
    activate_array_mish_ongpu(l.output_gpu, l.outputs * l.batch,
                              l.activation_input_gpu, l.output_gpu);
  else
    activate_array_ongpu(l.output_gpu, l.outputs * l.batch, l.activation);
}

void backward_shortcut_layer_gpu(const layer l, network_state state)
{
  if (l.activation == SWISH)
    gradient_array_swish_ongpu(l.output_gpu, l.outputs * l.batch,
                               l.activation_input_gpu, l.delta_gpu);
  else if (l.activation == MISH)
    gradient_array_mish_ongpu(l.outputs * l.batch, l.activation_input_gpu,
                              l.delta_gpu);
  else
    gradient_array_ongpu(l.output_gpu, l.outputs * l.batch, l.activation,
                         l.delta_gpu);

  backward_shortcut_multilayer_gpu(
      l.outputs, l.batch, l.n, l.input_sizes_gpu, l.layers_delta_gpu,
      state.delta, l.delta_gpu, l.weights_gpu, l.weight_updates_gpu, l.nweights,
      state.input, l.layers_output_gpu, l.weights_normalization);

  // axpy_ongpu(l.outputs*l.batch, 1, l.delta_gpu, 1, state.delta, 1);
  // shortcut_gpu(l.batch, l.out_w, l.out_h, l.out_c, l.delta_gpu, l.w, l.h,
  // l.c, state.net.layers[l.index].delta_gpu);
}

void update_shortcut_layer_gpu(layer l, int batch, float learning_rate_init,
                               float momentum, float decay, float loss_scale)
{
  if (l.nweights > 0) {
    float learning_rate =
        learning_rate_init * l.learning_rate_scale / loss_scale;
    // float momentum = a.momentum;
    // float decay = a.decay;
    // int batch = a.batch;

    reset_nan_and_inf(l.weight_updates_gpu, l.nweights);
    fix_nan_and_inf(l.weights_gpu, l.nweights);

    // constrain_weight_updates_ongpu(l.nweights, 1, l.weights_gpu,
    // l.weight_updates_gpu);
    constrain_ongpu(l.nweights, 1, l.weight_updates_gpu, 1);

    /*
    cuda_pull_array_async(l.weights_gpu, l.weights, l.nweights);
    cuda_pull_array_async(l.weight_updates_gpu, l.weight_updates, l.nweights);
    CHECK_CUDA(cudaStreamSynchronize(get_cuda_stream()));
    for (int i = 0; i < l.nweights; ++i) printf(" %f, ", l.weight_updates[i]);
    printf(" l.nweights = %d - updates \n", l.nweights);
    for (int i = 0; i < l.nweights; ++i) printf(" %f, ", l.weights[i]);
    printf(" l.nweights = %d \n\n", l.nweights);
    */

    // axpy_ongpu(l.nweights, -decay*batch, l.weights_gpu, 1,
    // l.weight_updates_gpu, 1);
    axpy_ongpu(l.nweights, learning_rate / batch, l.weight_updates_gpu, 1,
               l.weights_gpu, 1);
    scal_ongpu(l.nweights, momentum, l.weight_updates_gpu, 1);

    // fill_ongpu(l.nweights, 0, l.weight_updates_gpu, 1);

    // if (l.clip) {
    //     constrain_ongpu(l.nweights, l.clip, l.weights_gpu, 1);
    // }
  }
}

void pull_shortcut_layer(layer l)
{
  constrain_ongpu(l.nweights, 1, l.weight_updates_gpu, 1);
  cuda_pull_array_async(l.weight_updates_gpu, l.weight_updates, l.nweights);
  cuda_pull_array_async(l.weights_gpu, l.weights, l.nweights);
  CHECK_CUDA(cudaPeekAtLastError());
  CHECK_CUDA(cudaStreamSynchronize(get_cuda_stream()));
}

void push_shortcut_layer(layer l)
{
  cuda_push_array(l.weights_gpu, l.weights, l.nweights);
  CHECK_CUDA(cudaPeekAtLastError());
}
#endif
