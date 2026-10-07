#include "layers/crop_layer.h"

#include <stdio.h>

#include "dark_cuda.h"
#include "src/clog/clog.h"
#include "utils.h"

image get_crop_image(crop_layer l)
{
  int h = l.out_h;
  int w = l.out_w;
  int c = l.out_c;
  return float_to_image(w, h, c, l.output);
}

void backward_crop_layer(const crop_layer l, network_state state) {}
void backward_crop_layer_gpu(const crop_layer l, network_state state) {}

void forward_crop_layer(const crop_layer l, network_state state)
{
  int i, j, c, b, row, col;
  int index;
  int count = 0;
  int flip = (l.flip && rand() % 2);
  int dh = rand() % (l.h - l.out_h + 1);
  int dw = rand() % (l.w - l.out_w + 1);
  float scale = 2;
  float trans = -1;
  if (l.noadjust) {
    scale = 1;
    trans = 0;
  }
  if (!state.train) {
    flip = 0;
    dh = (l.h - l.out_h) / 2;
    dw = (l.w - l.out_w) / 2;
  }
  for (b = 0; b < l.batch; ++b) {
    for (c = 0; c < l.c; ++c) {
      for (i = 0; i < l.out_h; ++i) {
        for (j = 0; j < l.out_w; ++j) {
          if (flip) {
            col = l.w - dw - j - 1;
          } else {
            col = j + dw;
          }
          row = i + dh;
          index = col + l.w * (row + l.h * (c + l.c * b));
          l.output[count++] = state.input[index] * scale + trans;
        }
      }
    }
  }
}
