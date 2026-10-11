#ifndef SAM_CHANNELS_LAYER_H
#define SAM_CHANNELS_LAYER_H

#include "layer.h"
#include "network.h"

#ifdef __cplusplus
extern "C" {
#endif
void forward_sam_layer(const layer l, network_state state);
void backward_sam_layer(const layer l, network_state state);

#ifdef GPU
void forward_sam_layer_gpu(const layer l, network_state state);
void backward_sam_layer_gpu(const layer l, network_state state);
#endif

#ifdef __cplusplus
}
#endif
#endif  // SAM_CHANNELS_LAYER_H
