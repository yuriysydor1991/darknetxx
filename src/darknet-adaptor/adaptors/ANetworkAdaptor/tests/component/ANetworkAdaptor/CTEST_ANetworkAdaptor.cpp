#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "NetworkChecks.h"
#include "WeightsNetSpec.h"
#include "WeightsVariants.h"
#include "src/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/NetworkError.h"

using namespace testing;
using namespace weights_factory;

using darknet_adaptor::adaptors::ALayerAdaptor;
using darknet_adaptor::adaptors::ALayerAdaptorPtr;
using darknet_adaptor::adaptors::ANetworkAdaptor;
using darknet_adaptor::adaptors::ANetworkAdaptorPtr;
using darknet_adaptor::adaptors::NetworkError;
using darknet_adaptor::adaptors::networkptr;
using darknet_adaptor::loaders::OrigDefaultLoader;

namespace
{

/// @brief The directory of the files of the current test, removed with them.
class Workspace
{
 public:
  Workspace()
  {
    const TestInfo* const info = UnitTest::GetInstance()->current_test_info();
    std::string name =
        std::string{info->test_suite_name()} + "." + info->name();

    for (char& symbol : name) {
      if (symbol == '/') symbol = '_';
    }

    mdir = std::string{CTEST_ANetworkAdaptor_DATA_DIR} + "/generated/" + name +
           "." + std::to_string(std::random_device{}());

    std::filesystem::create_directories(mdir);
  }

  ~Workspace()
  {
    std::error_code ignored;
    std::filesystem::remove_all(mdir, ignored);
  }

  Workspace(const Workspace&) = delete;
  Workspace& operator=(const Workspace&) = delete;

  std::string file(const std::string& name) const { return mdir + "/" + name; }

  std::string write(const std::string& name, const std::string& content) const
  {
    const std::string path = file(name);
    std::ofstream out{path};
    out << content;
    return path;
  }

 private:
  std::string mdir;
};

/// @brief The deterministic values of the [-1, 1) range.
std::vector<float> pattern(size_t count, uint32_t seed)
{
  std::vector<float> values(count);
  uint32_t state = seed * 2654435761U + 1U;

  for (float& value : values) {
    state = state * 1664525U + 1013904223U;
    value =
        static_cast<float>(state >> 8) / static_cast<float>(1U << 24) * 2.f -
        1.f;
  }

  return values;
}

/// @brief The network input of the batch, the resized network input
/// included (the resize keeps the inputs field of the cfg input).
size_t input_count(const network& net)
{
  const int single = (net.w > 0 && net.h > 0 && net.c > 0)
                         ? net.w * net.h * net.c
                         : net.inputs;

  return static_cast<size_t>(single) * static_cast<size_t>(net.batch);
}

/// @brief Computes the network of the input made of the seed.
void predict(network& net, uint32_t seed)
{
  std::vector<float> input = pattern(input_count(net), seed);

  network_predict(net, input.data());
}

/// @brief The output of the layer of it's batch.
std::vector<float> layer_output(const layer& l)
{
  if (l.output == nullptr) return {};

  const size_t count =
      static_cast<size_t>(l.outputs) * static_cast<size_t>(l.batch);

  return std::vector<float>(l.output, l.output + count);
}

/// @brief The own buffers of the layer adaptors, which the layer geometry
/// sizes (and the ones of the weights, which it doesn't).
// clang-format off
#define LAYER_BUFFERS(X)                                                     \
  X(output) X(delta) X(x) X(x_norm) X(activation_input) X(binary_input)      \
  X(bin_re_packed_input) X(t_bit_input) X(rand) X(squared) X(norms)          \
  X(spatial_mean) X(loss) X(state) X(prev_state) X(forgot_state)             \
  X(forgot_delta) X(state_delta) X(combine_cpu) X(combine_delta_cpu)         \
  X(concat) X(concat_delta) X(cos_sim) X(exp_cos_sim) X(p_constrastive)      \
  X(embedding_output) X(indexes) X(labels) X(class_ids) X(cost) X(z_cpu)     \
  X(r_cpu) X(h_cpu) X(stored_h_cpu) X(prev_state_cpu) X(temp_cpu)            \
  X(temp2_cpu) X(temp3_cpu) X(dh_cpu) X(hh_cpu) X(prev_cell_cpu) X(cell_cpu) \
  X(f_cpu) X(i_cpu) X(g_cpu) X(o_cpu) X(c_cpu) X(stored_c_cpu) X(dc_cpu)     \
  X(weights) X(biases) X(scales) X(rolling_mean) X(rolling_variance)         \
  X(binary_weights) X(mean_arr) X(align_bit_weights) X(input_sizes)          \
  X(input_layers) X(drop_blocks_scale)

/// @brief The sub-layers of the recurrent and the antialiasing layers.
#define SUB_LAYERS(X)                                                        \
  X(input_layer) X(self_layer) X(output_layer) X(reset_layer)                \
  X(update_layer) X(state_layer) X(input_gate_layer) X(state_gate_layer)     \
  X(input_save_layer) X(state_save_layer) X(input_state_layer)               \
  X(state_state_layer) X(input_z_layer) X(state_z_layer) X(input_r_layer)    \
  X(state_r_layer) X(input_h_layer) X(state_h_layer) X(wz) X(uz) X(wr) X(ur) \
  X(wh) X(uh) X(uo) X(wo) X(vo) X(uf) X(wf) X(vf) X(ui) X(wi) X(vi) X(ug)    \
  X(wg)
// clang-format on

using Shape = std::map<std::string, int64_t>;

/// @brief The geometry and the own buffer sizes of the layer and it's
/// sub-layers.
void collect_shape(ALayerAdaptor& adaptor, const std::string& prefix,
                   Shape& shape)
{
  const layer& l = *adaptor.get();

#define FIELD(name) shape[prefix + #name] = static_cast<int64_t>(l.name);
  FIELD(type)
  // the [dropout] layer passes the previous layer output on, darknet sets it's
  // input size on the resize only
  if (l.type != DROPOUT) {
    FIELD(w)
    FIELD(h)
  }
  FIELD(c)
  FIELD(out_w)
  FIELD(out_h)
  FIELD(out_c)
  FIELD(inputs)
  FIELD(outputs)
  FIELD(batch)
  FIELD(workspace_size)
  FIELD(bit_align)
  FIELD(hidden)
#undef FIELD

#define BUFFER(name) \
  shape[prefix + #name + "[]"] = static_cast<int64_t>(adaptor.name().size());
  LAYER_BUFFERS(BUFFER)
#undef BUFFER

#define SUB(name)                                     \
  if (const ALayerAdaptorPtr sub = adaptor.name()) {  \
    collect_shape(*sub, prefix + #name + ".", shape); \
  }
  SUB_LAYERS(SUB)
#undef SUB
}

Shape shape_of(ANetworkAdaptor& net)
{
  Shape shape;
  const network& view = *net.get();

  shape["net.n"] = view.n;
  shape["net.w"] = view.w;
  shape["net.h"] = view.h;
  shape["net.outputs"] = view.outputs;

  size_t workspace_size = 0U;

  for (int index = 0; index < view.n; ++index) {
    const ALayerAdaptorPtr adaptor = net.layer_adaptor(index);

    if (!adaptor) {
      shape["layer " + std::to_string(index)] = -1;
      continue;
    }

    collect_shape(*adaptor, "layer " + std::to_string(index) + ".", shape);

    if (view.layers[index].workspace_size > workspace_size) {
      workspace_size = view.layers[index].workspace_size;
    }
  }

  // the network workspace of the biggest layer one
  shape["net.workspace[]"] = static_cast<int64_t>(net.workspace().size());
  shape["net.workspace_floats"] = static_cast<int64_t>(
      (workspace_size + sizeof(float) - 1U) / sizeof(float));

  return shape;
}

AssertionResult same_shapes(const Shape& actual, const Shape& expected)
{
  std::ostringstream differences;

  for (const auto& [key, value] : expected) {
    const auto found = actual.find(key);

    if (found == actual.end()) {
      differences << "\n  " << key << ": missing, expected " << value;
    } else if (found->second != value) {
      differences << "\n  " << key << ": " << found->second << ", expected "
                  << value;
    }
  }

  for (const auto& [key, value] : actual) {
    if (expected.find(key) == expected.end()) {
      differences << "\n  " << key << ": " << value << ", not expected";
    }
  }

  if (differences.str().empty()) return AssertionSuccess();

  return AssertionFailure() << "the shapes differ:" << differences.str();
}

/// @brief Every layer computed the same output of the same input.
AssertionResult same_outputs(const network& actual, const network& expected)
{
  if (actual.n != expected.n) {
    return AssertionFailure()
           << "the layers count " << actual.n << " vs " << expected.n;
  }

  for (int index = 0; index < actual.n; ++index) {
    const std::vector<float> a = layer_output(actual.layers[index]);
    const std::vector<float> e = layer_output(expected.layers[index]);

    if (a.size() != e.size()) {
      return AssertionFailure() << "the layer " << index << " output of "
                                << a.size() << " values, expected " << e.size();
    }

    for (size_t k = 0; k < a.size(); ++k) {
      // the same computation of the same values, bit to bit
      if (std::memcmp(&a[k], &e[k], sizeof(float)) != 0) {
        return AssertionFailure()
               << "the layer " << index << " ("
               << layer_type_2_string(actual.layers[index].type)
               << ") output value " << k << ": " << a[k] << ", expected "
               << e[k];
      }
    }
  }

  return AssertionSuccess();
}

/// @brief Copies the trainable parameters of the layer and it's sub-layers
/// into the same layer of the other network, the loading options of the
/// layers (dontload, flipped and the like) aside.
void copy_parameters(ALayerAdaptor& from, ALayerAdaptor& to)
{
#define COPY(name)                                                     \
  if (!from.name().empty()) {                                          \
    ASSERT_EQ(from.name().size(), to.name().size()) << #name;          \
    std::copy(from.name().begin(), from.name().end(), to.get()->name); \
  }
  COPY(weights)
  COPY(biases)
  COPY(scales)
  COPY(rolling_mean)
  COPY(rolling_variance)
#undef COPY

#define SUB(name)                                 \
  if (const ALayerAdaptorPtr sub = from.name()) { \
    ASSERT_NE(to.name(), nullptr) << #name;       \
    copy_parameters(*sub, *to.name());            \
  }
  SUB_LAYERS(SUB)
#undef SUB
}

/// @brief The cfg of the [net] section of the given input and it's layers.
std::string net_cfg(int batch, int width, int height, int channels,
                    const std::string& layers)
{
  std::ostringstream cfg;
  cfg << "[net]\nbatch=" << batch << "\nsubdivisions=1\nwidth=" << width
      << "\nheight=" << height << "\nchannels=" << channels << "\n"
      << layers;
  return cfg.str();
}

/// @brief A network of the layers darknet resizes.
struct ResizeCase
{
  std::string name;
  /// the layers of the 32 x 32 x 3 input network
  std::string layers;
  int batch{2};
  int time_steps{1};
  /// the original darknet computes the network on the CPU
  bool computes{true};
};

void PrintTo(const ResizeCase& resize_case, std::ostream* os)
{
  *os << resize_case.name;
}

std::vector<ResizeCase> resize_cases()
{
  return {
      {"convolutional",
       "[convolutional]\nfilters=8\nsize=3\npad=1\nbatch_normalize=1\n"
       "activation=leaky\n"
       "[convolutional]\nfilters=8\nsize=3\nstride=2\npad=1\ngroups=2\n"
       "activation=mish\n"
       "[convolutional]\nfilters=16\nsize=3\npad=1\ndilation=2\n"
       "activation=swish\n"
       "[convolutional]\nfilters=8\nsize=3\nstride_x=2\nstride_y=1\npad=1\n"
       "activation=relu\n"
       "[convolutional]\nfilters=4\nsize=1\nactivation=linear\n"},
      {"xnor",
       "[convolutional]\nfilters=32\nsize=3\npad=1\nactivation=leaky\n"
       "[convolutional]\nfilters=16\nsize=3\npad=1\nxnor=1\n"
       "activation=leaky\n"
       "[convolutional]\nfilters=8\nsize=3\npad=1\nxnor=1\n"
       "activation=leaky\n"
       "[convolutional]\nfilters=4\nsize=1\nactivation=linear\n"},
      {"antialiasing",
       "[convolutional]\nfilters=8\nsize=3\nstride=2\npad=1\nantialiasing=1\n"
       "activation=leaky\n"
       "[convolutional]\nfilters=8\nsize=3\nstride=2\npad=1\nantialiasing=2\n"
       "activation=leaky\n"
       "[convolutional]\nfilters=4\nsize=3\npad=1\nactivation=linear\n"},
      {"pooling_antialiasing",
       "[convolutional]\nfilters=8\nsize=3\npad=1\nactivation=leaky\n"
       "[maxpool]\nsize=2\nstride=2\nantialiasing=1\n"
       "[maxpool]\nsize=2\nstride=2\nantialiasing=2\n"
       "[convolutional]\nfilters=4\nsize=3\npad=1\nactivation=linear\n"},
      {"pooling",
       "[convolutional]\nfilters=8\nsize=3\npad=1\nactivation=leaky\n"
       "[maxpool]\nsize=2\nstride=2\n"
       "[maxpool]\nsize=3\nstride=1\n"
       "[local_avgpool]\nsize=2\nstride=2\n"
       "[convolutional]\nfilters=8\nsize=1\nactivation=leaky\n"
       "[avgpool]\n"},
      {"route_shortcut_and_attention",
       "[convolutional]\nfilters=8\nsize=3\npad=1\nactivation=leaky\n"
       "[convolutional]\nfilters=8\nsize=3\npad=1\nactivation=leaky\n"
       "[shortcut]\nfrom=-2\nactivation=linear\n"
       "[shortcut]\nfrom=-2,-3\nweights_type=per_channel\n"
       "weights_normalization=softmax\n"
       "[route]\nlayers=-1,-3\n"
       "[route]\nlayers=-1\ngroups=2\ngroup_id=1\n"
       "[convolutional]\nfilters=8\nsize=1\nactivation=logistic\n"
       "[sam]\nfrom=-2\n"
       "[avgpool]\n"
       "[scale_channels]\nfrom=-3\n"
       "[batchnorm]\n"},
      {"reorganization",
       "[convolutional]\nfilters=8\nsize=3\npad=1\nactivation=leaky\n"
       "[reorg3d]\nstride=2\n"
       "[convolutional]\nfilters=8\nsize=1\nactivation=leaky\n"
       "[upsample]\nstride=2\n"
       "[reorg]\nstride=2\n"
       "[normalization]\n"
       "[dropout]\nprobability=0.3\n"
       "[convolutional]\nfilters=4\nsize=1\nactivation=linear\n"},
      {"yolo",
       "[convolutional]\nfilters=8\nsize=3\nstride=2\npad=1\n"
       "activation=leaky\n"
       "[convolutional]\nfilters=14\nsize=1\nactivation=linear\n"
       "[yolo]\nmask=0,1\nanchors=4,6, 8,10, 12,14\nclasses=2\nnum=3\n"
       "[route]\nlayers=-3\n"
       "[convolutional]\nfilters=11\nsize=1\nactivation=linear\n"
       "[Gaussian_yolo]\nmask=1\nanchors=4,6, 8,10\nclasses=2\nnum=2\n"},
      {"region",
       "[convolutional]\nfilters=8\nsize=3\nstride=2\npad=1\n"
       "activation=leaky\n"
       "[convolutional]\nfilters=21\nsize=1\nactivation=linear\n"
       "[region]\nanchors=1,1, 2,2, 3,3\nclasses=2\ncoords=4\nnum=3\n"},
      {"cost",
       "[convolutional]\nfilters=4\nsize=3\npad=1\nactivation=logistic\n"
       "[cost]\ntype=sse\n"},
      {"recurrent",
       "[crnn]\nhidden=4\noutput=4\nsize=3\npad=1\nactivation=leaky\n"
       "batch_normalize=1\n"
       "[conv_lstm]\noutput=4\nsize=3\npad=1\n"
       "[conv_lstm]\noutput=4\nsize=3\npad=1\npeephole=1\n",
       4, 2},
      // the CPU forward of the bottleneck layer asserts the channels of it's
      // not made wi, wg and wo sub-layers in the original darknet
      {"recurrent_bottleneck",
       "[conv_lstm]\noutput=4\nsize=3\npad=1\nbottleneck=1\n"
       "[conv_lstm]\noutput=4\nsize=3\npad=1\npeephole=1\nbottleneck=1\n",
       4, 2, false},
  };
}

}  // namespace

class CTEST_ANetworkAdaptor : public Test
{
 public:
  OrigDefaultLoader loader;
  Workspace workspace;

  /// @brief The network of the cfg: the training one or the single image
  /// inference one.
  ANetworkAdaptorPtr parse(const std::string& cfg, bool train,
                           const std::string& name = "net.cfg")
  {
    const std::string path = workspace.write(name, cfg);
    return train ? loader.parse_network_cfg(path)
                 : loader.parse_network_cfg_custom(path, 1, 1);
  }

  /// @brief Gives the network the parameters of the other one of the same
  /// layers.
  void transfer_weights(ANetworkAdaptor& from, ANetworkAdaptor& to)
  {
    const int n = from.get()->n;

    ASSERT_EQ(to.get()->n, n);

    for (int index = 0; index < n; ++index) {
      const ALayerAdaptorPtr source = from.layer_adaptor(index);
      const ALayerAdaptorPtr target = to.layer_adaptor(index);

      ASSERT_NE(source, nullptr);
      ASSERT_NE(target, nullptr);

      copy_parameters(*source, *target);
    }
  }
};

/*
 * The network memory.
 */

TEST_F(CTEST_ANetworkAdaptor, a_new_network_keeps_the_shared_counters)
{
  const ANetworkAdaptorPtr net = ANetworkAdaptor::create();
  const network& view = *net->get();

  for (const void* counter :
       {static_cast<const void*>(view.seen),
        static_cast<const void*>(view.cur_iteration),
        static_cast<const void*>(view.cuda_graph_ready),
        static_cast<const void*>(view.badlabels_reject_threshold),
        static_cast<const void*>(view.delta_rolling_max),
        static_cast<const void*>(view.delta_rolling_avg),
        static_cast<const void*>(view.delta_rolling_std),
        static_cast<const void*>(view.total_bbox),
        static_cast<const void*>(view.rewritten_bbox)}) {
    EXPECT_NE(counter, nullptr);
  }

  EXPECT_EQ(*view.seen, 0U);
  EXPECT_EQ(*view.cur_iteration, 0);
  EXPECT_EQ(view.n, 0);
  EXPECT_EQ(view.workspace, nullptr);
}

TEST_F(CTEST_ANetworkAdaptor, layers_binds_every_adaptor_to_it_s_slot)
{
  const ANetworkAdaptorPtr net = ANetworkAdaptor::create();
  const ALayerAdaptorPtr conv = ALayerAdaptor::create();
  const ALayerAdaptorPtr pool = ALayerAdaptor::create();
  const ALayerAdaptorPtr shared = ALayerAdaptor::create();

  conv->type(CONVOLUTIONAL);
  pool->type(MAXPOOL);
  shared->type(CONVOLUTIONAL);
  shared->share_layer(conv);

  net->layers({conv, nullptr, pool, shared});

  const network& view = *net->get();

  ASSERT_EQ(view.n, 4);
  EXPECT_EQ(view.layers[0].type, CONVOLUTIONAL);
  EXPECT_EQ(view.layers[2].type, MAXPOOL);

  EXPECT_EQ(conv->get().get(), &view.layers[0]);
  EXPECT_EQ(pool->get().get(), &view.layers[2]);
  EXPECT_EQ(net->layer_adaptor(0), conv);
  EXPECT_EQ(net->layer_adaptor(1), nullptr);
  EXPECT_EQ(net->layer_adaptor(4), nullptr);
  EXPECT_EQ(net->layer_adaptor(-1), nullptr);

  // the empty slot stays zeroed
  layer zeroed;
  std::memset(&zeroed, 0, sizeof(zeroed));
  EXPECT_EQ(std::memcmp(&view.layers[1], &zeroed, sizeof(layer)), 0);

  // the layers referring to the other ones point at their slots
  EXPECT_EQ(view.layers[3].share_layer, &view.layers[0]);
}

TEST_F(CTEST_ANetworkAdaptor, allocate_layers_releases_the_layer_adaptors)
{
  const ANetworkAdaptorPtr net = ANetworkAdaptor::create();
  std::weak_ptr<ALayerAdaptor> watcher;

  {
    const ALayerAdaptorPtr conv = ALayerAdaptor::create();
    watcher = conv;
    net->layers({conv});
  }

  ASSERT_FALSE(watcher.expired());

  net->allocate_layers(3);

  EXPECT_TRUE(watcher.expired());
  EXPECT_EQ(net->get()->n, 3);
  EXPECT_EQ(net->layer_adaptor(0), nullptr);

  // the empty network keeps a layers array, which tells it apart
  net->allocate_layers(0);

  EXPECT_EQ(net->get()->n, 0);
  EXPECT_NE(net->get()->layers, nullptr);
}

TEST_F(CTEST_ANetworkAdaptor, the_view_keeps_the_network_alive)
{
  ANetworkAdaptorPtr net =
      parse(net_cfg(1, 16, 16, 3,
                    "[convolutional]\nfilters=4\nsize=3\npad=1\n"
                    "activation=leaky\n"),
            false);

  const std::weak_ptr<ANetworkAdaptor> watcher = net;
  networkptr view = net->get();

  net.reset();

  ASSERT_FALSE(watcher.expired());

  predict(*view, 1U);
  EXPECT_EQ(view->output, view->layers[0].output);

  view.reset();

  EXPECT_TRUE(watcher.expired());
}

TEST_F(CTEST_ANetworkAdaptor, link_output_takes_the_last_not_cost_layer)
{
  const ANetworkAdaptorPtr net =
      parse(net_cfg(2, 8, 8, 3,
                    "[convolutional]\nfilters=4\nsize=3\npad=1\n"
                    "activation=logistic\n"
                    "[cost]\n"),
            true);

  const network& view = *net->get();

  EXPECT_EQ(view.output, view.layers[0].output);
  EXPECT_EQ(view.outputs, view.layers[0].outputs);
  EXPECT_TRUE(net->output().empty());
}

TEST_F(CTEST_ANetworkAdaptor, workspace_bytes_allocates_the_whole_floats)
{
  const ANetworkAdaptorPtr net = ANetworkAdaptor::create();

  net->workspace_bytes(10U);

  EXPECT_EQ(net->workspace().size(), 3U);
  EXPECT_EQ(net->get()->workspace, net->workspace().data());

  net->workspace_bytes(0U);

  EXPECT_EQ(net->get()->workspace, nullptr);
}

/*
 * The resize of the networks.
 */

class CTEST_ANetworkAdaptor_resize : public CTEST_ANetworkAdaptor,
                                     public WithParamInterface<ResizeCase>
{
};

TEST_P(CTEST_ANetworkAdaptor_resize, resize_matches_the_fresh_parse)
{
  const ResizeCase& resize_case = GetParam();

  const auto cfg_of = [&resize_case](int width, int height) {
    std::string cfg =
        net_cfg(resize_case.batch, width, height, 3, resize_case.layers);
    if (resize_case.time_steps > 1) {
      cfg.insert(cfg.find('\n') + 1,
                 "time_steps=" + std::to_string(resize_case.time_steps) + "\n");
    }
    return cfg;
  };

  // the bigger, the smaller and the bigger again network input
  const std::vector<std::pair<int, int>> sizes{{64, 96}, {32, 32}, {64, 64}};

  for (const bool train : {true, false}) {
    SCOPED_TRACE(train ? "the training network" : "the inference network");

    for (size_t last = 0U; last < sizes.size(); ++last) {
      // the recurrent layers keep their state between the computations, so
      // every network computes once
      const ANetworkAdaptorPtr resized =
          parse(cfg_of(32, 32), train, "small.cfg");

      for (size_t step = 0U; step <= last; ++step) {
        const auto [width, height] = sizes[step];
        SCOPED_TRACE(std::to_string(width) + " x " + std::to_string(height));

        const ANetworkAdaptorPtr fresh =
            parse(cfg_of(width, height), train, "fresh.cfg");

        resized->resize(width, height);

        ASSERT_TRUE(same_shapes(shape_of(*resized), shape_of(*fresh)));

        // the output of the last not COST layer
        EXPECT_EQ(resized->get()->output, get_network_output(*resized->get()));

        if (step < last || !resize_case.computes) continue;

        transfer_weights(*resized, *fresh);

        predict(*resized->get(), 7U);
        predict(*fresh->get(), 7U);

        ASSERT_TRUE(same_outputs(*resized->get(), *fresh->get()));
      }
    }
  }
}

INSTANTIATE_TEST_SUITE_P(Layers, CTEST_ANetworkAdaptor_resize,
                         ValuesIn(resize_cases()),
                         [](const TestParamInfo<ResizeCase>& param_info) {
                           return param_info.param.name;
                         });

TEST_F(CTEST_ANetworkAdaptor,
       resize_throws_for_the_layers_darknet_doesnt_resize)
{
  for (const std::string& layers :
       {std::string{"[convolutional]\nfilters=4\nsize=3\npad=1\n"
                    "[connected]\noutput=4\n"},
        std::string{"[local]\nfilters=4\nsize=3\npad=1\n"},
        std::string{"[convolutional]\nfilters=4\nsize=3\npad=1\n"
                    "[softmax]\n"},
        std::string{"[convolutional]\nfilters=4\nsize=3\npad=1\n"
                    "[empty]\n"}}) {
    SCOPED_TRACE(layers);

    const ANetworkAdaptorPtr net = parse(net_cfg(1, 16, 16, 3, layers), false);

    EXPECT_THROW(net->resize(32, 32), NetworkError);
  }
}

TEST_F(CTEST_ANetworkAdaptor, resize_throws_for_the_borrowed_layers)
{
  const std::string cfg = net_cfg(2, 16, 16, 3,
                                  "[convolutional]\nfilters=4\nsize=3\npad=1\n"
                                  "activation=leaky\n");

  const ANetworkAdaptorPtr train = parse(cfg, true);
  const ANetworkAdaptorPtr map = parse(cfg, false);

  // the mAP network computes with the layers of the training one
  map->allocate_layers(train->get()->n);
  ANetworkAdaptor::copy_weights_net(*train->get(), *map->get());

  EXPECT_THROW(map->resize(32, 32), NetworkError);
}

/*
 * The generated networks.
 */

namespace
{

/// @brief The original darknet computes the network on the CPU: the forward
/// of the bottleneck [conv_lstm] layer asserts the channels of it's not made
/// wi, wg and wo sub-layers.
bool computes(const NetSpec& spec)
{
  for (const LayerSpec& layer : spec.layers) {
    if (layer.kind == LayerKind::ConvLstm && layer.bottleneck) return false;
  }

  return true;
}

bool resizable(const NetSpec& spec)
{
  if (!spec.has_2d_layer()) return false;

  for (const LayerSpec& layer : spec.layers) {
    switch (layer.kind) {
      case LayerKind::Convolutional:
      case LayerKind::Batchnorm:
      case LayerKind::Shortcut:
      case LayerKind::Crnn:
      case LayerKind::ConvLstm:
      case LayerKind::Maxpool:
      case LayerKind::LocalAvgpool:
      case LayerKind::Avgpool:
      case LayerKind::Route:
      case LayerKind::Upsample:
      case LayerKind::Reorg:
      case LayerKind::Dropout:
      case LayerKind::Normalization:
      case LayerKind::Yolo:
        break;
      default:
        return false;
    }
  }

  return true;
}

std::vector<WeightsVariant> generated_variants()
{
  std::vector<WeightsVariant> all = variants::layers();
  const std::vector<WeightsVariant> random = variants::random(
      variants::DEFAULT_RANDOM_SEED, variants::DEFAULT_RANDOM_COUNT);

  all.insert(all.end(), random.begin(), random.end());

  return all;
}

/// @brief The generated networks the CPU computes, see computes().
std::vector<WeightsVariant> computed_variants()
{
  std::vector<WeightsVariant> computed;

  for (const WeightsVariant& variant : generated_variants()) {
    if (computes(variant.spec)) computed.push_back(variant);
  }

  return computed;
}

std::vector<WeightsVariant> resizable_variants()
{
  std::vector<WeightsVariant> resizable_ones;

  for (const WeightsVariant& variant : generated_variants()) {
    if (resizable(variant.spec)) resizable_ones.push_back(variant);
  }

  return resizable_ones;
}

std::string variant_name(const TestParamInfo<WeightsVariant>& info)
{
  return info.param.name;
}

}  // namespace

class CTEST_ANetworkAdaptor_generated
    : public CTEST_ANetworkAdaptor,
      public WithParamInterface<WeightsVariant>
{
 public:
  ANetworkAdaptorPtr parse_spec(const NetSpec& spec, ParseMode mode,
                                const std::string& name)
  {
    return parse(spec.cfg(), mode == ParseMode::Train, name);
  }
};

TEST_P(CTEST_ANetworkAdaptor_generated, predicts)
{
  const WeightsVariant& variant = GetParam();

  const ANetworkAdaptorPtr net =
      parse_spec(variant.spec, variant.mode, "net.cfg");

  ASSERT_TRUE(matches_spec(*net->get(), variant.spec, variant.mode));

  predict(*net->get(), 3U);

  EXPECT_EQ(net->get()->output, get_network_output(*net->get()));
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_ANetworkAdaptor_generated,
                         ValuesIn(computed_variants()), variant_name);

class CTEST_ANetworkAdaptor_generated_resize
    : public CTEST_ANetworkAdaptor_generated
{
};

TEST_P(CTEST_ANetworkAdaptor_generated_resize, resize_matches_the_fresh_parse)
{
  const WeightsVariant& variant = GetParam();

  NetSpec doubled = variant.spec;
  doubled.width *= 2;
  doubled.height *= 2;

  const ANetworkAdaptorPtr resized =
      parse_spec(variant.spec, variant.mode, "small.cfg");
  const ANetworkAdaptorPtr fresh = parse_spec(doubled, variant.mode, "big.cfg");

  resized->resize(doubled.width, doubled.height);

  ASSERT_TRUE(same_shapes(shape_of(*resized), shape_of(*fresh)))
      << variant.spec.cfg();

  // the shapes only of the networks the CPU doesn't compute
  if (!computes(variant.spec)) return;

  transfer_weights(*resized, *fresh);

  predict(*resized->get(), 5U);
  predict(*fresh->get(), 5U);

  EXPECT_TRUE(same_outputs(*resized->get(), *fresh->get()))
      << variant.spec.cfg();
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_ANetworkAdaptor_generated_resize,
                         ValuesIn(resizable_variants()), variant_name);

/*
 * The batch, the fuse and the binary weights.
 */

TEST_F(CTEST_ANetworkAdaptor, set_batch_keeps_the_buffers)
{
  const ANetworkAdaptorPtr net =
      parse(net_cfg(4, 16, 16, 3,
                    "[convolutional]\nfilters=8\nsize=3\npad=1\n"
                    "batch_normalize=1\nactivation=leaky\n"
                    "[maxpool]\nsize=2\nstride=2\n"
                    "[convolutional]\nfilters=4\nsize=3\npad=1\n"
                    "activation=linear\n"
                    "[connected]\noutput=6\nactivation=linear\n"),
            true);

  network& view = *net->get();

  ASSERT_EQ(view.batch, 4);

  std::vector<const float*> outputs;
  for (int index = 0; index < view.n; ++index) {
    outputs.push_back(view.layers[index].output);
  }

  net->set_batch(1);

  EXPECT_EQ(view.batch, 1);

  size_t workspace_size = 0U;

  for (int index = 0; index < view.n; ++index) {
    SCOPED_TRACE("the layer " + std::to_string(index));

    EXPECT_EQ(view.layers[index].batch, 1);
    EXPECT_EQ(view.layers[index].output, outputs[static_cast<size_t>(index)]);

    if (view.layers[index].workspace_size > workspace_size) {
      workspace_size = view.layers[index].workspace_size;
    }
  }

  EXPECT_GE(net->workspace().size() * sizeof(float), workspace_size);

  predict(view, 2U);
}

TEST_F(CTEST_ANetworkAdaptor, fuse_conv_batchnorm_keeps_the_output)
{
  const ANetworkAdaptorPtr net =
      parse(net_cfg(1, 16, 16, 3,
                    "[convolutional]\nfilters=8\nsize=3\npad=1\n"
                    "batch_normalize=1\nactivation=leaky\n"
                    "[convolutional]\nfilters=8\nsize=3\npad=1\ngroups=2\n"
                    "batch_normalize=1\nactivation=mish\n"
                    "[convolutional]\nfilters=8\nsize=3\npad=1\ngroups=2\n"
                    "batch_normalize=1\nactivation=leaky\nshare_index=-1\n"
                    "[shortcut]\nfrom=-3\nweights_type=per_feature\n"
                    "weights_normalization=softmax\n"
                    "[convolutional]\nfilters=4\nsize=1\nactivation=linear\n"),
            false);

  network& view = *net->get();

  // the batch normalization of the trained network
  for (const int index : {0, 1}) {
    layer& l = view.layers[index];

    for (int f = 0; f < l.n; ++f) {
      l.scales[f] = 0.5f + 0.1f * static_cast<float>(f);
      l.rolling_mean[f] = 0.05f * static_cast<float>(f) - 0.1f;
      l.rolling_variance[f] = 0.5f + 0.2f * static_cast<float>(f);
      l.biases[f] = 0.01f * static_cast<float>(f);
    }
  }

  view.layers[3].weights[0] = 0.3f;
  view.layers[3].weights[1] = -0.7f;

  predict(view, 11U);
  const std::vector<float> expected = layer_output(view.layers[view.n - 1]);

  net->fuse_conv_batchnorm();

  for (const int index : {0, 1, 2}) {
    SCOPED_TRACE("the layer " + std::to_string(index));

    const layer& l = view.layers[index];

    EXPECT_EQ(l.batch_normalize, 0);
    EXPECT_EQ(l.scales, nullptr);
    EXPECT_EQ(l.rolling_mean, nullptr);
    EXPECT_EQ(l.rolling_variance, nullptr);
    EXPECT_TRUE(net->layer_adaptor(index)->scales().empty());
    EXPECT_TRUE(net->layer_adaptor(index)->rolling_mean().empty());
  }

  // the shared layer computes with the fused weights of the layer it shares
  EXPECT_EQ(view.layers[2].weights, view.layers[1].weights);
  EXPECT_EQ(view.layers[3].weights_normalization, NO_NORMALIZATION);

  predict(view, 11U);
  const std::vector<float> actual = layer_output(view.layers[view.n - 1]);

  ASSERT_EQ(actual.size(), expected.size());

  for (size_t k = 0; k < actual.size(); ++k) {
    // the fuse epsilon differs from the normalization one a bit
    EXPECT_NEAR(actual[k], expected[k], 1e-4f + 1e-3f * std::fabs(expected[k]))
        << "the output value " << k;
  }
}

TEST_F(CTEST_ANetworkAdaptor, calculate_binary_weights_of_the_xnor_layers)
{
  const ANetworkAdaptorPtr net =
      parse(net_cfg(1, 16, 16, 3,
                    "[convolutional]\nfilters=32\nsize=3\npad=1\n"
                    "activation=leaky\n"
                    "[convolutional]\nfilters=16\nsize=3\npad=1\nxnor=1\n"
                    "activation=leaky\n"
                    "[convolutional]\nfilters=8\nsize=3\npad=1\nxnor=1\n"
                    "bin_output=1\nactivation=leaky\n"
                    "[convolutional]\nfilters=4\nsize=1\nactivation=linear\n"),
            false);

  network& view = *net->get();

  net->calculate_binary_weights();

  EXPECT_EQ(view.layers[0].align_bit_weights, nullptr);
  EXPECT_EQ(view.layers[3].align_bit_weights, nullptr);

  for (const int index : {1, 2}) {
    SCOPED_TRACE("the layer " + std::to_string(index));

    const layer& l = view.layers[index];
    const ALayerAdaptorPtr adaptor = net->layer_adaptor(index);

    ASSERT_NE(l.align_bit_weights, nullptr);
    EXPECT_EQ(adaptor->align_bit_weights().size(),
              static_cast<size_t>(l.align_bit_weights_size));
    EXPECT_GT(l.new_lda, 0);

    // the mean of the absolute weights of every filter
    const int k = l.size * l.size * l.c;

    for (int f = 0; f < l.n; ++f) {
      double sum = 0.0;
      for (int i = 0; i < k; ++i) {
        sum += std::fabs(static_cast<double>(l.weights[f * k + i]));
      }

      EXPECT_NEAR(l.mean_arr[f], sum / k, 1e-5) << "the filter " << f;
    }
  }

  // the binary output layer passes the binarized output on
  EXPECT_EQ(view.layers[2].activation, LINEAR);

  // the bit packed XNOR computation, of the resized network as well
  predict(view, 4U);

  net->resize(32, 32);
  predict(view, 4U);

  for (const float value : layer_output(view.layers[view.n - 1])) {
    ASSERT_TRUE(std::isfinite(value));
  }
}

TEST_F(CTEST_ANetworkAdaptor, copy_weights_net_points_at_the_training_layers)
{
  const std::string cfg =
      net_cfg(4, 16, 16, 3,
              "[convolutional]\nfilters=4\nsize=3\npad=1\n"
              "batch_normalize=1\nactivation=leaky\n"
              "[maxpool]\nsize=2\nstride=2\n"
              "[convolutional]\nfilters=4\nsize=1\nactivation=linear\n");

  const ANetworkAdaptorPtr train = parse(cfg, true);
  const ANetworkAdaptorPtr map = parse(cfg, false);

  map->allocate_layers(train->get()->n);
  ANetworkAdaptor::copy_weights_net(*train->get(), *map->get());

  const network& source = *train->get();
  const network& copy = *map->get();

  for (int index = 0; index < source.n; ++index) {
    SCOPED_TRACE("the layer " + std::to_string(index));

    const layer& from = source.layers[index];
    const layer& to = copy.layers[index];

    EXPECT_EQ(to.type, from.type);
    EXPECT_EQ(to.output, from.output);
    EXPECT_EQ(to.weights, from.weights);
    EXPECT_EQ(to.rolling_mean, from.rolling_mean);
    EXPECT_EQ(to.outputs, from.outputs);
    EXPECT_EQ(to.batch, 1);
    EXPECT_EQ(to.steps, 1);
    EXPECT_EQ(to.train, 0);
    EXPECT_EQ(from.batch, 4);
  }
}
