#ifndef ROUTE_LAYER_H
#define ROUTE_LAYER_H
#include "layer.h"
#include "network.h"

typedef layer route_layer;

#ifdef __cplusplus
extern "C" {
#endif
void forward_route_layer(const route_layer l, network_state state);
void backward_route_layer(const route_layer l, network_state state);

#ifdef GPU
void forward_route_layer_gpu(const route_layer l, network_state state);
void backward_route_layer_gpu(const route_layer l, network_state state);
#endif

#ifdef __cplusplus
}
#endif
#endif
