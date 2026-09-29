#ifndef CROP_LAYER_H
#define CROP_LAYER_H

#include "image.h"
#include "layer.h"
#include "network.h"

typedef layer crop_layer;

#ifdef __cplusplus
extern "C" {
#endif
image get_crop_image(crop_layer l);
void forward_crop_layer(const crop_layer l, network_state state);
void backward_crop_layer(const crop_layer l, network_state state);

#ifdef GPU
void forward_crop_layer_gpu(crop_layer l, network_state state);
#endif

#ifdef __cplusplus
}
#endif

#endif
