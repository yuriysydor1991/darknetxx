#include "layers/normalization_layer.h"

#include <stdio.h>

#include "blas.h"
#include "src/clog/clog.h"
#include "utils.h"

void forward_normalization_layer(const layer layer, network_state state)
{
  int k, b;
  int w = layer.w;
  int h = layer.h;
  int c = layer.c;
  scal_cpu(w * h * c * layer.batch, 0, layer.squared, 1);

  for (b = 0; b < layer.batch; ++b) {
    float *squared = layer.squared + w * h * c * b;
    float *norms = layer.norms + w * h * c * b;
    float *input = state.input + w * h * c * b;
    pow_cpu(w * h * c, 2, input, 1, squared, 1);

    const_cpu(w * h, layer.kappa, norms, 1);
    // the channels of the input only (the original read past the squares of
    // the input of less channels than the half of the window)
    for (k = 0; k < layer.size / 2 && k < layer.c; ++k) {
      axpy_cpu(w * h, layer.alpha, squared + w * h * k, 1, norms, 1);
    }

    for (k = 1; k < layer.c; ++k) {
      copy_cpu(w * h, norms + w * h * (k - 1), 1, norms + w * h * k, 1);
      int prev = k - ((layer.size - 1) / 2) - 1;
      int next = k + (layer.size / 2);
      if (prev >= 0)
        axpy_cpu(w * h, -layer.alpha, squared + w * h * prev, 1,
                 norms + w * h * k, 1);
      if (next < layer.c)
        axpy_cpu(w * h, layer.alpha, squared + w * h * next, 1,
                 norms + w * h * k, 1);
    }
  }
  pow_cpu(w * h * c * layer.batch, -layer.beta, layer.norms, 1, layer.output,
          1);
  mul_cpu(w * h * c * layer.batch, state.input, 1, layer.output, 1);
}

void backward_normalization_layer(const layer layer, network_state state)
{
  // TODO This is approximate ;-)
  // Also this should add in to delta instead of overwritting.

  int w = layer.w;
  int h = layer.h;
  int c = layer.c;
  pow_cpu(w * h * c * layer.batch, -layer.beta, layer.norms, 1, state.delta, 1);
  mul_cpu(w * h * c * layer.batch, layer.delta, 1, state.delta, 1);
}

#ifdef GPU
void forward_normalization_layer_gpu(const layer layer, network_state state)
{
  int k, b;
  int w = layer.w;
  int h = layer.h;
  int c = layer.c;
  scal_ongpu(w * h * c * layer.batch, 0, layer.squared_gpu, 1);

  for (b = 0; b < layer.batch; ++b) {
    float *squared = layer.squared_gpu + w * h * c * b;
    float *norms = layer.norms_gpu + w * h * c * b;
    float *input = state.input + w * h * c * b;
    pow_ongpu(w * h * c, 2, input, 1, squared, 1);

    const_ongpu(w * h, layer.kappa, norms, 1);
    for (k = 0; k < layer.size / 2; ++k) {
      axpy_ongpu(w * h, layer.alpha, squared + w * h * k, 1, norms, 1);
    }

    for (k = 1; k < layer.c; ++k) {
      copy_ongpu(w * h, norms + w * h * (k - 1), 1, norms + w * h * k, 1);
      int prev = k - ((layer.size - 1) / 2) - 1;
      int next = k + (layer.size / 2);
      if (prev >= 0)
        axpy_ongpu(w * h, -layer.alpha, squared + w * h * prev, 1,
                   norms + w * h * k, 1);
      if (next < layer.c)
        axpy_ongpu(w * h, layer.alpha, squared + w * h * next, 1,
                   norms + w * h * k, 1);
    }
  }
  pow_ongpu(w * h * c * layer.batch, -layer.beta, layer.norms_gpu, 1,
            layer.output_gpu, 1);
  mul_ongpu(w * h * c * layer.batch, state.input, 1, layer.output_gpu, 1);
}

void backward_normalization_layer_gpu(const layer layer, network_state state)
{
  // TODO This is approximate ;-)

  int w = layer.w;
  int h = layer.h;
  int c = layer.c;
  pow_ongpu(w * h * c * layer.batch, -layer.beta, layer.norms_gpu, 1,
            state.delta, 1);
  mul_ongpu(w * h * c * layer.batch, layer.delta_gpu, 1, state.delta, 1);
}
#endif
