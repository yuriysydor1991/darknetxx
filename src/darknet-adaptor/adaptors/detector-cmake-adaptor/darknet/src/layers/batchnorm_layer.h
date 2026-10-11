#ifndef BATCHNORM_LAYER_H
#define BATCHNORM_LAYER_H

#include "image.h"
#include "layer.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif
void forward_batchnorm_layer(layer l, network_state state);
void backward_batchnorm_layer(layer l, network_state state);
void update_batchnorm_layer(layer l, int batch, float learning_rate,
                            float momentum, float decay);

#ifdef GPU
void forward_batchnorm_layer_gpu(layer l, network_state state);
void backward_batchnorm_layer_gpu(layer l, network_state state);
void update_batchnorm_layer_gpu(layer l, int batch, float learning_rate_init,
                                float momentum, float decay, float loss_scale);
void pull_batchnorm_layer(layer l);
void push_batchnorm_layer(layer l);
#endif

#ifdef __cplusplus
}
#endif
#endif
