#ifndef UPSAMPLE_LAYER_H
#define UPSAMPLE_LAYER_H
#include "dark_cuda.h"
#include "layer.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif
void forward_upsample_layer(const layer l, network_state state);
void backward_upsample_layer(const layer l, network_state state);

#ifdef GPU
void forward_upsample_layer_gpu(const layer l, network_state state);
void backward_upsample_layer_gpu(const layer l, network_state state);
#endif

#ifdef __cplusplus
}
#endif
#endif
