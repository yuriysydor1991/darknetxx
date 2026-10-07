#include "layers/sam_layer.h"

#include <assert.h>
#include <stdio.h>

#include "activations.h"
#include "blas.h"
#include "dark_cuda.h"
#include "src/clog/clog.h"
#include "utils.h"

void forward_sam_layer(const layer l, network_state state)
{
  int size = l.batch * l.out_c * l.out_w * l.out_h;
  // int channel_size = 1;
  float *from_output = state.net.layers[l.index].output;

  int i;
#pragma omp parallel for
  for (i = 0; i < size; ++i) {
    l.output[i] = state.input[i] * from_output[i];
  }

  activate_array(l.output, l.outputs * l.batch, l.activation);
}

void backward_sam_layer(const layer l, network_state state)
{
  gradient_array(l.output, l.outputs * l.batch, l.activation, l.delta);
  // axpy_cpu(l.outputs*l.batch, 1, l.delta, 1, state.delta, 1);
  // scale_cpu(l.batch, l.out_w, l.out_h, l.out_c, l.delta, l.w, l.h, l.c,
  // state.net.layers[l.index].delta);

  int size = l.batch * l.out_c * l.out_w * l.out_h;
  // int channel_size = 1;
  float *from_output = state.net.layers[l.index].output;
  float *from_delta = state.net.layers[l.index].delta;

  int i;
#pragma omp parallel for
  for (i = 0; i < size; ++i) {
    state.delta[i] +=
        l.delta[i] *
        from_output[i];  // l.delta * from  (should be divided by channel_size?)

    from_delta[i] = state.input[i] * l.delta[i];  // input * l.delta
  }
}

#ifdef GPU
void forward_sam_layer_gpu(const layer l, network_state state)
{
  int size = l.batch * l.out_c * l.out_w * l.out_h;
  int channel_size = 1;

  sam_gpu(state.net.layers[l.index].output_gpu, size, channel_size, state.input,
          l.output_gpu);

  activate_array_ongpu(l.output_gpu, l.outputs * l.batch, l.activation);
}

void backward_sam_layer_gpu(const layer l, network_state state)
{
  gradient_array_ongpu(l.output_gpu, l.outputs * l.batch, l.activation,
                       l.delta_gpu);

  int size = l.batch * l.out_c * l.out_w * l.out_h;
  int channel_size = 1;
  float *from_output = state.net.layers[l.index].output_gpu;
  float *from_delta = state.net.layers[l.index].delta_gpu;

  backward_sam_gpu(l.delta_gpu, size, channel_size, state.input, from_delta,
                   from_output, state.delta);
}
#endif
