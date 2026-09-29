#ifndef COST_LAYER_H
#define COST_LAYER_H
#include "layer.h"
#include "network.h"

typedef layer cost_layer;

#ifdef __cplusplus
extern "C" {
#endif
COST_TYPE get_cost_type(char *s);
char *get_cost_string(COST_TYPE a);
void forward_cost_layer(const cost_layer l, network_state state);
void backward_cost_layer(const cost_layer l, network_state state);

#ifdef GPU
void forward_cost_layer_gpu(cost_layer l, network_state state);
void backward_cost_layer_gpu(const cost_layer l, network_state state);
#endif

#ifdef __cplusplus
}
#endif
#endif
