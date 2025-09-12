#ifndef DARKNET_CXX_PORT_PROJECT_ORIG_DEFAULT_LOADER_FOR_NETWORK_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ORIG_DEFAULT_LOADER_FOR_NETWORK_CLASS_H

#include <memory>

#include "src/darknet-adaptor/DarknetContext.h"
#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/INetworkLoader.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The original darknet code for network conf and data loader.
 */
class OrigDefaultLoader : virtual public INetworkLoader
{
 public:
  using OrigDefaultLoaderPtr = std::shared_ptr<OrigDefaultLoader>;

  virtual ~OrigDefaultLoader() = default;
  OrigDefaultLoader() = default;

  virtual bool load(DarknetContextPtr dctx) override;

 private:
  typedef struct
  {
    std::string type;
    list *options;
  } section;

  typedef struct size_params
  {
    int batch;
    int inputs;
    int h;
    int w;
    int c;
    int index;
    int time_steps;
    int train;
    network net;
  } size_params;

  network *load_network(const std::string &cfg, const std::string &weights,
                        int clear);
  network *load_network_custom(const std::string &cfg,
                               const std::string &weights, int clear,
                               int batch);
  void load_weights(network *net, const std::string &filename);
  void load_weights_upto(network *net, const std::string &filename, int cutoff);
  void load_implicit_weights(layer l, FILE *fp);
  void load_shortcut_weights(layer l, FILE *fp);
  void load_convolutional_weights(layer l, FILE *fp);
  void load_convolutional_weights_binary(layer l, FILE *fp);
  void load_batchnorm_weights(layer l, FILE *fp);
  void load_connected_weights(layer l, FILE *fp, int transpose);
  void transpose_matrix(float *a, int rows, int cols);
  void save_weights(network net, const std::string &filename);
  void save_weights_upto(network net, const std::string &filename, int cutoff,
                         int save_ema);
  void save_connected_weights(layer l, FILE *fp);
  void save_batchnorm_weights(layer l, FILE *fp);
  void save_convolutional_weights_ema(layer l, FILE *fp);
  void save_convolutional_weights(layer l, FILE *fp);
  void save_implicit_weights(layer l, FILE *fp);
  void save_shortcut_weights(layer l, FILE *fp);
  void save_convolutional_weights_binary(layer l, FILE *fp);
  list *read_cfg(const std::string &filename);
  network parse_network_cfg_custom(const std::string &filename, int batch,
                                   int time_steps);
  network parse_network_cfg(const std::string &filename);
  void set_train_only_bn(network net);
  int is_network(section *s);
  void parse_net_options(list *options, network *net);
  learning_rate_policy get_policy(const std::string &s);
  route_layer parse_route(list *options, size_params params);
  layer parse_upsample(list *options, size_params params, network net);
  layer parse_activation(list *options, size_params params);
  layer parse_implicit(list *options, size_params params, network net);
  layer parse_sam(list *options, size_params params, network net);
  layer parse_scale_channels(list *options, size_params params, network net);
  layer parse_shortcut(list *options, size_params params, network net);
  layer parse_batchnorm(list *options, size_params params);
  layer parse_normalization(list *options, size_params params);
  dropout_layer parse_dropout(list *options, size_params params);
  avgpool_layer parse_avgpool(list *options, size_params params);
  maxpool_layer parse_maxpool(list *options, size_params params);
  maxpool_layer parse_local_avgpool(list *options, size_params params);
  layer parse_reorg_old(list *options, size_params params);
  layer parse_reorg(list *options, size_params params);
  crop_layer parse_crop(list *options, size_params params);
  cost_layer parse_cost(list *options, size_params params);
  detection_layer parse_detection(list *options, size_params params);
  layer parse_region(list *options, size_params params);
  layer parse_gaussian_yolo(list *options, size_params params);
  std::vector<int> parse_gaussian_yolo_mask(const std::string &a);
  layer parse_yolo(list *options, size_params params);
  float *get_classes_multipliers(const std::string &cpc, const int classes,
                                 const float max_delta);
  std::vector<int> parse_yolo_mask(const std::string &a);
  contrastive_layer parse_contrastive(list *options, size_params params);
  softmax_layer parse_softmax(list *options, size_params params);
  connected_layer parse_connected(list *options, size_params params);
  layer parse_history(list *options, size_params params);
  layer parse_conv_lstm(list *options, size_params params);
  layer parse_lstm(list *options, size_params params);
  layer parse_gru(list *options, size_params params);
  layer parse_rnn(list *options, size_params params);
  layer parse_crnn(list *options, size_params params);
  convolutional_layer parse_convolutional(list *options, size_params params);
  local_layer parse_local(list *options, size_params params);
  void parse_data(const std::string &data, float *a, int n);
  void free_section(section *s);
  LAYER_TYPE string_to_layer_type(const std::string &type);

  std::vector<int> parse_mask(const std::string &a);
};

using OrigDefaultLoaderPtr = OrigDefaultLoader::OrigDefaultLoaderPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_ORIG_DEFAULT_LOADER_FOR_NETWORK_CLASS_H
