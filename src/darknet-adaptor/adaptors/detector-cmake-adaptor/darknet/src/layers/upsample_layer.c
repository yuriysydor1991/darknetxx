#include "layers/upsample_layer.h"

#include <stdio.h>

#include "blas.h"
#include "dark_cuda.h"
#include "src/clog/clog.h"
#include "utils.h"

void forward_upsample_layer(const layer l, network_state net)
{
  fill_cpu(l.outputs * l.batch, 0, l.output, 1);
  if (l.reverse) {
    upsample_cpu(l.output, l.out_w, l.out_h, l.c, l.batch, l.stride, 0, l.scale,
                 net.input);
  } else {
    upsample_cpu(net.input, l.w, l.h, l.c, l.batch, l.stride, 1, l.scale,
                 l.output);
  }
}

void backward_upsample_layer(const layer l, network_state state)
{
  if (l.reverse) {
    upsample_cpu(l.delta, l.out_w, l.out_h, l.c, l.batch, l.stride, 1, l.scale,
                 state.delta);
  } else {
    upsample_cpu(state.delta, l.w, l.h, l.c, l.batch, l.stride, 0, l.scale,
                 l.delta);
  }
}

#ifdef GPU
void forward_upsample_layer_gpu(const layer l, network_state state)
{
  fill_ongpu(l.outputs * l.batch, 0, l.output_gpu, 1);
  if (l.reverse) {
    upsample_gpu(l.output_gpu, l.out_w, l.out_h, l.c, l.batch, l.stride, 0,
                 l.scale, state.input);
  } else {
    upsample_gpu(state.input, l.w, l.h, l.c, l.batch, l.stride, 1, l.scale,
                 l.output_gpu);
  }
}

void backward_upsample_layer_gpu(const layer l, network_state state)
{
  if (l.reverse) {
    upsample_gpu(l.delta_gpu, l.out_w, l.out_h, l.c, l.batch, l.stride, 1,
                 l.scale, state.delta);
  } else {
    upsample_gpu(state.delta, l.w, l.h, l.c, l.batch, l.stride, 0, l.scale,
                 l.delta_gpu);
  }
}
#endif
