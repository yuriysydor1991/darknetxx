#include "randoms.h"

#include <assert.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>

#include "memory.h"

int rand_int(int min, int max)
{
  if (max < min) {
    int s = min;
    min = max;
    max = s;
  }
  int r = (random_gen() % (max - min + 1)) + min;
  return r;
}

// From http://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
float rand_normal()
{
  static int haveSpare = 0;
  static double rand1, rand2;

  if (haveSpare) {
    haveSpare = 0;
    const float rt = sqrt(rand1) * sin(rand2);

    assert(isfinite(rt));

    return rt;
  }

  haveSpare = 1;

  rand1 = random_gen() / ((double)RAND_MAX);
  if (rand1 < 1e-100) rand1 = 1e-100;
  rand1 = -2 * log(rand1);
  rand2 = (random_gen() / ((double)RAND_MAX)) * 2.0 * M_PI;

  const double rt = sqrt(rand1) * cos(rand2);

  assert(isfinite(rt));

  return rt;
}

/*
   float rand_normal()
   {
   int n = 12;
   int i;
   float sum= 0;
   for(i = 0; i < n; ++i) sum += (float)random_gen()/RAND_MAX;
   return sum-n/2.;
   }
 */

size_t rand_size_t()
{
  return ((size_t)(random_gen() & 0xff) << 56) |
         ((size_t)(random_gen() & 0xff) << 48) |
         ((size_t)(random_gen() & 0xff) << 40) |
         ((size_t)(random_gen() & 0xff) << 32) |
         ((size_t)(random_gen() & 0xff) << 24) |
         ((size_t)(random_gen() & 0xff) << 16) |
         ((size_t)(random_gen() & 0xff) << 8) |
         ((size_t)(random_gen() & 0xff) << 0);
}

float rand_uniform(float min, float max)
{
  if (max < min) {
    float swap = min;
    min = max;
    max = swap;
  }

#if (RAND_MAX < 65536)
  int rnd = rand() * (RAND_MAX + 1) + rand();
  return ((float)rnd / (RAND_MAX * RAND_MAX) * (max - min)) + min;
#else
  return ((float)rand() / RAND_MAX * (max - min)) + min;
#endif
  // return (random_float() * (max - min)) + min;
}

float rand_scale(float s)
{
  float scale = rand_uniform_strong(1, s);
  if (random_gen() % 2) return scale;
  return 1. / scale;
}

float **one_hot_encode(float *a, int n, int k)
{
  int i;
  float **t = (float **)xcalloc(n, sizeof(float *));
  for (i = 0; i < n; ++i) {
    t[i] = (float *)xcalloc(k, sizeof(float));
    int index = (int)a[i];
    t[i][index] = 1;
  }
  return t;
}

static unsigned int x = 123456789, y = 362436069, z = 521288629;

// Marsaglia's xorshf96 generator: period 2^96-1
unsigned int random_gen_fast(void)
{
  unsigned int t;
  x ^= x << 16;
  x ^= x >> 5;
  x ^= x << 1;

  t = x;
  x = y;
  y = z;
  z = t ^ x ^ y;

  return z;
}

float random_float_fast()
{
  return ((float)random_gen_fast() / (float)UINT_MAX);
}

int rand_int_fast(int min, int max)
{
  if (max < min) {
    int s = min;
    min = max;
    max = s;
  }
  int r = (random_gen_fast() % (max - min + 1)) + min;
  return r;
}

unsigned int random_gen()
{
  unsigned int rnd = 0;
#ifdef WIN32
  rand_s(&rnd);
#else  // WIN32
  rnd = rand();
#if (RAND_MAX < 65536)
  rnd = rand() * (RAND_MAX + 1) + rnd;
#endif  //(RAND_MAX < 65536)
#endif  // WIN32
  return rnd;
}

float random_float()
{
  unsigned int rnd = 0;
#ifdef WIN32
  rand_s(&rnd);
  return ((float)rnd / (float)UINT_MAX);
#else  // WIN32

  rnd = rand();
#if (RAND_MAX < 65536)
  rnd = rand() * (RAND_MAX + 1) + rnd;
  return ((float)rnd / (float)(RAND_MAX * RAND_MAX));
#endif  //(RAND_MAX < 65536)
  return ((float)rnd / (float)RAND_MAX);

#endif  // WIN32
}

float rand_uniform_strong(float min, float max)
{
  if (max < min) {
    float swap = min;
    min = max;
    max = swap;
  }
  return (random_float() * (max - min)) + min;
}

float rand_precalc_random(float min, float max, float random_part)
{
  if (max < min) {
    float swap = min;
    min = max;
    max = swap;
  }

  return (random_part * (max - min)) + min;
}

#define RS_SCALE (1.0 / (1.0 + RAND_MAX))

double double_rand(void)
{
  double d;
  do {
    d = (((rand() * RS_SCALE) + rand()) * RS_SCALE + rand()) * RS_SCALE;
  } while (d >= 1);  // Round off
  return d;
}

unsigned int uint_rand(unsigned int less_than)
{
  return (unsigned int)((less_than)*double_rand());
}
