#ifndef DARKNET_LAYERS_TOOLS_RELATED_C_FUNCTIONS_H
#define DARKNET_LAYERS_TOOLS_RELATED_C_FUNCTIONS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

float *xrealloc_array_float_avg(float *ptr, const size_t oldsize,
                                const size_t newsize);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_LAYERS_TOOLS_RELATED_C_FUNCTIONS_H
