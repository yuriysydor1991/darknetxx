#include <gtest/gtest.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>

#include "CTEST_darknet_parser_provider.h"
#include "NetworkTracer.h"
#include "src/network.h"
#include "src/parser.h"

using namespace testing;

class CTEST_darknet_parser : public Test,
                             virtual public CTEST_darknet_parser_provider,
                             virtual public NetworkTracer
{
 public:
  static constexpr const size_t TEST_REPS = 1000000;

  ~CTEST_darknet_parser() = default;
  CTEST_darknet_parser() = default;

  std::string cfg1{CFG_1};
  std::string w1{W_1};

  std::string cfg2{CFG_2};
  std::string w2{W_2};
};

TEST_F(CTEST_darknet_parser, load_network_no_valid_filepaths_failure)
{
  static_assert(NULL == nullptr);

  auto* net = load_network(NULL, NULL, 0);

  EXPECT_EQ(net, nullptr);
}

TEST_F(CTEST_darknet_parser, load_network_no_valid_weights_failure)
{
  static_assert(NULL == nullptr);

  auto* net = load_network(cfg1.data(), NULL, 0);

  EXPECT_EQ(net, nullptr);
}

TEST_F(CTEST_darknet_parser, load_network_no_valid_config_failure)
{
  static_assert(NULL == nullptr);

  auto* net = load_network(NULL, w1.data(), 0);

  EXPECT_EQ(net, nullptr);
}

TEST_F(CTEST_darknet_parser, load_network_success)
{
  static_assert(NULL == nullptr);

  auto* net = load_network(cfg1.data(), w1.data(), 0);

  EXPECT_NE(net, nullptr);

  // trace_network(cfg1, w1, "layers_trace.cpp");

  free_network(*net);
  free(net);
}

TEST_F(CTEST_darknet_parser, load_network_2_success)
{
  static_assert(NULL == nullptr);

  auto* net = load_network(cfg2.data(), w2.data(), 0);

  EXPECT_NE(net, nullptr);

  // trace_network(cfg2, w2, "layers_trace.cpp");

  free_network(*net);
  free(net);
}

TEST_F(CTEST_darknet_parser, load_network_data_expected_success)
{
  static_assert(NULL == nullptr);

  auto* net = load_network(cfg1.data(), w1.data(), 0);

  EXPECT_NE(net, nullptr);

  EXPECT_TRUE(equal(net, get_1()));

  free_network(*net);
  free(net);
}

TEST_F(CTEST_darknet_parser, load_network_2_data_expected_success)
{
  static_assert(NULL == nullptr);

  auto* net = load_network(cfg2.data(), w2.data(), 0);

  EXPECT_NE(net, nullptr);

  EXPECT_TRUE(equal(net, get_2()));

  free_network(*net);
  free(net);
}
