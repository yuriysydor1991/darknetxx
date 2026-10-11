#ifndef DARKNET_RANDOMS_RELATED_C_FUNCTIONS_H
#define DARKNET_RANDOMS_RELATED_C_FUNCTIONS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int rand_int(int min, int max);
float rand_normal();
size_t rand_size_t();
float rand_uniform(float min, float max);
float rand_scale(float s);
float **one_hot_encode(float *a, int n, int k);
unsigned int random_gen_fast(void);
float random_float_fast();
int rand_int_fast(int min, int max);
unsigned int random_gen();
float random_float();
float rand_uniform_strong(float min, float max);
float rand_precalc_random(float min, float max, float random_part);
double double_rand(void);
unsigned int uint_rand(unsigned int less_than);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_RANDOMS_RELATED_C_FUNCTIONS_H
