#ifndef DARKNETXX_PROJECT_CTEST_LAYER_DATA_INFO_STRUCT_H
#define DARKNETXX_PROJECT_CTEST_LAYER_DATA_INFO_STRUCT_H

#include <cstddef>
#include <vector>

struct dlinfo
{
  int type{0};

  float* biases{nullptr};
  size_t nbiases{0U};

  float* scales{nullptr};
  size_t nscales{0U};

  float* rolling_mean{nullptr};
  size_t nrolling_mean{0U};

  float* rolling_variance{nullptr};
  size_t nrolling_variance{0U};

  float* weights{nullptr};
  size_t nweights{0U};

  float* delta{nullptr};
  size_t ndelta{0U};

  float* output{nullptr};
  size_t noutput{0U};
};

using dlinfov = std::vector<dlinfo>;

#endif  // DARKNETXX_PROJECT_CTEST_LAYER_DATA_INFO_STRUCT_H
