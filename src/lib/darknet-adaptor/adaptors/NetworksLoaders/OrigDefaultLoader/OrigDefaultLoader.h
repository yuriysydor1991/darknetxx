#ifndef DARKNET_CXX_PORT_PROJECT_ORIG_DEFAULT_LOADER_FOR_NETWORK_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ORIG_DEFAULT_LOADER_FOR_NETWORK_CLASS_H

#include <memory>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/DarknetContext.h"
#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/INetworkLoader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/CfgSection.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkDescription.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The original darknet network loader: the C++ port of the network
 * cfg file parser of the original darknet parser.c (parse_network_cfg(),
 * parse_network_cfg_custom(), load_network() and the like).
 *
 * The loader builds the networks of the C++ adaptors: the layers are made by
 * the layer classes of the Layers directory, the network memory belongs to
 * the ANetworkAdaptor and the darknet C code computes with it's struct view.
 * The networks are made of the cfg files or of the dxxwjz2 and the dxxwjz3 ones
 * (see the NetworkSource), the weights are loaded by the readers of their
 * formats (see the WeightsFileFormats).
 *
 * The parse errors, where the original darknet ended the process with the
 * error() call, throw NetworkError here; the load() of the context catches
 * and logs them instead.
 */
class OrigDefaultLoader : virtual public INetworkLoader
{
 public:
  using OrigDefaultLoaderPtr = std::shared_ptr<OrigDefaultLoader>;
  using ALayerAdaptorPtr = adaptors::ALayerAdaptorPtr;
  using ANetworkAdaptor = adaptors::ANetworkAdaptor;
  using ANetworkAdaptorPtr = adaptors::ANetworkAdaptorPtr;

  virtual ~OrigDefaultLoader() = default;
  OrigDefaultLoader() = default;

  static OrigDefaultLoaderPtr create();

  /**
   * @brief Loads the networks the work of the context needs and hands them
   * to the context (see DarknetContext::set_train_networks() and the like):
   * - the --train-cfg one: a training network per GPU with the given weights
   *   (the --clear dropping their iteration and their training record) and
   *   the mAP calculation network for the --map, which computes with the
   *   layers of the first one, the journal of the training (see the
   *   TrainingJournal);
   * - the --detect-cfg one: the single image batch network with the given
   *   weights, the batch normalization fused into them, and the class names
   *   of the detection.
   *
   * The class names are the ones of the --names file, of the dxxwjz2 weights
   * file, of the dxxwjz2 network file or of the names= file of the cfg, the
   * first of them given.
   *
   * @return False on the failure, which is logged.
   */
  virtual bool load(DarknetContextPtr dctx) override;

  /**
   * @brief The parse_network_cfg_custom(): builds the network of the cfg
   * file. The positive batch builds the inference network of the batch
   * (without the training buffers), the zero one the training network of the
   * cfg batch. The positive time_steps replaces the cfg ones.
   *
   * Throws NetworkError on the failure.
   */
  ANetworkAdaptorPtr parse_network_cfg_custom(const std::string& filename,
                                              int batch, int time_steps);

  /// @brief The parse_network_cfg_custom() of the cfg text of the named
  /// file.
  ANetworkAdaptorPtr parse_network_text(const std::string& cfg,
                                        const std::string& filename, int batch,
                                        int time_steps);

  /// @brief The parse_network_cfg(): the training network of the cfg file.
  ANetworkAdaptorPtr parse_network_cfg(const std::string& filename);

  /**
   * @brief The load_network(): the training network of the cfg file with the
   * weights of the file (the random ones for the empty weights path), the
   * clear dropping the seen images and the iteration.
   *
   * Throws NetworkError on the failure.
   */
  ANetworkAdaptorPtr load_network(const std::string& cfg,
                                  const std::string& weights, bool clear);

  /**
   * @brief The load_network_custom(): the inference network of the given
   * batch with the weights of the file, the batch normalization fused into
   * them.
   *
   * Throws NetworkError on the failure.
   */
  ANetworkAdaptorPtr load_network_custom(const std::string& cfg,
                                         const std::string& weights, bool clear,
                                         int batch);

  /**
   * @brief The single image batch inference network of the network file (the
   * cfg or the dxxwjz2 one) with the weights of the weights file of any
   * format, the batch normalization fused into them, and it's class names
   * (see the load(), the names file is the --names one).
   *
   * Throws NetworkError on the failure.
   */
  ANetworkAdaptorPtr load_inference_network(const std::string& cfg,
                                            const std::string& weights,
                                            const std::string& names,
                                            std::vector<std::string>& classes);

 private:
  /// @brief The state of the network parse passed to the parse_*() methods.
  struct size_params
  {
    int batch{0};
    int inputs{0};
    int h{0};
    int w{0};
    int c{0};
    int index{0};
    int time_steps{0};
    int train{0};
    /// the network of the layers parsed so far
    ANetworkAdaptor* net{nullptr};
  };

  bool load_train_networks(const DarknetContextPtr& dctx);
  bool load_detect_network(const DarknetContextPtr& dctx);

  /**
   * @brief The mAP calculation network of the single image batch: the cfg
   * [net] section options and the layers of the given training network,
   * which it computes with.
   */
  ANetworkAdaptorPtr make_map_network(const DarknetContextPtr& dctx,
                                      const NetworkDescription& description,
                                      const ANetworkAdaptorPtr& train);

  /// @brief The description of the network of the cfg or the dxxwjz2 file,
  /// throws NetworkError on the failure.
  static NetworkDescription read_description(const std::string& path);

  /// @brief Everything the network file keeps but the weights (see the
  /// NetworkSource::read()), throws NetworkError on the failure.
  static WeightsDocument read_source(const std::string& path);

  /// @brief Loads the weights file of any format into the network, keeps the
  /// rest of the file. Throws NetworkError on the failure.
  static void load_weights(network& net, const std::string& weights,
                           WeightsDocument& rest);

  /// @brief Throws NetworkError for the dxxwjz2 file of no weights, the
  /// network to train.
  static void require_weights(const std::string& weights);

  /// @brief The class names of the work (see the load()), the names file is
  /// the --names one.
  static std::vector<std::string> class_names(
      const std::string& file, const WeightsDocument& rest,
      const NetworkDescription& description, const network& net);

  void parse_net_options(CfgSection& options, ANetworkAdaptor& net);
  static learning_rate_policy get_policy(const std::string& s);
  static LAYER_TYPE string_to_layer_type(const std::string& type);
  static void set_train_only_bn(network& net);

  /// @brief The layer of the index of the network parsed so far, throws
  /// NetworkError for the one not parsed yet.
  static const layer& parsed_layer(const size_params& params, int index);

  ALayerAdaptorPtr parse_local(CfgSection& options, const size_params& params);
  ALayerAdaptorPtr parse_convolutional(CfgSection& options,
                                       const size_params& params);
  ALayerAdaptorPtr parse_crnn(CfgSection& options, const size_params& params);
  ALayerAdaptorPtr parse_rnn(CfgSection& options, const size_params& params);
  ALayerAdaptorPtr parse_gru(CfgSection& options, const size_params& params);
  ALayerAdaptorPtr parse_lstm(CfgSection& options, const size_params& params);
  ALayerAdaptorPtr parse_conv_lstm(CfgSection& options,
                                   const size_params& params);
  ALayerAdaptorPtr parse_history(CfgSection& options,
                                 const size_params& params);
  ALayerAdaptorPtr parse_connected(CfgSection& options,
                                   const size_params& params);
  ALayerAdaptorPtr parse_softmax(CfgSection& options,
                                 const size_params& params);
  ALayerAdaptorPtr parse_contrastive(CfgSection& options,
                                     const size_params& params);
  ALayerAdaptorPtr parse_yolo(CfgSection& options, const size_params& params);
  ALayerAdaptorPtr parse_gaussian_yolo(CfgSection& options,
                                       const size_params& params);
  ALayerAdaptorPtr parse_region(CfgSection& options, const size_params& params);
  ALayerAdaptorPtr parse_detection(CfgSection& options,
                                   const size_params& params);
  ALayerAdaptorPtr parse_cost(CfgSection& options, const size_params& params);
  ALayerAdaptorPtr parse_crop(CfgSection& options, const size_params& params);
  ALayerAdaptorPtr parse_reorg(CfgSection& options, const size_params& params);
  ALayerAdaptorPtr parse_reorg_old(CfgSection& options,
                                   const size_params& params);
  ALayerAdaptorPtr parse_local_avgpool(CfgSection& options,
                                       const size_params& params);
  ALayerAdaptorPtr parse_maxpool(CfgSection& options,
                                 const size_params& params);
  ALayerAdaptorPtr parse_avgpool(CfgSection& options,
                                 const size_params& params);
  ALayerAdaptorPtr parse_dropout(CfgSection& options,
                                 const size_params& params);
  ALayerAdaptorPtr parse_normalization(CfgSection& options,
                                       const size_params& params);
  ALayerAdaptorPtr parse_batchnorm(CfgSection& options,
                                   const size_params& params);
  ALayerAdaptorPtr parse_shortcut(CfgSection& options,
                                  const size_params& params);
  ALayerAdaptorPtr parse_scale_channels(CfgSection& options,
                                        const size_params& params);
  ALayerAdaptorPtr parse_sam(CfgSection& options, const size_params& params);
  ALayerAdaptorPtr parse_implicit(CfgSection& options,
                                  const size_params& params);
  ALayerAdaptorPtr parse_activation(CfgSection& options,
                                    const size_params& params);
  ALayerAdaptorPtr parse_upsample(CfgSection& options,
                                  const size_params& params);
  ALayerAdaptorPtr parse_route(CfgSection& options, const size_params& params);
  ALayerAdaptorPtr parse_empty(const size_params& params);

  /**
   * @brief The comma separated integers of the value up to the first '#',
   * as parse_yolo_mask() read them, the empty vector for the NULL one.
   */
  static std::vector<int> parse_int_list(const char* a);

  /// @brief The comma separated floats of the value, at most count of them.
  static std::vector<float> parse_float_list(const char* a, bool stop_at_hash);

  static std::vector<float> get_classes_multipliers(const char* cpc,
                                                    int classes,
                                                    float max_delta);

  /// @brief The read_map() of the class ids map file, one per line.
  static std::vector<int> read_map(const std::string& filename);

  /// @brief The read_tree() of the hierarchical softmax tree file.
  static std::shared_ptr<tree> read_tree(const std::string& filename);
};

using OrigDefaultLoaderPtr = OrigDefaultLoader::OrigDefaultLoaderPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_ORIG_DEFAULT_LOADER_FOR_NETWORK_CLASS_H
