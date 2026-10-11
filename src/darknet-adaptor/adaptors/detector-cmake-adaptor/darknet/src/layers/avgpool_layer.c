#include "layers/avgpool_layer.h"

#include <stdio.h>

#include "dark_cuda.h"
#include "src/clog/clog.h"
#include "utils.h"

void forward_avgpool_layer(const avgpool_layer l, network_state state)
{
  int b, i, k;

  for (b = 0; b < l.batch; ++b) {
    for (k = 0; k < l.c; ++k) {
      int out_index = k + b * l.c;
      l.output[out_index] = 0;
      for (i = 0; i < l.h * l.w; ++i) {
        int in_index = i + l.h * l.w * (k + b * l.c);
        l.output[out_index] += state.input[in_index];
      }
      l.output[out_index] /= l.h * l.w;
    }
  }
}

void backward_avgpool_layer(const avgpool_layer l, network_state state)
{
  int b, i, k;

  for (b = 0; b < l.batch; ++b) {
    for (k = 0; k < l.c; ++k) {
      int out_index = k + b * l.c;
      for (i = 0; i < l.h * l.w; ++i) {
        int in_index = i + l.h * l.w * (k + b * l.c);
        state.delta[in_index] += l.delta[out_index] / (l.h * l.w);
      }
    }
  }
}
