#ifndef DARKNET_NETWORK_STATE_STRUCTURE_H
#define DARKNET_NETWORK_STATE_STRUCTURE_H

#include "src/structs/network.h"

#ifdef __cplusplus
extern "C" {
#endif

struct network_state;
typedef struct network_state network_state;

// network.h
typedef struct network_state
{
  float *truth;
  float *input;
  float *delta;
  float *workspace;
  int train;
  int index;
  network net;
} network_state;

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_NETWORK_STATE_STRUCTURE_H
