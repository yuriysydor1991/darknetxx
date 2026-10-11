#include "gpu_index_decls.h"

#ifdef __cplusplus
extern "C" {
#endif

int cuda_debug_sync = 0;

#ifdef GPU
int gpu_index = 0;
#else
// No GPU to use, which the code reading the index expects to see as a negative
// value (the original Darknet main() set it to -1 in the CPU builds): for
// example the XNOR weights get bit-inverted for the GPU kernels otherwise.
int gpu_index = -1;
#endif

#ifdef __cplusplus
}
#endif  // __cplusplus
