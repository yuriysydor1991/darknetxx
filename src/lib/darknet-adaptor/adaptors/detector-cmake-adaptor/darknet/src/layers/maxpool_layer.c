#include "layers/maxpool_layer.h"

#include <stdio.h>

#include "dark_cuda.h"
#include "gemm.h"
#include "layers/convolutional_layer.h"
#include "src/clog/clog.h"
#include "utils.h"

image get_maxpool_image(maxpool_layer l)
{
  int h = l.out_h;
  int w = l.out_w;
  int c = l.c;
  return float_to_image(w, h, c, l.output);
}

image get_maxpool_delta(maxpool_layer l)
{
  int h = l.out_h;
  int w = l.out_w;
  int c = l.c;
  return float_to_image(w, h, c, l.delta);
}

void create_maxpool_cudnn_tensors(layer *l)
{
#ifdef CUDNN
  CHECK_CUDNN(cudnnCreatePoolingDescriptor(&l->poolingDesc));
  CHECK_CUDNN(cudnnCreateTensorDescriptor(&l->srcTensorDesc));
  CHECK_CUDNN(cudnnCreateTensorDescriptor(&l->dstTensorDesc));
#endif  // CUDNN
}

void cudnn_maxpool_setup(layer *l)
{
#ifdef CUDNN
  CHECK_CUDNN(cudnnSetPooling2dDescriptor(
      l->poolingDesc, CUDNN_POOLING_MAX,
      CUDNN_NOT_PROPAGATE_NAN,  // CUDNN_PROPAGATE_NAN, CUDNN_NOT_PROPAGATE_NAN
      l->size, l->size,
      l->pad / 2,  // 0, //l.pad,
      l->pad / 2,  // 0, //l.pad,
      l->stride_x, l->stride_y));

  CHECK_CUDNN(cudnnSetTensor4dDescriptor(l->srcTensorDesc, CUDNN_TENSOR_NCHW,
                                         CUDNN_DATA_FLOAT, l->batch, l->c, l->h,
                                         l->w));
  CHECK_CUDNN(cudnnSetTensor4dDescriptor(l->dstTensorDesc, CUDNN_TENSOR_NCHW,
                                         CUDNN_DATA_FLOAT, l->batch, l->out_c,
                                         l->out_h, l->out_w));
#endif  // CUDNN
}

void cudnn_local_avgpool_setup(layer *l)
{
#ifdef CUDNN
  CHECK_CUDNN(cudnnSetPooling2dDescriptor(
      l->poolingDesc, CUDNN_POOLING_AVERAGE_COUNT_EXCLUDE_PADDING,
      CUDNN_NOT_PROPAGATE_NAN,  // CUDNN_PROPAGATE_NAN, CUDNN_NOT_PROPAGATE_NAN
      l->size, l->size,
      l->pad / 2,  // 0, //l.pad,
      l->pad / 2,  // 0, //l.pad,
      l->stride_x, l->stride_y));

  CHECK_CUDNN(cudnnSetTensor4dDescriptor(l->srcTensorDesc, CUDNN_TENSOR_NCHW,
                                         CUDNN_DATA_FLOAT, l->batch, l->c, l->h,
                                         l->w));
  CHECK_CUDNN(cudnnSetTensor4dDescriptor(l->dstTensorDesc, CUDNN_TENSOR_NCHW,
                                         CUDNN_DATA_FLOAT, l->batch, l->out_c,
                                         l->out_h, l->out_w));
#endif  // CUDNN
}

void forward_maxpool_layer(const maxpool_layer l, network_state state)
{
  if (l.maxpool_depth) {
    int b, i;
    for (b = 0; b < l.batch; ++b) {
#pragma omp parallel for
      for (i = 0; i < l.h; ++i) {
        // declared inside the parallel loop, so every thread has it's own
        // counters (the upstream code shares them between the threads)
        int j, k, g;
        for (j = 0; j < l.w; ++j) {
          for (g = 0; g < l.out_c; ++g) {
            int out_index = j + l.w * (i + l.h * (g + l.out_c * b));
            float max = -FLT_MAX;
            int max_i = -1;

            for (k = g; k < l.c; k += l.out_c) {
              int in_index = j + l.w * (i + l.h * (k + l.c * b));
              float val = state.input[in_index];

              max_i = (val > max) ? in_index : max_i;
              max = (val > max) ? val : max;
            }
            l.output[out_index] = max;
            if (l.indexes) l.indexes[out_index] = max_i;
          }
        }
      }
    }
    return;
  }

  if (!state.train && l.stride_x == l.stride_y) {
    forward_maxpool_layer_avx(state.input, l.output, l.indexes, l.size, l.w,
                              l.h, l.out_w, l.out_h, l.c, l.pad, l.stride,
                              l.batch);
  } else {
    int b, i, j, k, m, n;
    int w_offset = -l.pad / 2;
    int h_offset = -l.pad / 2;

    int h = l.out_h;
    int w = l.out_w;
    int c = l.c;

    for (b = 0; b < l.batch; ++b) {
      for (k = 0; k < c; ++k) {
        for (i = 0; i < h; ++i) {
          for (j = 0; j < w; ++j) {
            int out_index = j + w * (i + h * (k + c * b));
            float max = -FLT_MAX;
            int max_i = -1;
            for (n = 0; n < l.size; ++n) {
              for (m = 0; m < l.size; ++m) {
                int cur_h = h_offset + i * l.stride_y + n;
                int cur_w = w_offset + j * l.stride_x + m;
                int index = cur_w + l.w * (cur_h + l.h * (k + b * l.c));
                int valid =
                    (cur_h >= 0 && cur_h < l.h && cur_w >= 0 && cur_w < l.w);
                float val = (valid != 0) ? state.input[index] : -FLT_MAX;
                max_i = (val > max) ? index : max_i;
                max = (val > max) ? val : max;
              }
            }
            l.output[out_index] = max;
            if (l.indexes) l.indexes[out_index] = max_i;
          }
        }
      }
    }
  }

  if (l.antialiasing) {
    network_state s = {0};
    s.train = state.train;
    s.workspace = state.workspace;
    s.net = state.net;
    s.input = l.output;
    forward_convolutional_layer(*(l.input_layer), s);
    // simple_copy_ongpu(l.outputs*l.batch, l.output, l.input_antialiasing);
    memcpy(l.output, l.input_layer->output,
           l.input_layer->outputs * l.input_layer->batch * sizeof(float));
  }
}

void backward_maxpool_layer(const maxpool_layer l, network_state state)
{
  int i;
  int h = l.out_h;
  int w = l.out_w;
  int c = l.out_c;
  // Stays serial: several outputs may share the same max input index, so the
  // iterations would race on the state.delta elements.
  // #pragma omp parallel for
  for (i = 0; i < h * w * c * l.batch; ++i) {
    int index = l.indexes[i];
    state.delta[index] += l.delta[i];
  }
}

void forward_local_avgpool_layer(const maxpool_layer l, network_state state)
{
  int b, i, j, k, m, n;
  int w_offset = -l.pad / 2;
  int h_offset = -l.pad / 2;

  int h = l.out_h;
  int w = l.out_w;
  int c = l.c;

  for (b = 0; b < l.batch; ++b) {
    for (k = 0; k < c; ++k) {
      for (i = 0; i < h; ++i) {
        for (j = 0; j < w; ++j) {
          int out_index = j + w * (i + h * (k + c * b));
          float avg = 0;
          int counter = 0;
          for (n = 0; n < l.size; ++n) {
            for (m = 0; m < l.size; ++m) {
              int cur_h = h_offset + i * l.stride_y + n;
              int cur_w = w_offset + j * l.stride_x + m;
              int index = cur_w + l.w * (cur_h + l.h * (k + b * l.c));
              int valid =
                  (cur_h >= 0 && cur_h < l.h && cur_w >= 0 && cur_w < l.w);
              if (valid) {
                counter++;
                avg += state.input[index];
              }
            }
          }
          l.output[out_index] = avg / counter;
        }
      }
    }
  }
}

void backward_local_avgpool_layer(const maxpool_layer l, network_state state)
{
  int b, i, j, k, m, n;
  int w_offset = -l.pad / 2;
  int h_offset = -l.pad / 2;

  int h = l.out_h;
  int w = l.out_w;
  int c = l.c;

  for (b = 0; b < l.batch; ++b) {
    for (k = 0; k < c; ++k) {
      for (i = 0; i < h; ++i) {
        for (j = 0; j < w; ++j) {
          int out_index = j + w * (i + h * (k + c * b));
          for (n = 0; n < l.size; ++n) {
            for (m = 0; m < l.size; ++m) {
              int cur_h = h_offset + i * l.stride_y + n;
              int cur_w = w_offset + j * l.stride_x + m;
              int index = cur_w + l.w * (cur_h + l.h * (k + b * l.c));
              int valid =
                  (cur_h >= 0 && cur_h < l.h && cur_w >= 0 && cur_w < l.w);

              if (valid)
                state.delta[index] += l.delta[out_index] / (l.size * l.size);
            }
          }
        }
      }
    }
  }
}
