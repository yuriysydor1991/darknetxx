#ifndef DARKNETXX_PROJECT_NETWORK_TRACER_COMPONENT_TEST_HELPER_CLASS_H
#define DARKNETXX_PROJECT_NETWORK_TRACER_COMPONENT_TEST_HELPER_CLASS_H

#include <fstream>
#include <string>

#include "dlinfo.h"
#include "src/network.h"
#include "src/parser.h"

class NetworkTracer
{
 public:
  void print_n_floats(const std::string& name, float* arr, int n,
                      std::fstream& file);

  bool trace_network(std::string cfg1, std::string w1,
                     const std::string& fpath);

  bool equal(network* net, const dlinfov& layers);

  bool equal(float* a, size_t as, float* b, size_t bs);

  size_t get_biases_size(const layer& l);
};

#endif  // DARKNETXX_PROJECT_NETWORK_TRACER_COMPONENT_TEST_HELPER_CLASS_H
