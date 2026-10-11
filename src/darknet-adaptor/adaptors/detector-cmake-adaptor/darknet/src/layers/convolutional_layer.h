#ifndef CONVOLUTIONAL_LAYER_H
#define CONVOLUTIONAL_LAYER_H

#include "activations.h"
#include "dark_cuda.h"
#include "image.h"
#include "layer.h"
#include "network.h"

typedef layer convolutional_layer;

#ifdef __cplusplus
extern "C" {
#endif
#ifdef GPU
void forward_convolutional_layer_gpu(convolutional_layer layer,
                                     network_state state);
void backward_convolutional_layer_gpu(convolutional_layer layer,
                                      network_state state);
void update_convolutional_layer_gpu(convolutional_layer layer, int batch,
                                    float learning_rate, float momentum,
                                    float decay, float loss_scale);

void push_convolutional_layer(convolutional_layer layer);
void pull_convolutional_layer(convolutional_layer layer);

void add_bias_gpu(float *output, float *biases, int batch, int n, int size);
void backward_bias_gpu(float *bias_updates, float *delta, int batch, int n,
                       int size);
#ifdef CUDNN
void cudnn_convolutional_setup(layer *l, int cudnn_preference,
                               size_t workspace_size_specify);
void create_convolutional_cudnn_tensors(layer *l);
void cuda_convert_f32_to_f16(float *input_f32, size_t size, float *output_f16);
#endif
#endif

size_t get_convolutional_workspace_size(layer l);
void denormalize_convolutional_layer(convolutional_layer l);
void set_specified_workspace_limit(convolutional_layer *l,
                                   size_t workspace_size_limit);
void forward_convolutional_layer(const convolutional_layer layer,
                                 network_state state);
void update_convolutional_layer(convolutional_layer layer, int batch,
                                float learning_rate, float momentum,
                                float decay);
image *visualize_convolutional_layer(convolutional_layer layer, char *window,
                                     image *prev_weights);
void binarize_weights(float *weights, int n, int size, float *binary);
void swap_binary(convolutional_layer *l);
void binarize_weights2(float *weights, int n, int size, char *binary,
                       float *scales);

void get_mean_array(float *src, size_t size, size_t filters, float *mean_arr);

void backward_convolutional_layer(convolutional_layer layer,
                                  network_state state);

void add_bias(float *output, float *biases, int batch, int n, int size);
void backward_bias(float *bias_updates, float *delta, int batch, int n,
                   int size);

image get_convolutional_image(convolutional_layer layer);
image get_convolutional_delta(convolutional_layer layer);
image get_convolutional_weight(convolutional_layer layer, int i);

int convolutional_out_height(convolutional_layer layer);
int convolutional_out_width(convolutional_layer layer);
void rescale_weights(convolutional_layer l, float scale, float trans);
void rgbgr_weights(convolutional_layer l);
void assisted_excitation_forward(convolutional_layer l, network_state state);
void assisted_excitation_forward_gpu(convolutional_layer l,
                                     network_state state);

#ifdef __cplusplus
}
#endif

#endif
