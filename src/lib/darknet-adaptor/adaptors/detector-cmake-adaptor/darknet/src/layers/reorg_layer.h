#ifndef REORG_LAYER_H
#define REORG_LAYER_H

#include "dark_cuda.h"
#include "image.h"
#include "layer.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif
void forward_reorg_layer(const layer l, network_state state);
void backward_reorg_layer(const layer l, network_state state);

#ifdef GPU
void forward_reorg_layer_gpu(layer l, network_state state);
void backward_reorg_layer_gpu(layer l, network_state state);
#endif

#ifdef __cplusplus
}
#endif

#endif
