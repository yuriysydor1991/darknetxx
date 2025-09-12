#include "layers_tools.h"

#include <stdio.h>

#include "memory.h"
#include "src/clog/clog.h"

float *xrealloc_array_float_avg(float *ptr, const size_t oldsize,
                                const size_t newsize)
{
  double avg = 0.0;

#pragma omp parallel for reduction(+ : avg)
  for (size_t iter = 0; iter < oldsize; ++iter) {
    CLOGT("outputs value [%ll]: %f", iter, ptr[iter]);
    avg += (double)ptr[iter];
  }

  avg /= oldsize;

  CLOGT("avg value: %f", avg);

  ptr = (float *)xrealloc(ptr, newsize * sizeof(float));

#pragma omp parallel for
  for (size_t iter = oldsize; iter < newsize; ++iter) {
    ptr[iter] = avg;
    CLOGT("new outputs value [%ll]: %f", iter, ptr[iter]);
  }

  return ptr;
}
