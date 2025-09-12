#include "NetworkTracer.h"

#include <gtest/gtest.h>

#include <iomanip>
#include <limits>

void NetworkTracer::print_n_floats(const std::string& name, float* arr, int n,
                                   std::fstream& file)
{
  if (arr == NULL) {
    file << "static float* " << name << " = nullptr;" << std::endl;
    file << "static size_t " << name << "_size = " << 0 << "U;" << std::endl;
    return;
  }

  file << "static float " << name << "[] = {";
  int newline = 0;
  for (int i = 0; i < n; ++i) {
    if (arr[i] == 0.0f) {
      file << "0.0f"
           << ", ";
    } else {
      file << std::setprecision(std::numeric_limits<float>::max_digits10)
           << arr[i] << "f, ";
    }

    if (newline++ >= 10) {
      file << std::endl;
      newline = 0;
    }
  }
  file << "} ;" << std::endl;

  file << "static size_t " << name << "_size = " << n << ";" << std::endl;
}

size_t NetworkTracer::get_biases_size(const layer& l)
{
  return l.type == 0 ? l.n : l.nbiases;
}

bool NetworkTracer::trace_network(std::string cfg1, std::string w1,
                                  const std::string& fpath)
{
  static_assert(NULL == nullptr);

  auto* net = load_network(cfg1.data(), w1.data(), 0);

  EXPECT_NE(net, nullptr);
  EXPECT_GE(net->n, 0);

  std::fstream file(fpath.c_str(), std::fstream::trunc | std::fstream::out);

  if (!file.is_open()) {
    return false;
  }

  for (int k = 0; k < net->n; ++k) {
    // std::cout << std::endl << "net.layers[" << k << "]" << std::endl;
    layer l = net->layers[k];

    EXPECT_GE(l.nweights, 0);

    std::string lid = "layer_" + std::to_string(k) + "_";

    print_n_floats(lid + "biases", l.biases, get_biases_size(l), file);
    print_n_floats(lid + "scales", l.scales, l.n, file);
    print_n_floats(lid + "rolling_mean", l.rolling_mean, l.n, file);
    print_n_floats(lid + "rolling_variance", l.rolling_variance, l.n, file);
    print_n_floats(lid + "weights", l.weights, l.n, file);
    // print_n_floats("delta", l.delta, (l.outputs * l.batch), file); // shuld
    // be zero print_n_floats("output", l.output, (l.out_h * l.out_w * l.out_c *
    // l.batch), file); // shuld be zero
  }

  file << "static const std::vector<dlinfo> d_1 {" << std::endl;

  for (int k = 0; k < net->n; ++k) {
    // std::cout << std::endl << "net.layers[" << k << "]" << std::endl;
    file << std::endl << "dlinfo {" << std::endl;
    layer l = net->layers[k];

    std::string lid = "layer_" + std::to_string(k) + "_";

    file << static_cast<int>(l.type) << "," << std::endl
         << lid + "biases," << std::endl
         << lid + "biases_size," << std::endl
         << lid + "scales," << std::endl
         << lid + "scales_size," << std::endl
         << lid + "rolling_mean," << std::endl
         << lid + "rolling_mean_size," << std::endl
         << lid + "rolling_variance," << std::endl
         << lid + "rolling_variance_size," << std::endl
         << lid + "weights," << std::endl
         << lid + "weights_size," << std::endl;

    file << "}," << std::endl;
  }

  file << "}; // d_1 " << std::endl;

  file.close();

  free_network(*net);
  free(net);

  return true;
}

bool NetworkTracer::equal(network* net, const dlinfov& layers)
{
  EXPECT_NE(net, nullptr);
  EXPECT_FALSE(layers.empty());

  if (net->n != layers.size()) {
    EXPECT_EQ(net->n, layers.size()) << "Expected same sizes";
    return false;
  }

  for (int k = 0; k < net->n; ++k) {
    layer l = net->layers[k];

    const bool ebiases =
        equal(l.biases, l.biases == nullptr ? 0U : get_biases_size(l),
              layers[k].biases, layers[k].nbiases);

    if (!ebiases) {
      EXPECT_TRUE(ebiases) << "Expected biases arrays to be equal for layer "
                           << k;
      return false;
    }

    const bool escales = equal(l.scales, l.scales == nullptr ? 0U : l.n,
                               layers[k].scales, layers[k].nscales);

    if (!escales) {
      EXPECT_TRUE(escales) << "Expected scales arrays to be equal for layer "
                           << k;
      return false;
    }

    const bool erolling_mean =
        equal(l.rolling_mean, l.rolling_mean == nullptr ? 0U : l.n,
              layers[k].rolling_mean, layers[k].nrolling_mean);

    if (!erolling_mean) {
      EXPECT_TRUE(erolling_mean)
          << "Expected erolling_mean arrays to be equal for layer " << k;
      return false;
    }

    const bool erolling_variance =
        equal(l.rolling_variance, l.rolling_variance == nullptr ? 0U : l.n,
              layers[k].rolling_variance, layers[k].nrolling_variance);

    if (!erolling_variance) {
      EXPECT_TRUE(erolling_variance)
          << "Expected erolling_variance arrays to be equal for layer " << k;
      return false;
    }

    const bool eweights = equal(l.weights, l.weights == nullptr ? 0U : l.n,
                                layers[k].weights, layers[k].nweights);

    if (!eweights) {
      EXPECT_TRUE(eweights)
          << "Expected weights arrays to be equal for layer " << k;
      return false;
    }
  }

  return true;
}

bool NetworkTracer::equal(float* a, size_t as, float* b, size_t bs)
{
  if (a == nullptr && b == nullptr && as == 0U && bs == 0U) {
    return true;
  }

  if (as != bs) {
    EXPECT_EQ(as, bs);
    return false;
  }

  for (size_t iter = 0U; iter < as && iter < bs; ++iter) {
    EXPECT_EQ(a[iter], b[iter]);
    if (a[iter] != b[iter]) {
      return false;
    }
  }

  return true;
}
