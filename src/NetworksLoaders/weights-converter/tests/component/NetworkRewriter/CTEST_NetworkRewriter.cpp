#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

#include "DarknetNetwork.h"
#include "WeightsFile.h"
#include "WeightsWorkspace.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pReader.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pWriter.h"
#include "src/NetworksLoaders/original-weights/OrigWeightsWriter.h"
#include "src/NetworksLoaders/weights-converter/NetworkRewriter.h"
#include "src/NetworksLoaders/weights-converter/WeightsConverter.h"
#include "src/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/log/default-logger/DefaultLogger.h"

using namespace testing;
using namespace weights_factory;

using darknet_adaptor::loaders::NetworkDescription;
using darknet_adaptor::loaders::NetworkRewriter;
using darknet_adaptor::loaders::OrigWeightsWriter;
using darknet_adaptor::loaders::WeightsConversion;
using darknet_adaptor::loaders::WeightsConverter;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::WeightsFileFormats;
using darknet_adaptor::loaders::dxxwjz3p::v1::Dxxwjz3pReader;
using darknet_adaptor::loaders::dxxwjz3p::v1::Dxxwjz3pWriter;

namespace
{

/// @brief Keeps the info messages of the darknet parser out of the output.
class QuietLogEnvironment : public Environment
{
 public:
  void SetUp() override
  {
    default_logger::DefaultLogger::level(
        default_logger::DefaultLogger::LVL_WARNING);
  }
};

[[maybe_unused]] Environment* const quiet_log_environment =
    AddGlobalTestEnvironment(new QuietLogEnvironment);

/// @brief The convolutional layers 0 and 2 and the connected one 3 keep the
/// weights, the cfg text the way the dxxwjz3 files keep it.
const std::string CFG =
    "[net]\nbatch=1\nwidth=8\nheight=8\nchannels=2\nlearning_rate=0.001\n\n"
    "[convolutional]\nbatch_normalize=1\nfilters=4\nsize=3\npad=1\n"
    "activation=leaky\n\n"
    "[maxpool]\nsize=2\nstride=2\n\n"
    "[convolutional]\nfilters=6\nsize=3\npad=1\nactivation=leaky\n\n"
    "[connected]\noutput=3\nactivation=linear\n";

std::string replaced(const std::string& text, const std::string& what,
                     const std::string& with)
{
  return std::regex_replace(text, std::regex{what}, with,
                            std::regex_constants::format_first_only);
}

}  // namespace

class CTEST_NetworkRewriter : public Test
{
 public:
  void SetUp() override
  {
    const std::string cfg = workspace.file("net.cfg");
    const std::string weights = workspace.file("net.weights");
    const std::string names = workspace.file("net.names");

    std::ofstream{cfg} << CFG;
    std::ofstream{names} << "cat\ndog\nfox\n";

    DarknetNetwork darknet{cfg, ParseMode::Train};

    ASSERT_TRUE(OrigWeightsWriter{}.save(darknet.net(), weights));

    WeightsConversion conversion;

    conversion.from = WeightsFileFormats::named("weights");
    conversion.src = weights;
    conversion.to = WeightsFileFormats::named("dxxwjz3p");
    conversion.dst = project;
    conversion.cfg = cfg;
    conversion.names = names;

    ASSERT_TRUE(WeightsConverter{}.convert(conversion));

    // the project of the trained weights
    ASSERT_TRUE(Dxxwjz3pReader{}.read(project, nullptr, source));

    source.header.seen = 640U;
    source.training.record(1, 2.5F, 2.5F);
    source.trailing = {1, 2};

    ASSERT_TRUE(Dxxwjz3pWriter{}.write(source, project));
  }

  /// @brief Rewrites the network of the cfg into the edited project, reads
  /// it back.
  bool rewrite(const std::string& cfg, WeightsDocument& edited)
  {
    message = "?";

    return NetworkRewriter::rewrite(
               project, dst, NetworkDescription{cfg, {"a", "b"}}, message) &&
           Dxxwjz3pReader{}.read(dst, nullptr, edited);
  }

  /// @brief The indexes of the document layers of the arrays.
  static std::vector<int> indexes(const WeightsDocument& document)
  {
    std::vector<int> result;

    for (const auto& one : document.layers) {
      if (!one.arrays.empty()) {
        result.push_back(one.index);
      }
    }

    return result;
  }

  /// @brief Tells the arrays of the layer are the source ones.
  bool same_arrays(const WeightsDocument& edited, int index) const
  {
    const auto* rewritten = edited.find(index);
    const auto* original = source.find(index);

    if (rewritten == nullptr || original == nullptr ||
        rewritten->arrays.size() != original->arrays.size()) {
      return false;
    }

    for (std::size_t array = 0U; array < rewritten->arrays.size(); ++array) {
      if (rewritten->arrays[array].values != original->arrays[array].values) {
        return false;
      }
    }

    return true;
  }

  WeightsWorkspace workspace{CTEST_NetworkRewriter_DATA_DIR};
  const std::string project{workspace.file("project.dxxwjz3p")};
  const std::string dst{workspace.file("edited.dxxwjz3p")};
  WeightsDocument source;
  std::string message;
};

TEST_F(CTEST_NetworkRewriter, the_net_options_keep_every_weight)
{
  const std::string cfg =
      replaced(CFG, "learning_rate=0.001", "learning_rate=0.01\nmomentum=0.9");
  WeightsDocument edited;

  ASSERT_TRUE(rewrite(cfg, edited)) << message;

  EXPECT_EQ(message, "");
  EXPECT_EQ(edited.network.cfg(), cfg);
  EXPECT_EQ(edited.network.classes(), (std::vector<std::string>{"a", "b"}));
  EXPECT_EQ(indexes(edited), (std::vector<int>{0, 2, 3}));
  EXPECT_TRUE(same_arrays(edited, 0));
  EXPECT_TRUE(same_arrays(edited, 3));
  EXPECT_EQ(edited.header, source.header);
  EXPECT_EQ(edited.training.size(), 1U);
  EXPECT_EQ(edited.trailing, source.trailing);
}

TEST_F(CTEST_NetworkRewriter, the_changed_layer_drops_it_and_the_later_ones)
{
  WeightsDocument edited;

  ASSERT_TRUE(rewrite(replaced(CFG, "filters=6", "filters=8"), edited));

  EXPECT_EQ(message,
            "The weights of the layer 2 and of the ones after it were dropped, "
            "the network changed them");
  EXPECT_EQ(indexes(edited), (std::vector<int>{0}));
  EXPECT_TRUE(same_arrays(edited, 0));

  // the network to train of the weights kept
  EXPECT_EQ(edited.header.seen, 0U);
  EXPECT_EQ(edited.training.size(), 0U);
  EXPECT_TRUE(edited.trailing.empty());
}

TEST_F(CTEST_NetworkRewriter, the_inserted_layer_drops_the_weights_it_moved)
{
  // the activation layer before the connected one, the sizes stay the same
  WeightsDocument edited;

  ASSERT_TRUE(
      rewrite(replaced(CFG, "\\[connected\\]",
                       "[activation]\nactivation=linear\n\n[connected]"),
              edited));

  EXPECT_EQ(indexes(edited), (std::vector<int>{0, 2}));
}

TEST_F(CTEST_NetworkRewriter, the_input_size_drops_the_weights_it_sizes)
{
  WeightsDocument edited;

  ASSERT_TRUE(rewrite(replaced(CFG, "width=8", "width=16"), edited));

  // the convolutional weights don't depend on the width
  EXPECT_EQ(indexes(edited), (std::vector<int>{0, 2}));
  EXPECT_TRUE(same_arrays(edited, 2));
}

TEST_F(CTEST_NetworkRewriter, the_network_of_no_darknet_build_is_refused)
{
  EXPECT_FALSE(NetworkRewriter::rewrite(
      project, dst, NetworkDescription{"[convolutional]\nfilters=1\n", {}},
      message));
  EXPECT_NE(message.find("First section must be [net]"), std::string::npos)
      << message;

  EXPECT_FALSE(NetworkRewriter::rewrite(
      project, dst, NetworkDescription{CFG + "\n[route]\nlayers=-1,7\n", {}},
      message));
  EXPECT_FALSE(std::filesystem::exists(dst));
}

TEST_F(CTEST_NetworkRewriter, the_route_of_the_different_sizes_is_refused)
{
  EXPECT_FALSE(NetworkRewriter::rewrite(
      project, dst, NetworkDescription{CFG + "\n[route]\nlayers=-2,0\n", {}},
      message));
  EXPECT_EQ(message,
            "The layer 4 routes the layers of the different sizes: 4 x 4 and 8 "
            "x 8");
  EXPECT_FALSE(std::filesystem::exists(dst));
}

TEST_F(CTEST_NetworkRewriter, no_source_is_the_network_to_train)
{
  WeightsDocument edited;

  ASSERT_TRUE(NetworkRewriter::rewrite("", dst, NetworkDescription{CFG, {"x"}},
                                       message));
  ASSERT_TRUE(Dxxwjz3pReader{}.read(dst, nullptr, edited));

  EXPECT_EQ(message, "");
  EXPECT_TRUE(edited.network_alone());
  EXPECT_EQ(edited.network.cfg(), CFG);
  EXPECT_EQ(edited.network.classes(), std::vector<std::string>{"x"});
}

TEST_F(CTEST_NetworkRewriter, the_project_is_rewritten_in_place)
{
  const std::string cfg = replaced(CFG, "output=3", "output=5");
  WeightsDocument edited;

  ASSERT_TRUE(NetworkRewriter::rewrite(
      project, project, NetworkDescription{cfg, {"a"}}, message));
  ASSERT_TRUE(Dxxwjz3pReader{}.read(project, nullptr, edited));

  EXPECT_EQ(edited.network.cfg(), cfg);
  EXPECT_EQ(indexes(edited), (std::vector<int>{0, 2}));
}

TEST_F(CTEST_NetworkRewriter, the_files_of_no_network_are_refused)
{
  const std::string weights = workspace.file("edited.weights");

  EXPECT_FALSE(NetworkRewriter::rewrite(project, weights,
                                        NetworkDescription{CFG, {}}, message));
  EXPECT_EQ(message, "The weights file " + weights +
                         " keeps no network, the dxxwjz3p project or the "
                         "dxxwjz3 file does");

  EXPECT_FALSE(NetworkRewriter::rewrite(workspace.file("net.weights"), dst,
                                        NetworkDescription{CFG, {}}, message));
  EXPECT_FALSE(std::filesystem::exists(dst));
}
