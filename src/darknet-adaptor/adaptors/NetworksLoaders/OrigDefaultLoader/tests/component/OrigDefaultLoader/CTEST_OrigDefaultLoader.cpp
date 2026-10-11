#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "CTEST_OrigDefaultLoader_provider.h"
#include "LibraryContext.h"
#include "NetworkChecks.h"
#include "NetworkTracer.h"
#include "WeightsNetSpec.h"
#include "WeightsVariants.h"
#include "src/darknet-adaptor/DarknetContext.h"
#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/NetworkError.h"
#include "src/darknet-adaptor/adaptors/NetworksCApi/NetworksRegistry.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Writer.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkFiles.h"
#include "src/darknet-adaptor/adaptors/TrainingJournal/TrainingJournal.h"

using namespace testing;
using namespace weights_factory;

using darknet_adaptor::DarknetContext;
using darknet_adaptor::adaptors::ALayerAdaptorPtr;
using darknet_adaptor::adaptors::ANetworkAdaptorPtr;
using darknet_adaptor::adaptors::NetworkError;
using darknet_adaptor::adaptors::NetworksRegistry;
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
    std::string name = info == nullptr ? std::string{"loader"}
                                       : std::string{info->test_suite_name()} +
                                             "." + info->name();

    for (char& symbol : name) {
      if (symbol == '/' || symbol == ' ') symbol = '_';
    }

    mdir = std::string{CTEST_OrigDefaultLoader_provider::GENERATED_DIR} + "/" +
           name + "." + std::to_string(std::random_device{}());

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

  /// @brief Writes the file content, returns the file path.
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

const char* const NET_HEADER =
    "[net]\nbatch=4\nsubdivisions=2\nwidth=16\nheight=16\nchannels=3\n";

/// @brief Every layer of the network is kept by it's adaptor, bound to it's
/// slot of the network layers array.
AssertionResult layers_bound(const ANetworkAdaptorPtr& net)
{
  const network& view = *net->get();

  for (int index = 0; index < view.n; ++index) {
    const ALayerAdaptorPtr adaptor = net->layer_adaptor(index);

    if (!adaptor) {
      return AssertionFailure() << "the layer " << index << " has no adaptor";
    }

    if (adaptor->get().get() != &view.layers[index]) {
      return AssertionFailure()
             << "the adaptor of the layer " << index << " keeps another struct";
    }
  }

  return AssertionSuccess();
}

}  // namespace

class CTEST_OrigDefaultLoader : public Test,
                                virtual public CTEST_OrigDefaultLoader_provider,
                                virtual public NetworkTracer
{
 public:
  ~CTEST_OrigDefaultLoader() = default;
  CTEST_OrigDefaultLoader() = default;

  OrigDefaultLoader loader;
  Workspace workspace;

  std::string cfg1{CFG_1};
  std::string w1{W_1};

  std::string cfg2{CFG_2};
  std::string w2{W_2};

  std::string cfg3{CFG_3};

  /// @brief Parses the network of the cfg content the given way.
  ANetworkAdaptorPtr parse(const std::string& content, bool train = true)
  {
    const std::string cfg = workspace.write("net.cfg", content);
    return train ? loader.parse_network_cfg(cfg)
                 : loader.parse_network_cfg_custom(cfg, 1, 1);
  }
};

/*
 * The reference networks.
 */

TEST_F(CTEST_OrigDefaultLoader, load_network_without_weights_success)
{
  // a cfg only load gives the randomly initialized network, same as in the
  // original darknet
  const ANetworkAdaptorPtr net = loader.load_network(cfg1, "", false);

  ASSERT_NE(net, nullptr);

  EXPECT_EQ(net->get()->n, 10);
  EXPECT_EQ(*net->get()->seen, 0U);
  EXPECT_TRUE(layers_bound(net));
}

TEST_F(CTEST_OrigDefaultLoader, load_network_no_valid_config_failure)
{
  EXPECT_THROW(loader.load_network("", w1, false), NetworkError);
  EXPECT_THROW(loader.load_network(workspace.file("missing.cfg"), w1, false),
               NetworkError);
}

TEST_F(CTEST_OrigDefaultLoader, load_network_no_valid_weights_failure)
{
  EXPECT_THROW(
      loader.load_network(cfg1, workspace.file("missing.weights"), false),
      NetworkError);
}

TEST_F(CTEST_OrigDefaultLoader, load_network_success)
{
  const ANetworkAdaptorPtr net = loader.load_network(cfg1, w1, false);

  ASSERT_NE(net, nullptr);
  EXPECT_TRUE(layers_bound(net));
}

TEST_F(CTEST_OrigDefaultLoader, load_network_2_success)
{
  const ANetworkAdaptorPtr net = loader.load_network(cfg2, w2, false);

  ASSERT_NE(net, nullptr);
  EXPECT_TRUE(layers_bound(net));
}

TEST_F(CTEST_OrigDefaultLoader, load_network_data_expected_success)
{
  const ANetworkAdaptorPtr net = loader.load_network(cfg1, w1, false);

  ASSERT_NE(net, nullptr);

  // trace_network(cfg1, w1, "layers_trace.cpp");

  EXPECT_TRUE(equal(net->get().get(), get_1()));
}

TEST_F(CTEST_OrigDefaultLoader, load_network_2_data_expected_success)
{
  const ANetworkAdaptorPtr net = loader.load_network(cfg2, w2, false);

  ASSERT_NE(net, nullptr);

  // trace_network(cfg2, w2, "layers_trace.cpp");

  EXPECT_TRUE(equal(net->get().get(), get_2()));
}

TEST_F(CTEST_OrigDefaultLoader, load_network_clear_drops_the_iteration)
{
  const ANetworkAdaptorPtr loaded = loader.load_network(cfg1, w1, false);
  const ANetworkAdaptorPtr cleared = loader.load_network(cfg1, w1, true);

  EXPECT_GT(*loaded->get()->seen, 0U);
  EXPECT_EQ(*cleared->get()->seen, 0U);
  EXPECT_EQ(*cleared->get()->cur_iteration, 0);
}

TEST_F(CTEST_OrigDefaultLoader, parse_network_with_optionless_sections_success)
{
  const ANetworkAdaptorPtr net = loader.parse_network_cfg(cfg3);

  EXPECT_EQ(net->get()->n, 3);
  EXPECT_EQ(net->get()->layers[1].type, AVGPOOL);
  EXPECT_EQ(net->get()->layers[2].type, SOFTMAX);
}

/*
 * The networks memory.
 */

TEST_F(CTEST_OrigDefaultLoader, every_layer_adaptor_keeps_it_s_slot)
{
  const ANetworkAdaptorPtr net =
      parse(std::string{NET_HEADER} +
            "[convolutional]\nfilters=4\nsize=3\npad=1\nactivation=leaky\n"
            "batch_normalize=1\n"
            "[maxpool]\nsize=2\nstride=2\n"
            "[route]\nlayers=-1\n"
            "[convolutional]\nfilters=21\nsize=1\nactivation=linear\n"
            "[yolo]\nmask=0,1,2\nanchors=1,2, 3,4, 5,6\nclasses=2\nnum=3\n");

  const network& view = *net->get();

  ASSERT_EQ(view.n, 5);
  EXPECT_TRUE(layers_bound(net));

  // the network output is the last layer one, the workspace is allocated
  EXPECT_EQ(view.output, view.layers[4].output);
  EXPECT_EQ(view.outputs, view.layers[4].outputs);
  EXPECT_NE(view.workspace, nullptr);

  // the counters shared by every copy of the network struct
  ASSERT_NE(view.seen, nullptr);
  ASSERT_NE(view.cur_iteration, nullptr);
  EXPECT_EQ(*view.seen, 0U);
  EXPECT_EQ(*view.cur_iteration, 0);
}

TEST_F(CTEST_OrigDefaultLoader, the_network_outlives_the_loader)
{
  ANetworkAdaptorPtr net;

  {
    OrigDefaultLoader scoped;
    net = scoped.load_network(cfg1, w1, false);
  }

  ASSERT_NE(net, nullptr);
  EXPECT_TRUE(equal(net->get().get(), get_1()));
}

TEST_F(CTEST_OrigDefaultLoader, inference_networks_skip_the_training_buffers)
{
  const std::string content =
      std::string{NET_HEADER} +
      "[convolutional]\nfilters=4\nsize=3\npad=1\nactivation=leaky\n"
      "batch_normalize=1\n"
      "[convolutional]\nfilters=4\nsize=3\npad=1\nactivation=leaky\n"
      "[shortcut]\nfrom=-2\n"
      "[maxpool]\nsize=2\nstride=2\n";

  const ANetworkAdaptorPtr train = parse(content, true);
  const ANetworkAdaptorPtr infer = parse(content, false);

  const layer& train_conv = train->get()->layers[0];
  const layer& infer_conv = infer->get()->layers[0];

  EXPECT_EQ(train->get()->batch, 2);
  EXPECT_EQ(infer->get()->batch, 1);

  EXPECT_NE(train_conv.delta, nullptr);
  EXPECT_NE(train_conv.weight_updates, nullptr);
  EXPECT_NE(train_conv.x, nullptr);
  EXPECT_NE(train_conv.weights_ema, nullptr);
  EXPECT_NE(train->get()->layers[3].indexes, nullptr);

  EXPECT_EQ(infer_conv.delta, nullptr);
  EXPECT_EQ(infer_conv.weight_updates, nullptr);
  EXPECT_EQ(infer_conv.x, nullptr);
  EXPECT_EQ(infer_conv.weights_ema, nullptr);
  EXPECT_EQ(infer->get()->layers[3].indexes, nullptr);

  // both keep the weights and the batch normalization loaded from the files
  for (const layer* l : {&train_conv, &infer_conv}) {
    EXPECT_NE(l->weights, nullptr);
    EXPECT_NE(l->biases, nullptr);
    EXPECT_NE(l->scales, nullptr);
    EXPECT_NE(l->rolling_mean, nullptr);
    EXPECT_NE(l->rolling_variance, nullptr);
  }
}

TEST_F(CTEST_OrigDefaultLoader, dropout_and_empty_layers_pass_the_previous_one)
{
  const ANetworkAdaptorPtr net =
      parse(std::string{NET_HEADER} +
            "[convolutional]\nfilters=4\nsize=3\npad=1\nactivation=leaky\n"
            "[dropout]\nprobability=0.3\n"
            "[empty]\n"
            "[shortcut]\nfrom=-3\n");

  const network& view = *net->get();

  // the original aliases, no own buffers
  EXPECT_EQ(view.layers[1].output, view.layers[0].output);
  EXPECT_EQ(view.layers[1].delta, view.layers[0].delta);
  EXPECT_EQ(view.layers[2].output, view.layers[1].output);
  EXPECT_EQ(view.layers[2].delta, view.layers[1].delta);
  EXPECT_TRUE(net->layer_adaptor(1)->output().empty());
  EXPECT_TRUE(net->layer_adaptor(2)->output().empty());

  // the shortcut borrows the output of the layer it takes
  ASSERT_NE(view.layers[3].layers_output, nullptr);
  EXPECT_EQ(view.layers[3].layers_output[0], view.layers[0].output);
  EXPECT_EQ(view.layers[3].layers_delta[0], view.layers[0].delta);
}

TEST_F(CTEST_OrigDefaultLoader, shared_conv_layers_borrow_the_weights)
{
  const ANetworkAdaptorPtr net =
      parse(std::string{NET_HEADER} +
            "[convolutional]\nfilters=4\nsize=3\npad=1\nactivation=leaky\n"
            "batch_normalize=1\n"
            "[convolutional]\nfilters=4\nsize=3\npad=1\nactivation=leaky\n"
            "batch_normalize=1\n"
            "[convolutional]\nfilters=4\nsize=3\npad=1\nactivation=leaky\n"
            "batch_normalize=1\nshare_index=-1\n");

  const network& view = *net->get();
  const layer& shared = view.layers[2];

  EXPECT_EQ(shared.share_layer, &view.layers[1]);
  EXPECT_EQ(shared.weights, view.layers[1].weights);
  EXPECT_EQ(shared.biases, view.layers[1].biases);
  EXPECT_EQ(shared.scales, view.layers[1].scales);
  EXPECT_EQ(shared.rolling_mean, view.layers[1].rolling_mean);
  EXPECT_EQ(shared.rolling_variance, view.layers[1].rolling_variance);
  // the own output of the layer
  EXPECT_NE(shared.output, view.layers[1].output);
  EXPECT_TRUE(net->layer_adaptor(2)->weights().empty());
}

TEST_F(CTEST_OrigDefaultLoader, recurrent_layers_keep_their_sub_layers)
{
  const ANetworkAdaptorPtr net = parse(
      "[net]\nbatch=4\nsubdivisions=1\nwidth=8\nheight=8\nchannels=3\n"
      "time_steps=2\n"
      "[crnn]\nhidden=4\noutput=3\nsize=3\npad=1\nactivation=leaky\n"
      "[conv_lstm]\noutput=4\nsize=3\npad=1\n"
      "[conv_lstm]\noutput=4\nsize=3\npad=1\npeephole=1\nbottleneck=1\n");

  const network& view = *net->get();
  const layer& crnn = view.layers[0];

  ASSERT_NE(crnn.input_layer, nullptr);
  ASSERT_NE(crnn.self_layer, nullptr);
  ASSERT_NE(crnn.output_layer, nullptr);
  // the output and the delta of the layer are the output sub-layer ones
  EXPECT_EQ(crnn.output, crnn.output_layer->output);
  EXPECT_EQ(crnn.delta, crnn.output_layer->delta);
  EXPECT_EQ(crnn.input_layer->type, CONVOLUTIONAL);

  // the peephole less layer keeps the empty vf, vi and vo holders, the
  // bottleneck one the empty wi, wg and wo ones, as the original did
  const layer& plain = view.layers[1];
  ASSERT_NE(plain.vf, nullptr);
  EXPECT_EQ(plain.vf->weights, nullptr);
  EXPECT_NE(plain.wi->weights, nullptr);

  const layer& bottleneck = view.layers[2];
  ASSERT_NE(bottleneck.wi, nullptr);
  EXPECT_EQ(bottleneck.wi->weights, nullptr);
  EXPECT_NE(bottleneck.vf->weights, nullptr);
}

/*
 * The cfg errors, which ended the process in the original darknet.
 */

TEST_F(CTEST_OrigDefaultLoader, cfg_errors_throw)
{
  const std::vector<std::pair<std::string, std::string>> errors{
      {"no sections", ""},
      {"no net section",
       "[convolutional]\nfilters=4\nsize=3\nactivation=leaky\n"},
      {"option before the net section", "batch=1\n[net]\nwidth=8\n"},
      {"unknown section",
       std::string{NET_HEADER} + "[unknown_layer]\nfoo=bar\n"},
      {"no input", "[net]\nbatch=1\n[convolutional]\nfilters=4\nsize=3\n"},
      {"zero subdivisions",
       "[net]\nbatch=4\nsubdivisions=0\nwidth=8\nheight=8\nchannels=3\n"
       "[maxpool]\nsize=2\nstride=2\n"},
      {"conv of the flat input",
       "[net]\nbatch=1\ninputs=10\n[convolutional]\nfilters=4\nsize=3\n"},
      {"route to a later layer",
       std::string{NET_HEADER} +
           "[convolutional]\nfilters=4\nsize=3\npad=1\n[route]\nlayers=5\n"},
      {"route to the layer itself",
       std::string{NET_HEADER} +
           "[convolutional]\nfilters=4\nsize=3\npad=1\n[route]\nlayers=0,1\n"},
      {"route without layers", std::string{NET_HEADER} + "[route]\n"},
      {"shortcut without from",
       std::string{NET_HEADER} +
           "[convolutional]\nfilters=4\nsize=3\npad=1\n[shortcut]\n"},
      {"shortcut of the first layer",
       std::string{NET_HEADER} + "[shortcut]\nfrom=-1\n"},
      {"dropout of the first layer", std::string{NET_HEADER} + "[dropout]\n"},
      {"empty of the first layer", std::string{NET_HEADER} + "[empty]\n"},
      {"share of a later layer",
       std::string{NET_HEADER} +
           "[convolutional]\nfilters=4\nsize=3\npad=1\nshare_index=3\n"},
      {"share of a different layer",
       std::string{NET_HEADER} +
           "[convolutional]\nfilters=4\nsize=3\npad=1\n"
           "[convolutional]\nfilters=8\nsize=3\npad=1\nshare_index=-1\n"},
      {"contrastive without yolo",
       "[net]\nbatch=4\nsubdivisions=1\nwidth=8\nheight=8\nchannels=3\n"
       "[convolutional]\nfilters=4\nsize=3\npad=1\n"
       "[contrastive]\nclasses=2\n"},
      {"yolo of the wrong filters",
       std::string{NET_HEADER} +
           "[convolutional]\nfilters=4\nsize=1\n"
           "[yolo]\nmask=0\nanchors=1,2\nclasses=2\nnum=1\n"},
      {"shortcut weights type",
       std::string{NET_HEADER} +
           "[convolutional]\nfilters=4\nsize=3\npad=1\n"
           "[shortcut]\nfrom=-1\nweights_type=per_pixel\n"},
      {"counters per class",
       std::string{NET_HEADER} +
           "[convolutional]\nfilters=21\nsize=1\n"
           "[yolo]\nmask=0,1,2\nanchors=1,2, 3,4, 5,6\nclasses=2\nnum=3\n"
           "counters_per_class=1,2,3\n"},
      {"missing map file",
       std::string{NET_HEADER} +
           "[convolutional]\nfilters=21\nsize=1\n"
           "[yolo]\nmask=0,1,2\nanchors=1,2, 3,4, 5,6\nclasses=2\nnum=3\n"
           "map=/no/such/map.txt\n"},
      {"missing tree file",
       std::string{NET_HEADER} + "[softmax]\ntree=/no/such/tree.txt\n"},
  };

  for (const auto& [what, content] : errors) {
    SCOPED_TRACE(what);

    EXPECT_THROW(parse(content), NetworkError);
  }

  EXPECT_THROW(loader.parse_network_cfg(workspace.file("missing.cfg")),
               NetworkError);
}

TEST_F(CTEST_OrigDefaultLoader, networks_without_2d_layers_parse)
{
  // the original divided by the zero count of the w > 1 and h > 1 layers
  // (SIGFPE) for these networks
  for (const std::string& content :
       {std::string{"[net]\nbatch=2\nsubdivisions=1\nwidth=1\nheight=1\n"
                    "channels=3\n[connected]\noutput=4\n[softmax]\n"},
        std::string{"[net]\nbatch=2\nsubdivisions=1\ninputs=10\n"
                    "[connected]\noutput=4\nactivation=relu\n"
                    "[connected]\noutput=2\n"},
        std::string{"[net]\nbatch=4\nsubdivisions=1\nwidth=4\nheight=4\n"
                    "channels=1\ntime_steps=2\n[rnn]\nhidden=3\noutput=2\n"}}) {
    SCOPED_TRACE(content);

    ANetworkAdaptorPtr net;

    ASSERT_NO_THROW(net = parse(content));
    EXPECT_TRUE(layers_bound(net));
  }
}

TEST_F(CTEST_OrigDefaultLoader, cfg_reader_semantics)
{
  // the whitespaces anywhere are removed, the comments skipped, the first
  // option of a key wins, the value less option is skipped
  const ANetworkAdaptorPtr net = parse(
      "# the comment\n"
      "; another one\n"
      "[ net ]\n"
      "batch = 8\n"
      "subdivisions=4\n"
      "subdivisions=8\n"
      "width=\t16\n"
      "height=16\r\n"
      "channels=3\n"
      "learning_rate=\n"
      "\n"
      "[convolutional]\n"
      "filters=4 # the inline comment is kept by the atoi() of it\n"
      "size=3\n"
      "pad=1\n"
      "activation=leaky\n");

  const network& view = *net->get();

  EXPECT_EQ(view.w, 16);
  EXPECT_EQ(view.h, 16);
  EXPECT_EQ(view.subdivisions, 4);
  EXPECT_EQ(view.batch, 2);
  // the rejected empty value leaves the default
  EXPECT_FLOAT_EQ(view.learning_rate, 0.001f);
  EXPECT_EQ(view.layers[0].n, 4);
}

TEST_F(CTEST_OrigDefaultLoader, net_options_defaults)
{
  const ANetworkAdaptorPtr net = parse(
      "[net]\nwidth=8\nheight=8\nchannels=3\n"
      "[maxpool]\nsize=2\nstride=2\n");

  const network& view = *net->get();

  EXPECT_EQ(view.batch, 1);
  EXPECT_EQ(view.subdivisions, 1);
  EXPECT_EQ(view.max_batches, 0);
  EXPECT_FLOAT_EQ(view.momentum, 0.9f);
  EXPECT_FLOAT_EQ(view.decay, 0.0001f);
  EXPECT_EQ(view.policy, CONSTANT);
  // the original darknet weights saving intervals, the chart one of the
  // --save-charts-every-iter flag
  EXPECT_EQ(view.saveweights, 1000);
  EXPECT_EQ(view.savelast, 100);
  EXPECT_EQ(view.savechart, 0);
  EXPECT_STREQ(view.names, "data/names.list");
  EXPECT_STREQ(view.train_images, "data/train.txt");
  EXPECT_STREQ(view.valid_images, "data/train.txt");
  EXPECT_STREQ(view.backup_directory, "backup");
}

TEST_F(CTEST_OrigDefaultLoader, saving_intervals)
{
  const ANetworkAdaptorPtr net = parse(
      "[net]\nwidth=8\nheight=8\nchannels=3\n"
      "saveweights=50\nsavelast=10\nsavechart=5\n"
      "[maxpool]\nsize=2\nstride=2\n");

  const network& view = *net->get();

  EXPECT_EQ(view.saveweights, 50);
  EXPECT_EQ(view.savelast, 10);
  EXPECT_EQ(view.savechart, 5);
}

TEST_F(CTEST_OrigDefaultLoader, steps_policy_arrays)
{
  const ANetworkAdaptorPtr net = parse(
      "[net]\nwidth=8\nheight=8\nchannels=3\nmax_batches=20000\n"
      "policy=steps\nsteps=100,200,300 # the comment\nscales=.1,.5\n"
      "seq_scales=2,3,4\n"
      "[maxpool]\nsize=2\nstride=2\n");

  const network& view = *net->get();

  ASSERT_EQ(view.num_steps, 3);
  EXPECT_EQ(view.steps[0], 100);
  EXPECT_EQ(view.steps[2], 300);
  EXPECT_FLOAT_EQ(view.scales[0], 0.1f);
  EXPECT_FLOAT_EQ(view.scales[1], 0.5f);
  // the original read past the end of the shorter scales list
  EXPECT_FLOAT_EQ(view.scales[2], 1.0f);
  EXPECT_FLOAT_EQ(view.seq_scales[2], 4.0f);
  EXPECT_EQ(view.saveweights, 10000);
}

TEST_F(CTEST_OrigDefaultLoader, softmax_tree_and_yolo_map_files)
{
  const std::string tree_file = workspace.write(
      "tree.txt", "root -1\nanimal 0\nplant 0\ncat 1\ndog 1\nrose 2\n");
  const std::string map_file = workspace.write("map.txt", "3\n1\n2\n");

  const ANetworkAdaptorPtr net = parse(
      std::string{NET_HEADER} + "[convolutional]\nfilters=21\nsize=1\n" +
      "[yolo]\nmask=0,1,2\nanchors=1,2, 3,4, 5,6\nclasses=2\nnum=3\nmap=" +
      map_file +
      "\n"
      "[softmax]\ntree=" +
      tree_file + "\n");

  const network& view = *net->get();
  const layer& yolo = view.layers[1];
  const layer& softmax = view.layers[2];

  ASSERT_NE(yolo.map, nullptr);
  EXPECT_EQ(yolo.map[0], 3);
  EXPECT_EQ(yolo.map[2], 2);

  ASSERT_NE(softmax.softmax_tree, nullptr);
  EXPECT_EQ(view.hierarchy, softmax.softmax_tree);

  const tree& t = *softmax.softmax_tree;
  EXPECT_EQ(t.n, 6);
  EXPECT_STREQ(t.name[3], "cat");
  EXPECT_EQ(t.parent[4], 1);
  EXPECT_EQ(t.leaf[0], 0);
  EXPECT_EQ(t.leaf[5], 1);
  EXPECT_EQ(t.groups, 4);
}

/*
 * The generated networks of the weights factory.
 */

class CTEST_OrigDefaultLoader_generated
    : public TestWithParam<weights_factory::WeightsVariant>
{
 public:
  OrigDefaultLoader loader;
  Workspace workspace;
};

TEST_P(CTEST_OrigDefaultLoader_generated, parses_the_network_of_the_spec)
{
  const WeightsVariant& variant = GetParam();
  const std::string cfg = workspace.write("net.cfg", variant.spec.cfg());

  const ANetworkAdaptorPtr net =
      variant.mode == ParseMode::Train
          ? loader.parse_network_cfg(cfg)
          : loader.parse_network_cfg_custom(cfg, 1, 1);

  ASSERT_TRUE(matches_spec(*net->get(), variant.spec, variant.mode));
  EXPECT_TRUE(layers_bound(net));

  // every weights array of the file layout is there, of the layout size
  const Layout layout = variant.spec.layout();
  const network& view = *net->get();

  for (const Block& block : layout) {
    if (block.sub != SubLayer::None) continue;

    const ALayerAdaptorPtr adaptor = net->layer_adaptor(block.layer);
    ASSERT_NE(adaptor, nullptr);

    const layer& l = view.layers[block.layer];
    const float* values = nullptr;
    const float* shared = nullptr;
    size_t own = 0U;

    switch (block.array) {
      case ArrayKind::Biases:
        values = l.biases;
        shared = l.share_layer ? l.share_layer->biases : nullptr;
        own = adaptor->biases().size();
        break;
      case ArrayKind::Weights:
        values = l.weights;
        shared = l.share_layer ? l.share_layer->weights : nullptr;
        own = adaptor->weights().size();
        break;
      case ArrayKind::Scales:
        values = l.scales;
        shared = l.share_layer ? l.share_layer->scales : nullptr;
        own = adaptor->scales().size();
        break;
      case ArrayKind::RollingMean:
        values = l.rolling_mean;
        shared = l.share_layer ? l.share_layer->rolling_mean : nullptr;
        own = adaptor->rolling_mean().size();
        break;
      case ArrayKind::RollingVariance:
        values = l.rolling_variance;
        shared = l.share_layer ? l.share_layer->rolling_variance : nullptr;
        own = adaptor->rolling_variance().size();
        break;
      default:
        continue;
    }

    ASSERT_NE(values, nullptr) << describe(block);

    if (l.share_layer != nullptr) {
      // the layer computes with the arrays of the layer it shares
      EXPECT_EQ(values, shared) << describe(block);
      EXPECT_EQ(own, 0U) << describe(block);
    } else {
      // the own array of the file layout size: the batchnorm layer biases of
      // the channels count, the conv ones of the filters count and the like
      EXPECT_EQ(own, block.count) << describe(block);
    }
  }
}

std::string variant_name(const TestParamInfo<WeightsVariant>& info)
{
  return info.param.name;
}

INSTANTIATE_TEST_SUITE_P(Layers, CTEST_OrigDefaultLoader_generated,
                         ValuesIn(weights_factory::variants::layers()),
                         variant_name);

INSTANTIATE_TEST_SUITE_P(Random, CTEST_OrigDefaultLoader_generated,
                         ValuesIn(weights_factory::variants::random(
                             weights_factory::variants::DEFAULT_RANDOM_SEED,
                             weights_factory::variants::DEFAULT_RANDOM_COUNT)),
                         variant_name);

/*
 * The networks of the context work.
 */

class CTEST_OrigDefaultLoader_context : public CTEST_OrigDefaultLoader
{
 public:
  std::shared_ptr<darknetxx011::LibraryContext> actx =
      std::make_shared<darknetxx011::LibraryContext>();
  std::shared_ptr<DarknetContext> dctx;

  void SetUp() override
  {
    dctx = std::make_shared<DarknetContext>(actx);
    ASSERT_TRUE(dctx->init());
  }

  void TearDown() override
  {
    if (dctx) dctx->release_networks();
  }
};

TEST_F(CTEST_OrigDefaultLoader_context, loads_the_training_networks)
{
  dctx->set_Train_cfg(cfg1);
  dctx->set_Latest_weights(w1);

  ASSERT_TRUE(loader.load(dctx));

  auto odctx = dctx->get_darknet_ctx();

  ASSERT_EQ(dctx->get_train_networks().size(), 1U);
  EXPECT_EQ(odctx->ngpus, 1);
  ASSERT_NE(odctx->nets, nullptr);

  const network& view = *dctx->get_train_networks().front()->get();

  // the context views share the network buffers
  EXPECT_EQ(odctx->nets[0].layers, view.layers);
  EXPECT_EQ(odctx->net.layers, view.layers);
  EXPECT_EQ(odctx->net.seen, view.seen);
  EXPECT_GT(*view.seen, 0U);

  // the darknet C API finds the network of the views
  EXPECT_EQ(NetworksRegistry::find(odctx->net),
            dctx->get_train_networks().front());

  // no mAP without the --map
  EXPECT_EQ(dctx->get_map_network(), nullptr);
  EXPECT_EQ(odctx->net_map.layers, nullptr);

  dctx->release_networks();

  EXPECT_EQ(odctx->nets, nullptr);
  EXPECT_EQ(odctx->net.layers, nullptr);
  EXPECT_TRUE(dctx->get_train_networks().empty());
}

TEST_F(CTEST_OrigDefaultLoader_context, clear_drops_the_training_iteration)
{
  dctx->set_Train_cfg(cfg1);
  dctx->set_Latest_weights(w1);
  dctx->set_Clear(true);

  ASSERT_TRUE(loader.load(dctx));

  const network& view = *dctx->get_train_networks().front()->get();

  EXPECT_EQ(*view.seen, 0U);
  EXPECT_EQ(*view.cur_iteration, 0);
}

TEST_F(CTEST_OrigDefaultLoader_context,
       training_goes_on_from_the_record_of_the_weights)
{
  using darknet_adaptor::loaders::NetworkDescription;
  using darknet_adaptor::loaders::WeightsDocument;

  const ANetworkAdaptorPtr parsed = loader.parse_network_cfg_custom(cfg1, 1, 1);
  const std::string path = workspace.file("net_last.dxxwjz2");
  WeightsDocument document;
  std::string text;

  ASSERT_TRUE(darknet_adaptor::loaders::OrigWeightsReader{}.read(
      w1, parsed->get().get(), document));
  ASSERT_TRUE(darknet_adaptor::loaders::NetworkFiles::read_text(cfg1, text));

  // the weights of the 5th iteration (of the 64 images) of the record of 8
  document.header.seen = 5U * 64U;
  document.network = NetworkDescription{text, std::vector<std::string>(11U)};

  for (int iteration = 1; iteration <= 8; ++iteration) {
    document.training.record(iteration, 2.f, 2.f);
  }

  document.training.record_map(2, 0.5f);
  document.training.record_map(7, 0.6f);

  ASSERT_TRUE(darknet_adaptor::loaders::dxxwjz2::v2::Dxxwjz2Writer{}.write(
      document, path));

  dctx->set_Train_cfg(path);
  dctx->set_Latest_weights(path);

  ASSERT_TRUE(loader.load(dctx));

  auto odctx = dctx->get_darknet_ctx();
  const auto& record = dctx->get_training_journal()->rest().training;

  EXPECT_EQ(record.iterations(), (std::vector<int>{1, 2, 3, 4, 5}));
  EXPECT_EQ(record.map_iterations(), (std::vector<int>{2}));
  EXPECT_EQ(odctx->resumed.points, 5);
  EXPECT_EQ(odctx->resumed.map_points, 1);
  EXPECT_EQ(odctx->names_size, 11);
  EXPECT_STREQ(odctx->base_name, "net");
}

TEST_F(CTEST_OrigDefaultLoader_context,
       map_network_computes_with_the_training_layers)
{
  const std::string names =
      workspace.write("x.names", "a\nb\nc\nd\ne\nf\ng\nh\ni\nj\nk\n");

  dctx->set_Train_cfg(cfg1);
  dctx->set_Map(true);
  dctx->set_Names(names);

  ASSERT_TRUE(loader.load(dctx));

  auto odctx = dctx->get_darknet_ctx();
  const network& train = *dctx->get_train_networks().front()->get();

  ASSERT_NE(dctx->get_map_network(), nullptr);
  const network& map = *dctx->get_map_network()->get();

  EXPECT_EQ(odctx->net_map.layers, map.layers);
  EXPECT_EQ(map.batch, 1);
  ASSERT_EQ(map.n, train.n);

  for (int index = 0; index < map.n; ++index) {
    SCOPED_TRACE("the layer " + std::to_string(index));

    // the views of the training layers of the single image batch
    EXPECT_EQ(map.layers[index].output, train.layers[index].output);
    EXPECT_EQ(map.layers[index].weights, train.layers[index].weights);
    EXPECT_EQ(map.layers[index].batch, 1);
    EXPECT_EQ(map.layers[index].train, 0);
    EXPECT_EQ(map.layers[index].type, train.layers[index].type);
  }

  // the own layers are released, the command line names file taken
  EXPECT_EQ(dctx->get_map_network()->layer_adaptor(0), nullptr);
  EXPECT_STREQ(map.names, names.c_str());
}

TEST_F(CTEST_OrigDefaultLoader_context, loads_the_fused_detection_network)
{
  dctx->set_Detect_cfg(cfg1);
  dctx->set_Latest_weights(w1);

  ASSERT_TRUE(loader.load(dctx));

  auto odctx = dctx->get_darknet_ctx();

  ASSERT_NE(dctx->get_detect_network(), nullptr);
  const network& view = *dctx->get_detect_network()->get();

  EXPECT_EQ(odctx->net.layers, view.layers);
  EXPECT_EQ(view.batch, 1);

  for (int index = 0; index < view.n; ++index) {
    if (view.layers[index].type == CONVOLUTIONAL) {
      // the batch normalization is fused into the weights and released
      EXPECT_EQ(view.layers[index].batch_normalize, 0);
      EXPECT_EQ(view.layers[index].rolling_mean, nullptr);
    }
  }
}

TEST_F(CTEST_OrigDefaultLoader_context, load_failures)
{
  // neither the train nor the detect cfg
  EXPECT_FALSE(loader.load(dctx));
  EXPECT_FALSE(loader.load(nullptr));

  dctx->set_Train_cfg(workspace.file("missing.cfg"));
  EXPECT_FALSE(loader.load(dctx));
  EXPECT_TRUE(dctx->get_train_networks().empty());

  dctx->set_Train_cfg(cfg1);
  dctx->set_Latest_weights(workspace.file("missing.weights"));
  EXPECT_FALSE(loader.load(dctx));

  // the mAP calculation of the names of another count than the classes
  dctx->set_Latest_weights("");
  dctx->set_Map(true);
  dctx->set_Names(workspace.write("two.names", "a\nb\n"));
  EXPECT_FALSE(loader.load(dctx));
  dctx->set_Map(false);
  dctx->set_Latest_weights(workspace.file("missing.weights"));

  // the detection takes precedence, still without the weights
  dctx->set_Detect_cfg(cfg1);
  EXPECT_FALSE(loader.load(dctx));
  EXPECT_EQ(dctx->get_detect_network(), nullptr);
}
