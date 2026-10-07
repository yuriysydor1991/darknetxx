#ifndef REPRESENTATION_LAYER_H
#define REPRESENTATION_LAYER_H

#include "layer.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif
void forward_implicit_layer(const layer l, network_state state);
void backward_implicit_layer(const layer l, network_state state);
void update_implicit_layer(layer l, int batch, float learning_rate_init,
                           float momentum, float decay);

#ifdef GPU
void forward_implicit_layer_gpu(const layer l, network_state state);
void backward_implicit_layer_gpu(const layer l, network_state state);

void update_implicit_layer_gpu(layer l, int batch, float learning_rate_init,
                               float momentum, float decay, float loss_scale);
void pull_implicit_layer(layer l);
void push_implicit_layer(layer l);
#endif

#ifdef __cplusplus
}
#endif
#endif  // REPRESENTATION_LAYER_H
