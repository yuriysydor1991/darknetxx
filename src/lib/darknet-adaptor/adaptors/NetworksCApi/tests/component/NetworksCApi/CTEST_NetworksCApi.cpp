#include <gtest/gtest.h>

#include <atomic>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksCApi/NetworksRegistry.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/parser.h"

using namespace testing;

using darknet_adaptor::adaptors::ANetworkAdaptor;
using darknet_adaptor::adaptors::ANetworkAdaptorPtr;
using darknet_adaptor::adaptors::NetworksRegistry;
using darknet_adaptor::loaders::OrigDefaultLoader;

namespace
{

const char* const CFG =
    "[net]\nbatch=4\nsubdivisions=2\nwidth=16\nheight=16\nchannels=3\n"
    "[convolutional]\nfilters=8\nsize=3\npad=1\nbatch_normalize=1\n"
    "activation=leaky\n"
    "[maxpool]\nsize=2\nstride=2\n"
    "[convolutional]\nfilters=32\nsize=3\npad=1\nactivation=leaky\n"
    "[convolutional]\nfilters=8\nsize=3\npad=1\nxnor=1\nactivation=leaky\n"
    "[convolutional]\nfilters=4\nsize=1\nactivation=linear\n";

/// @brief The count of the float values of the layer weights.
size_t weights_count(const layer& l)
{
  return l.type == CONVOLUTIONAL ? static_cast<size_t>(l.nweights) : 0U;
}

}  // namespace

class CTEST_NetworksCApi : public Test
{
 public:
  CTEST_NetworksCApi()
  {
    const TestInfo* const info = UnitTest::GetInstance()->current_test_info();

    dir = std::string{CTEST_NetworksCApi_DATA_DIR} + "/generated/" +
          info->name() + "." + std::to_string(std::random_device{}());

    std::filesystem::create_directories(dir);

    cfg = write("net.cfg", CFG);
  }

  ~CTEST_NetworksCApi()
  {
    std::error_code ignored;
    std::filesystem::remove_all(dir, ignored);
  }

  std::string write(const std::string& name, const std::string& content)
  {
    const std::string path = dir + "/" + name;
    std::ofstream out{path};
    out << content;
    return path;
  }

  /// @brief The mutable C string the darknet C API takes.
  static char* c_str(std::string& value) { return value.data(); }

  std::string dir;
  std::string cfg;
  size_t registered{NetworksRegistry::size()};
};

/*
 * The networks the C code makes.
 */

TEST_F(CTEST_NetworksCApi, parse_network_cfg_hands_the_network_over)
{
  network net = parse_network_cfg(c_str(cfg));

  ASSERT_NE(net.layers, nullptr);
  EXPECT_EQ(net.n, 5);
  EXPECT_EQ(net.batch, 2);

  // the registry keeps the network the view is of
  EXPECT_EQ(NetworksRegistry::size(), registered + 1U);
  EXPECT_TRUE(NetworksRegistry::owns(net));

  const ANetworkAdaptorPtr adaptor = NetworksRegistry::find(net);

  ASSERT_NE(adaptor, nullptr);
  EXPECT_EQ(adaptor->get()->layers, net.layers);
  EXPECT_EQ(adaptor->get()->seen, net.seen);
  // the training buffers
  EXPECT_NE(net.layers[0].delta, nullptr);

  std::vector<float> input(
      static_cast<size_t>(net.inputs) * static_cast<size_t>(net.batch), 0.5f);
  EXPECT_NE(network_predict(net, input.data()), nullptr);

  free_network(net);

  EXPECT_EQ(NetworksRegistry::size(), registered);
  EXPECT_EQ(NetworksRegistry::find(net), nullptr);
}

TEST_F(CTEST_NetworksCApi, parse_network_cfg_custom_makes_the_inference_network)
{
  network net = parse_network_cfg_custom(c_str(cfg), 1, 1);

  EXPECT_EQ(net.batch, 1);
  EXPECT_EQ(net.layers[0].delta, nullptr);
  EXPECT_TRUE(NetworksRegistry::owns(net));

  free_network(net);

  EXPECT_EQ(NetworksRegistry::size(), registered);
}

TEST_F(CTEST_NetworksCApi, save_and_load_weights_round_trip)
{
  std::string weights = dir + "/net.weights";
  std::string first = dir + "/first.weights";

  network source = parse_network_cfg(c_str(cfg));
  network copy = parse_network_cfg(c_str(cfg));
  network part = parse_network_cfg(c_str(cfg));

  save_weights(source, c_str(weights));
  load_weights(&copy, c_str(weights));

  // the first layer only
  save_weights_upto(source, c_str(first), 1, 0);
  load_weights_upto(&part, c_str(first), part.n);

  for (int index = 0; index < source.n; ++index) {
    SCOPED_TRACE("the layer " + std::to_string(index));

    const size_t count = weights_count(source.layers[index]);
    if (count == 0U) continue;

    EXPECT_EQ(std::memcmp(copy.layers[index].weights,
                          source.layers[index].weights, count * sizeof(float)),
              0);
    EXPECT_EQ(std::memcmp(
                  copy.layers[index].biases, source.layers[index].biases,
                  static_cast<size_t>(source.layers[index].n) * sizeof(float)),
              0);

    // the random initialization of the not saved layers stays
    EXPECT_EQ(
        std::memcmp(part.layers[index].weights, source.layers[index].weights,
                    count * sizeof(float)) == 0,
        index == 0);
  }

  EXPECT_EQ(*copy.seen, *source.seen);

  free_network(source);
  free_network(copy);
  free_network(part);

  EXPECT_EQ(NetworksRegistry::size(), registered);
}

TEST_F(CTEST_NetworksCApi, load_network_returns_the_holder_the_caller_frees)
{
  std::string weights = dir + "/net.weights";

  {
    network source = parse_network_cfg(c_str(cfg));
    *source.cur_iteration = 3;
    save_weights(source, c_str(weights));
    free_network(source);
  }

  network* const net = load_network(c_str(cfg), c_str(weights), 0);

  ASSERT_NE(net, nullptr);
  EXPECT_GT(*net->seen, 0U);
  EXPECT_TRUE(NetworksRegistry::owns(*net));

  network* const cleared = load_network(c_str(cfg), c_str(weights), 1);

  ASSERT_NE(cleared, nullptr);
  EXPECT_EQ(*cleared->seen, 0U);

  free_network_ptr(net);
  free(net);
  free_network_ptr(cleared);
  free(cleared);

  EXPECT_EQ(NetworksRegistry::size(), registered);
}

TEST_F(CTEST_NetworksCApi, load_network_of_no_cfg_is_null)
{
  char empty[] = "";

  EXPECT_EQ(load_network(nullptr, nullptr, 0), nullptr);
  EXPECT_EQ(load_network(empty, nullptr, 0), nullptr);
  EXPECT_EQ(NetworksRegistry::size(), registered);
}

TEST_F(CTEST_NetworksCApi, load_network_custom_fuses_the_batch_normalization)
{
  std::string weights = dir + "/net.weights";

  {
    network source = parse_network_cfg(c_str(cfg));
    save_weights(source, c_str(weights));
    free_network(source);
  }

  network* const net = load_network_custom(c_str(cfg), c_str(weights), 0, 1);

  ASSERT_NE(net, nullptr);
  EXPECT_EQ(net->batch, 1);
  EXPECT_EQ(net->layers[0].batch_normalize, 0);
  EXPECT_EQ(net->layers[0].rolling_mean, nullptr);

  free_network_ptr(net);
  free(net);

  EXPECT_EQ(NetworksRegistry::size(), registered);
}

/*
 * The C code copies of the network view.
 */

TEST_F(CTEST_NetworksCApi, the_copies_of_the_view_reach_their_network)
{
  network net = parse_network_cfg(c_str(cfg));
  const ANetworkAdaptorPtr adaptor = NetworksRegistry::find(net);

  ASSERT_NE(adaptor, nullptr);

  // the C code passes the struct by value and changes it's scalars
  network copy = net;
  copy.learning_rate = 0.5f;

  ASSERT_EQ(resize_network(&copy, 32, 48), 0);

  EXPECT_EQ(copy.w, 32);
  EXPECT_EQ(copy.h, 48);
  EXPECT_EQ(copy.layers[0].out_w, 32);
  EXPECT_EQ(copy.workspace, adaptor->get()->workspace);
  EXPECT_EQ(adaptor->get()->w, 32);
  EXPECT_FLOAT_EQ(adaptor->get()->learning_rate, 0.5f);

  set_batch_network(&copy, 1);

  EXPECT_EQ(copy.batch, 1);
  EXPECT_EQ(adaptor->get()->batch, 1);
  EXPECT_EQ(copy.layers[0].batch, 1);

  // the layers array is the same, the free of any copy releases the network
  free_network(net);

  EXPECT_EQ(NetworksRegistry::find(copy), nullptr);
  EXPECT_EQ(NetworksRegistry::size(), registered);
}

TEST_F(CTEST_NetworksCApi, fuse_and_binary_weights_of_the_view)
{
  network net = parse_network_cfg_custom(c_str(cfg), 1, 1);

  ASSERT_EQ(net.layers[0].batch_normalize, 1);

  fuse_conv_batchnorm(net);

  EXPECT_EQ(net.layers[0].batch_normalize, 0);
  EXPECT_EQ(net.layers[0].scales, nullptr);

  calculate_binary_weights(net);

  EXPECT_EQ(net.layers[2].align_bit_weights, nullptr);
  EXPECT_NE(net.layers[3].align_bit_weights, nullptr);

  std::vector<float> input(static_cast<size_t>(net.inputs), 0.25f);
  EXPECT_NE(network_predict(net, input.data()), nullptr);

  free_network(net);
}

TEST_F(CTEST_NetworksCApi, copy_weights_net_of_the_views)
{
  network train = parse_network_cfg(c_str(cfg));
  network map = parse_network_cfg_custom(c_str(cfg), 1, 1);

  copy_weights_net(train, &map);

  for (int index = 0; index < train.n; ++index) {
    EXPECT_EQ(map.layers[index].output, train.layers[index].output);
    EXPECT_EQ(map.layers[index].batch, 1);
    EXPECT_EQ(map.layers[index].train, 0);
  }

  // every network releases it's own buffers only
  free_network(map);
  free_network(train);

  EXPECT_EQ(NetworksRegistry::size(), registered);
}

TEST_F(CTEST_NetworksCApi, free_network_leaves_the_cxx_networks_to_their_owner)
{
  OrigDefaultLoader loader;
  const ANetworkAdaptorPtr owned = loader.parse_network_cfg(cfg);

  // the way the DarknetContext hands it's networks to the computation
  NetworksRegistry::add(owned, false);

  internal::CaptureStderr();
  free_network(*owned->get());
  const std::string errors = internal::GetCapturedStderr();

  EXPECT_EQ(NetworksRegistry::find(*owned->get()), owned);
  EXPECT_NE(errors.find("left to the owner"), std::string::npos) << errors;

  EXPECT_TRUE(NetworksRegistry::remove(*owned->get()));
  EXPECT_EQ(NetworksRegistry::size(), registered);
}

TEST_F(CTEST_NetworksCApi, free_network_of_the_unknown_networks_does_nothing)
{
  network empty;
  std::memset(&empty, 0, sizeof(empty));

  free_network(empty);

  layer foreign_layers[1];
  network foreign = empty;
  foreign.layers = foreign_layers;

  internal::CaptureStderr();
  free_network(foreign);
  const std::string errors = internal::GetCapturedStderr();

  EXPECT_NE(errors.find("unknown network"), std::string::npos) << errors;
  EXPECT_EQ(NetworksRegistry::size(), registered);
}

/*
 * The errors, which end the process as the original functions did.
 */

class CTEST_NetworksCApi_errors : public CTEST_NetworksCApi
{
 public:
  CTEST_NetworksCApi_errors()
  {
    // the OpenMP workers of the computation make the forking death tests
    // unsafe
    GTEST_FLAG_SET(death_test_style, "threadsafe");
  }
};

TEST_F(CTEST_NetworksCApi_errors, parse_network_cfg_of_a_missing_cfg_exits)
{
  std::string missing = dir + "/missing.cfg";

  EXPECT_EXIT(parse_network_cfg(c_str(missing)), ExitedWithCode(EXIT_FAILURE),
              "");
}

TEST_F(CTEST_NetworksCApi_errors, parse_network_cfg_of_an_invalid_cfg_exits)
{
  std::string invalid = write("invalid.cfg", "[net]\nbatch=1\n[unknown]\n");

  EXPECT_EXIT(parse_network_cfg(c_str(invalid)), ExitedWithCode(EXIT_FAILURE),
              "");
}

TEST_F(CTEST_NetworksCApi_errors, load_weights_of_a_missing_file_exits)
{
  std::string missing = dir + "/missing.weights";

  EXPECT_EXIT(
      {
        network net = parse_network_cfg(c_str(cfg));
        load_weights(&net, c_str(missing));
      },
      ExitedWithCode(EXIT_FAILURE), "Couldn't open file");
}

TEST_F(CTEST_NetworksCApi_errors, save_weights_to_an_unwritable_path_exits)
{
  std::string unwritable = dir + "/no/such/dir/net.weights";

  EXPECT_EXIT(
      {
        network net = parse_network_cfg(c_str(cfg));
        save_weights(net, c_str(unwritable));
      },
      ExitedWithCode(EXIT_FAILURE), "Couldn't open file");
}

TEST_F(CTEST_NetworksCApi_errors, resize_network_of_an_unknown_network_exits)
{
  EXPECT_EXIT(
      {
        layer foreign_layers[1];
        network foreign;
        std::memset(&foreign, 0, sizeof(foreign));
        foreign.layers = foreign_layers;
        foreign.n = 1;

        resize_network(&foreign, 32, 32);
      },
      ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(CTEST_NetworksCApi_errors, the_view_of_other_counters_exits)
{
  EXPECT_EXIT(
      {
        network net = parse_network_cfg(c_str(cfg));
        uint64_t other_seen = 0U;

        network forged = net;
        forged.seen = &other_seen;

        set_batch_network(&forged, 1);
      },
      ExitedWithCode(EXIT_FAILURE), "");
}

TEST_F(CTEST_NetworksCApi_errors, resize_network_of_the_not_resizable_one_exits)
{
  std::string flat = write("flat.cfg",
                           "[net]\nbatch=1\nwidth=8\nheight=8\nchannels=3\n"
                           "[connected]\noutput=4\n");

  EXPECT_EXIT(
      {
        network net = parse_network_cfg(c_str(flat));
        resize_network(&net, 16, 16);
      },
      ExitedWithCode(EXIT_FAILURE), "");
}

/*
 * The registry.
 */

TEST_F(CTEST_NetworksCApi, the_registry_keeps_the_owned_networks)
{
  std::weak_ptr<ANetworkAdaptor> watcher;
  network view;

  {
    const ANetworkAdaptorPtr net = ANetworkAdaptor::create();
    net->allocate_layers(1);

    NetworksRegistry::add(net, true);

    watcher = net;
    view = *net->get();
  }

  ASSERT_FALSE(watcher.expired());
  EXPECT_TRUE(NetworksRegistry::owns(view));
  EXPECT_EQ(NetworksRegistry::find(view), watcher.lock());

  EXPECT_TRUE(NetworksRegistry::remove(view));

  EXPECT_TRUE(watcher.expired());
  EXPECT_FALSE(NetworksRegistry::remove(view));
  EXPECT_EQ(NetworksRegistry::size(), registered);
}

TEST_F(CTEST_NetworksCApi, the_registry_forgets_the_released_networks)
{
  ANetworkAdaptorPtr net = ANetworkAdaptor::create();
  net->allocate_layers(1);

  NetworksRegistry::add(net, false);

  const network view = *net->get();

  EXPECT_FALSE(NetworksRegistry::owns(view));
  EXPECT_EQ(NetworksRegistry::find(view), net);
  EXPECT_EQ(NetworksRegistry::size(), registered + 1U);

  // registering it again changes the ownership only
  NetworksRegistry::add(net, true);

  EXPECT_TRUE(NetworksRegistry::owns(view));
  EXPECT_EQ(NetworksRegistry::size(), registered + 1U);

  NetworksRegistry::add(net, false);
  net.reset();

  // the weak entry of the released network
  EXPECT_EQ(NetworksRegistry::find(view), nullptr);
  EXPECT_FALSE(NetworksRegistry::owns(view));
  EXPECT_EQ(NetworksRegistry::size(), registered);

  NetworksRegistry::remove(view);
}

TEST_F(CTEST_NetworksCApi, the_registry_serves_the_threads)
{
  constexpr int THREADS = 4;
  constexpr int NETWORKS = 100;

  std::atomic_int failures{0};
  std::vector<std::thread> threads;

  for (int thread = 0; thread < THREADS; ++thread) {
    threads.emplace_back([&failures]() {
      for (int k = 0; k < NETWORKS; ++k) {
        const ANetworkAdaptorPtr net = ANetworkAdaptor::create();
        net->allocate_layers(1);

        NetworksRegistry::add(net, (k % 2) == 0);

        if (NetworksRegistry::find(*net->get()) != net) ++failures;
        if (!NetworksRegistry::remove(*net->get())) ++failures;
      }
    });
  }

  for (std::thread& thread : threads) {
    thread.join();
  }

  EXPECT_EQ(failures.load(), 0);
  EXPECT_EQ(NetworksRegistry::size(), registered);
}
