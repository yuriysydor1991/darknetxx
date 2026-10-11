#include "layers/route_layer.h"

#include <stdio.h>

#include "blas.h"
#include "dark_cuda.h"
#include "src/clog/clog.h"
#include "utils.h"

void forward_route_layer(const route_layer l, network_state state)
{
  int i, j;
  int offset = 0;
  for (i = 0; i < l.n; ++i) {
    int index = l.input_layers[i];
    float *input = state.net.layers[index].output;
    int input_size = l.input_sizes[i];
    int part_input_size = input_size / l.groups;
    for (j = 0; j < l.batch; ++j) {
      // copy_cpu(input_size, input + j*input_size, 1, l.output + offset +
      // j*l.outputs, 1);
      copy_cpu(part_input_size,
               input + j * input_size + part_input_size * l.group_id, 1,
               l.output + offset + j * l.outputs, 1);
    }
    // offset += input_size;
    offset += part_input_size;
  }
}

void backward_route_layer(const route_layer l, network_state state)
{
  int i, j;
  int offset = 0;
  for (i = 0; i < l.n; ++i) {
    int index = l.input_layers[i];
    float *delta = state.net.layers[index].delta;
    int input_size = l.input_sizes[i];
    int part_input_size = input_size / l.groups;
    for (j = 0; j < l.batch; ++j) {
      // axpy_cpu(input_size, 1, l.delta + offset + j*l.outputs, 1, delta +
      // j*input_size, 1);
      axpy_cpu(part_input_size, 1, l.delta + offset + j * l.outputs, 1,
               delta + j * input_size + part_input_size * l.group_id, 1);
    }
    // offset += input_size;
    offset += part_input_size;
  }
}

#ifdef GPU
void forward_route_layer_gpu(const route_layer l, network_state state)
{
  if (l.stream >= 0) {
    switch_stream(l.stream);
  }

  if (l.wait_stream_id >= 0) {
    wait_stream(l.wait_stream_id);
  }

  int i, j;
  int offset = 0;
  for (i = 0; i < l.n; ++i) {
    int index = l.input_layers[i];
    float *input = state.net.layers[index].output_gpu;
    int input_size = l.input_sizes[i];
    int part_input_size = input_size / l.groups;
    for (j = 0; j < l.batch; ++j) {
      // copy_ongpu(input_size, input + j*input_size, 1, l.output_gpu + offset +
      // j*l.outputs, 1); simple_copy_ongpu(input_size, input + j*input_size,
      // l.output_gpu + offset + j*l.outputs);
      simple_copy_ongpu(part_input_size,
                        input + j * input_size + part_input_size * l.group_id,
                        l.output_gpu + offset + j * l.outputs);
    }
    // offset += input_size;
    offset += part_input_size;
  }
}

void backward_route_layer_gpu(const route_layer l, network_state state)
{
  int i, j;
  int offset = 0;
  for (i = 0; i < l.n; ++i) {
    int index = l.input_layers[i];
    float *delta = state.net.layers[index].delta_gpu;
    int input_size = l.input_sizes[i];
    int part_input_size = input_size / l.groups;
    for (j = 0; j < l.batch; ++j) {
      // axpy_ongpu(input_size, 1, l.delta_gpu + offset + j*l.outputs, 1, delta
      // + j*input_size, 1);
      axpy_ongpu(part_input_size, 1, l.delta_gpu + offset + j * l.outputs, 1,
                 delta + j * input_size + part_input_size * l.group_id, 1);
    }
    // offset += input_size;
    offset += part_input_size;
  }
}
#endif
