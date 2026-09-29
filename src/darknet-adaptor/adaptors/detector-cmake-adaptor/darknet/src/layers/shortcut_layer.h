#ifndef SHORTCUT_LAYER_H
#define SHORTCUT_LAYER_H

#include "layer.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif
void forward_shortcut_layer(const layer l, network_state state);
void backward_shortcut_layer(const layer l, network_state state);
void update_shortcut_layer(layer l, int batch, float learning_rate_init,
                           float momentum, float decay);

#ifdef GPU
void forward_shortcut_layer_gpu(const layer l, network_state state);
void backward_shortcut_layer_gpu(const layer l, network_state state);
void update_shortcut_layer_gpu(layer l, int batch, float learning_rate_init,
                               float momentum, float decay, float loss_scale);
void pull_shortcut_layer(layer l);
void push_shortcut_layer(layer l);
#endif

#ifdef __cplusplus
}
#endif
#endif
