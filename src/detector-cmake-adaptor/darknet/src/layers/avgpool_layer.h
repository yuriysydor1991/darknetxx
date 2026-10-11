#ifndef AVGPOOL_LAYER_H
#define AVGPOOL_LAYER_H

#include "dark_cuda.h"
#include "image.h"
#include "layer.h"
#include "network.h"

typedef layer avgpool_layer;

#ifdef __cplusplus
extern "C" {
#endif
image get_avgpool_image(avgpool_layer l);
void forward_avgpool_layer(const avgpool_layer l, network_state state);
void backward_avgpool_layer(const avgpool_layer l, network_state state);

#ifdef GPU
void forward_avgpool_layer_gpu(avgpool_layer l, network_state state);
void backward_avgpool_layer_gpu(avgpool_layer l, network_state state);
#endif

#ifdef __cplusplus
}
#endif

#endif
