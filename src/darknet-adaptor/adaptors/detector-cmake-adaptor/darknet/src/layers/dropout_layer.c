#include "layers/dropout_layer.h"

#include <stdio.h>
#include <stdlib.h>

#include "dark_cuda.h"
#include "src/clog/clog.h"
#include "utils.h"

void forward_dropout_layer(dropout_layer l, network_state state)
{
  int i;
  if (!state.train) return;
  for (i = 0; i < l.batch * l.inputs; ++i) {
    float r = rand_uniform(0, 1);
    l.rand[i] = r;
    if (r < l.probability) state.input[i] = 0;
    else
      state.input[i] *= l.scale;
  }
}

void backward_dropout_layer(dropout_layer l, network_state state)
{
  int i;
  if (!state.delta) return;
  for (i = 0; i < l.batch * l.inputs; ++i) {
    float r = l.rand[i];
    if (r < l.probability) state.delta[i] = 0;
    else
      state.delta[i] *= l.scale;
  }
}
