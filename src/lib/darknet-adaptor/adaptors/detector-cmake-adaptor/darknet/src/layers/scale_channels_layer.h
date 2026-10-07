#ifndef SCALE_CHANNELS_LAYER_H
#define SCALE_CHANNELS_LAYER_H

#include "layer.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif
void forward_scale_channels_layer(const layer l, network_state state);
void backward_scale_channels_layer(const layer l, network_state state);

#ifdef GPU
void forward_scale_channels_layer_gpu(const layer l, network_state state);
void backward_scale_channels_layer_gpu(const layer l, network_state state);
#endif

#ifdef __cplusplus
}
#endif
#endif  // SCALE_CHANNELS_LAYER_H
