#ifndef MAXPOOL_LAYER_H
#define MAXPOOL_LAYER_H

#include "dark_cuda.h"
#include "image.h"
#include "layer.h"
#include "network.h"

typedef layer maxpool_layer;

#ifdef __cplusplus
extern "C" {
#endif
image get_maxpool_image(maxpool_layer l);
void forward_maxpool_layer(const maxpool_layer l, network_state state);
void backward_maxpool_layer(const maxpool_layer l, network_state state);

void forward_local_avgpool_layer(const maxpool_layer l, network_state state);
void backward_local_avgpool_layer(const maxpool_layer l, network_state state);

#ifdef GPU
void forward_maxpool_layer_gpu(maxpool_layer l, network_state state);
void backward_maxpool_layer_gpu(maxpool_layer l, network_state state);
void cudnn_maxpool_setup(maxpool_layer *l);

void forward_local_avgpool_layer_gpu(maxpool_layer layer, network_state state);
void backward_local_avgpool_layer_gpu(maxpool_layer layer, network_state state);
#endif  // GPU

#ifdef __cplusplus
}
#endif

#endif
