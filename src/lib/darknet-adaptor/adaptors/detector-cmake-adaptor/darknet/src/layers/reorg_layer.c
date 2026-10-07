#include "layers/reorg_layer.h"

#include <stdio.h>

#include "blas.h"
#include "dark_cuda.h"
#include "src/clog/clog.h"
#include "utils.h"

void forward_reorg_layer(const layer l, network_state state)
{
  if (l.reverse) {
    reorg_cpu(state.input, l.out_w, l.out_h, l.out_c, l.batch, l.stride, 1,
              l.output);
  } else {
    reorg_cpu(state.input, l.out_w, l.out_h, l.out_c, l.batch, l.stride, 0,
              l.output);
  }
}

void backward_reorg_layer(const layer l, network_state state)
{
  if (l.reverse) {
    reorg_cpu(l.delta, l.out_w, l.out_h, l.out_c, l.batch, l.stride, 0,
              state.delta);
  } else {
    reorg_cpu(l.delta, l.out_w, l.out_h, l.out_c, l.batch, l.stride, 1,
              state.delta);
  }
}

#ifdef GPU
void forward_reorg_layer_gpu(layer l, network_state state)
{
  if (l.reverse) {
    reorg_ongpu(state.input, l.out_w, l.out_h, l.out_c, l.batch, l.stride, 1,
                l.output_gpu);
  } else {
    reorg_ongpu(state.input, l.out_w, l.out_h, l.out_c, l.batch, l.stride, 0,
                l.output_gpu);
  }
}

void backward_reorg_layer_gpu(layer l, network_state state)
{
  if (l.reverse) {
    reorg_ongpu(l.delta_gpu, l.out_w, l.out_h, l.out_c, l.batch, l.stride, 0,
                state.delta);
  } else {
    reorg_ongpu(l.delta_gpu, l.out_w, l.out_h, l.out_c, l.batch, l.stride, 1,
                state.delta);
  }
}
#endif
