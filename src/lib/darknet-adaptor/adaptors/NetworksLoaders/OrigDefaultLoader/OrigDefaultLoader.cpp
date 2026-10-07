#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "src/lib/darknet-adaptor/DarknetContext.h"
#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ActivationLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/AvgpoolLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/BatchnormLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ConnectedLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ConvLstmLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ConvolutionalLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/CostLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/CrnnLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/CropLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/DetectionLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/DropoutLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/EmptyLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/GaussianYoloLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/GruLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ImplicitLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LocalLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LstmLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/MaxpoolLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/NormalizationLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/RegionLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ReorgLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ReorgOldLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/RnnLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/RouteLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/SamLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ScaleChannelsLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ShortcutLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/SoftmaxLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/UpsampleLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/YoloLayer.h"
#include "src/lib/darknet-adaptor/adaptors/NetworkError.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/CfgSection.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/NetworkSource.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkFiles.h"
#include "src/lib/darknet-adaptor/adaptors/TrainingJournal/TrainingJournal.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"
#include "src/log/log.h"

namespace darknet_adaptor::loaders
{

namespace layers = adaptors::layers;
using adaptors::ANetworkAdaptor;
using adaptors::NetworkError;
using ALayerAdaptorPtr = adaptors::ALayerAdaptorPtr;
using ANetworkAdaptorPtr = adaptors::ANetworkAdaptorPtr;

namespace
{

/// @brief The darknet enum parsers take the non const strings, which they
/// only compare.
char* c_str(const char* value) { return const_cast<char*>(value); }

ACTIVATION activation_of(const char* name)
{
  return get_activation(c_str(name));
}

/// @brief The pieces of the value separated by the commas, up to the first
/// '#' for the stop_at_hash.
std::vector<std::string> split(const char* value, bool stop_at_hash)
{
  std::vector<std::string> pieces;

  if (value == nullptr) {
    return pieces;
  }

  std::string text{value};

  if (stop_at_hash) {
    const size_t hash = text.find('#');

    if (hash != std::string::npos) {
      // the last piece keeps the text after the '#', as the atoi() of the
      // original stops at it anyway
      const size_t commas = static_cast<size_t>(std::count(
          text.begin(), text.begin() + static_cast<std::ptrdiff_t>(hash), ','));
      std::stringstream all{text};
      std::string piece;

      while (pieces.size() <= commas && std::getline(all, piece, ',')) {
        pieces.push_back(piece);
      }

      return pieces;
    }
  }

  std::stringstream all{text};
  std::string piece;

  while (std::getline(all, piece, ',')) {
    pieces.push_back(piece);
  }

  // the trailing comma adds the empty piece, as the original counted it
  if (!text.empty() && text.back() == ',') {
    pieces.emplace_back();
  }

  return pieces;
}

/// @brief The tree the read_tree() made, kept in the C++ containers.
struct TreeStorage
{
  tree view{};
  std::vector<int> leaf;
  std::vector<int> parent;
  std::vector<int> group;
  std::vector<int> group_size;
  std::vector<int> group_offset;
  std::vector<std::string> names;
  std::vector<char*> name_pointers;
};

}  // namespace

OrigDefaultLoaderPtr OrigDefaultLoader::create()
{
  return std::make_shared<OrigDefaultLoader>();
}

bool OrigDefaultLoader::load(DarknetContextPtr dctx)
{
  if (dctx == nullptr) {
    LOGE("Invalid context pointer provided");
    return false;
  }

  try {
    if (!dctx->get_Detect_cfg().empty()) {
      return load_detect_network(dctx);
    }

    if (!dctx->get_Train_cfg().empty()) {
      return load_train_networks(dctx);
    }
  }
  catch (const NetworkError& error) {
    LOGE("Failure to load the network: " << error.what());
    return false;
  }
  catch (const std::bad_alloc& error) {
    LOGE("Failure to allocate the network, possibly out of CPU RAM: "
         << error.what());
    return false;
  }

  LOGE("Neither the train nor the detect cfg file provided");

  return false;
}

bool OrigDefaultLoader::load_train_networks(const DarknetContextPtr& dctx)
{
  auto odctx = dctx->get_darknet_ctx();

  if (odctx->ngpus <= 0) {
    CLOGWM("The ngpus value should contain at least value 1, setting it up");
    odctx->ngpus = 1;
  }

  const std::string& cfg = dctx->get_Train_cfg();
  const std::string& weights = dctx->get_Latest_weights();
  const IWeightsFileFormatPtr format =
      WeightsFileFormats::named(dctx->get_Save_format());

  if (format == nullptr) {
    throw NetworkError("Unknown network weights file format to save: " +
                       dctx->get_Save_format());
  }

  const NetworkDescription description = read_description(cfg);
  WeightsDocument rest;
  std::vector<ANetworkAdaptorPtr> nets;
  // the iteration of the weights the training goes on from
  int iteration = 0;

  // every network of a GPU gets the same random initialization
  srand(static_cast<unsigned int>(time(nullptr)));
  const int seed = rand();

  for (int k = 0; k < odctx->ngpus; ++k) {
    srand(static_cast<unsigned int>(seed));

    ANetworkAdaptorPtr net = parse_network_text(description.cfg(), cfg, 0, 0);
    network& view = *net->get();

    view.benchmark_layers = odctx->benchmark_layers;

    if (!weights.empty()) {
      load_weights(view, weights, rest);
    }

    iteration = *view.cur_iteration;

    if (odctx->clear) {
      *view.seen = 0;
      *view.cur_iteration = 0;
    }

    view.learning_rate *= static_cast<float>(odctx->ngpus);

    nets.push_back(std::move(net));
  }

  ANetworkAdaptorPtr map;

  if (odctx->calc_map) {
    map = make_map_network(dctx, description, nets.front());
  }

  // the training from the beginning has no record to resume, the one of the
  // weights of an earlier iteration none of the iterations after it
  if (odctx->clear) {
    rest.training.clear();
  }

  rest.training.truncate_after(iteration);

  const std::vector<std::string> names =
      class_names(dctx, rest, description, *nets.front()->get());
  const network& net = *nets.front()->get();

  if (odctx->calc_map &&
      names.size() != static_cast<std::size_t>(net.layers[net.n - 1].classes)) {
    throw NetworkError(
        "The mAP calculation requires the class names of every "
        "class of the network: " +
        std::to_string(names.size()) +
        " names for classes=" + std::to_string(net.layers[net.n - 1].classes));
  }

  rest.network = NetworkDescription{description.cfg(), names};

  dctx->set_class_names(names);
  dctx->set_training_base(NetworkSource::base_of(cfg, iteration));

  dctx->set_train_networks(std::move(nets));
  dctx->set_map_network(std::move(map));
  dctx->set_training_journal(
      std::make_shared<adaptors::TrainingJournal>(format, std::move(rest)));

  return true;
}

ANetworkAdaptorPtr OrigDefaultLoader::make_map_network(
    const DarknetContextPtr& dctx, const NetworkDescription& description,
    const ANetworkAdaptorPtr& train)
{
  auto odctx = dctx->get_darknet_ctx();

  CLOGIM(" Prepare additional network for mAP calculation...");

  ANetworkAdaptorPtr map =
      parse_network_text(description.cfg(), dctx->get_Train_cfg(), 1, 1);
  network& view = *map->get();

  view.benchmark_layers = odctx->benchmark_layers;

  // the command line class names file is the one of the mAP calculation too
  if (!dctx->get_Names().empty()) {
    map->names(dctx->get_Names());
  }

  // The mAP calculation computes with the layers of the training network
  // (the copy_weights_net() refreshes them before every calculation), so the
  // own layers of the network are released.
  const network& source = *train->get();

  map->allocate_layers(source.n);
  ANetworkAdaptor::copy_weights_net(source, *map->get());

  map->hierarchy(source.hierarchy);
  map->link_output();

  return map;
}

bool OrigDefaultLoader::load_detect_network(const DarknetContextPtr& dctx)
{
  auto odctx = dctx->get_darknet_ctx();

  const std::string& cfg = dctx->get_Detect_cfg();
  const std::string& weights = dctx->get_Latest_weights();
  const NetworkDescription description = read_description(cfg);

  // set batch=1
  ANetworkAdaptorPtr net = parse_network_text(description.cfg(), cfg, 1, 1);

  require_weights(weights);

  WeightsDocument rest;

  load_weights(*net->get(), weights, rest);

  net->get()->benchmark_layers = odctx->benchmark_layers;

  net->fuse_conv_batchnorm();
  net->calculate_binary_weights();

  dctx->set_class_names(class_names(dctx, rest, description, *net->get()));
  dctx->set_detect_network(std::move(net));

  return true;
}

NetworkDescription OrigDefaultLoader::read_description(const std::string& path)
{
  NetworkDescription description;

  if (!NetworkSource::read(path, description)) {
    throw NetworkError("Couldn't read the network of the file: " + path);
  }

  return description;
}

void OrigDefaultLoader::load_weights(network& net, const std::string& weights,
                                     WeightsDocument& rest)
{
  const IWeightsReaderPtr reader =
      WeightsFileFormats::of_file(weights)->create_reader();

  if (!reader->load(net, weights, net.n, rest)) {
    throw NetworkError("Couldn't load the weights file: " + weights);
  }
}

void OrigDefaultLoader::require_weights(const std::string& weights)
{
  WeightsDocument rest;
  bool held = true;

  if (NetworkSource::describes(weights) &&
      WeightsFileFormats::of_file(weights)->create_reader()->peek(weights, rest,
                                                                  held) &&
      !held) {
    throw NetworkError("The " + weights +
                       " file holds no weights, it's the network to train");
  }
}

std::vector<std::string> OrigDefaultLoader::class_names(
    const DarknetContextPtr& dctx, const WeightsDocument& rest,
    const NetworkDescription& description, const network& net)
{
  std::vector<std::string> names;
  const std::string& file = dctx->get_Names();

  if (!file.empty()) {
    if (!NetworkFiles::read_classes(file, names)) {
      LOGE("Couldn't read the class names file: " << file);
    }

    return names;
  }

  if (!rest.network.classes().empty()) {
    return rest.network.classes();
  }

  if (!description.classes().empty()) {
    return description.classes();
  }

  std::error_code error;

  if (net.names != nullptr &&
      std::filesystem::is_regular_file(net.names, error)) {
    NetworkFiles::read_classes(net.names, names);
  }

  return names;
}

ANetworkAdaptorPtr OrigDefaultLoader::load_network(const std::string& cfg,
                                                   const std::string& weights,
                                                   bool clear)
{
  if (cfg.empty()) {
    CLOGEM("Invalid config file path string provided");
    throw NetworkError("No network cfg file provided");
  }

  // the weights are optional: without them the network gets the random
  // initialization, same as in the original darknet
  CLOGI("Try to load cfg: %s, clear = %d", cfg.c_str(), clear ? 1 : 0);

  ANetworkAdaptorPtr net = parse_network_cfg(cfg);

  if (!weights.empty()) {
    CLOGI(" Try to load weights: %s", weights.c_str());

    WeightsDocument rest;

    load_weights(*net->get(), weights, rest);
  }

  if (clear) {
    *net->get()->seen = 0;
    *net->get()->cur_iteration = 0;
  }

  return net;
}

ANetworkAdaptorPtr OrigDefaultLoader::load_network_custom(
    const std::string& cfg, const std::string& weights, bool clear, int batch)
{
  CLOGI(" Try to load cfg: %s, weights: %s, clear = %d", cfg.c_str(),
        weights.c_str(), clear ? 1 : 0);

  ANetworkAdaptorPtr net = parse_network_cfg_custom(cfg, batch, 1);

  if (!weights.empty()) {
    CLOGI(" Try to load weights: %s", weights.c_str());

    WeightsDocument rest;

    load_weights(*net->get(), weights, rest);
  }

  net->fuse_conv_batchnorm();

  if (clear) {
    *net->get()->seen = 0;
    *net->get()->cur_iteration = 0;
  }

  return net;
}

ANetworkAdaptorPtr OrigDefaultLoader::parse_network_cfg(
    const std::string& filename)
{
  return parse_network_cfg_custom(filename, 0, 0);
}

ANetworkAdaptorPtr OrigDefaultLoader::parse_network_cfg_custom(
    const std::string& filename, int batch, int time_steps)
{
  return parse_network_text(read_description(filename).cfg(), filename, batch,
                            time_steps);
}

ANetworkAdaptorPtr OrigDefaultLoader::parse_network_text(
    const std::string& cfg, const std::string& filename, int batch,
    int time_steps)
{
  std::istringstream text{cfg};
  CfgSections sections = CfgSection::read_cfg(text, filename);

  if (sections.empty()) {
    CLOGEM("Config file has no sections");
    throw NetworkError("Config file has no sections: " + filename);
  }

  ANetworkAdaptorPtr anet = ANetworkAdaptor::create();
  network& net = *anet->get();

  anet->allocate_layers(static_cast<int>(sections.size()) - 1);
  net.gpu_index = gpu_index;

  size_params params;

  if (batch > 0) {
    params.train = 0;  // allocates memory for Inference only
  } else {
    params.train = 1;  // allocates memory for Inference & Training
  }

  CfgSection& net_section = sections.front();

  if (net_section.type() != "[net]" && net_section.type() != "[network]") {
    CLOGEM("First section must be [net] or [network]");
    throw NetworkError("First section must be [net] or [network]");
  }

  parse_net_options(net_section, *anet);

  params.h = net.h;
  params.w = net.w;
  params.c = net.c;
  CLOGI("Net imputs: %d", net.inputs);
  params.inputs = net.inputs;
  if (batch > 0) net.batch = batch;
  if (time_steps > 0) net.time_steps = time_steps;
  if (net.batch < 1) net.batch = 1;
  if (net.time_steps < 1) net.time_steps = 1;
  if (net.batch < net.time_steps) net.batch = net.time_steps;
  params.batch = net.batch;
  params.time_steps = net.time_steps;
  params.net = anet.get();
  CLOGI("mini_batch = %d, batch = %d, time_steps = %d, train = %d", net.batch,
        net.batch * net.subdivisions, net.time_steps, params.train);

  int last_stop_backward = -1;
  int avg_outputs = 0;
  int avg_counter = 0;
  float bflops = 0;
  size_t workspace_size = 0;
  size_t max_inputs = 0;
  size_t max_outputs = 0;
  int receptive_w = 1, receptive_h = 1;
  int receptive_w_scale = 1, receptive_h_scale = 1;
  const int show_receptive_field =
      static_cast<int>(net_section.find_float_quiet("show_receptive_field", 0));

  // find l.stopbackward = option_find_int_quiet(options, "stopbackward", 0);
  if (params.train == 1) {
    for (size_t index = 1U; index < sections.size(); ++index) {
      const int stopbackward =
          sections[index].find_int_quiet("stopbackward", 0);
      if (stopbackward == 1) {
        last_stop_backward = static_cast<int>(index) - 1;
        CLOGI("last_stop_backward = %d", last_stop_backward);
      }
    }
  }

  const int old_params_train = params.train;

  CLOGIM("   layer   filters  size/strd(dil)      input                output");

  for (size_t section = 1U; section < sections.size(); ++section) {
    const int count = static_cast<int>(section) - 1;
    CfgSection& options = sections[section];

    params.train = old_params_train;
    if (count < last_stop_backward) params.train = 0;

    params.index = count;
    CLOGI("%4d ", count);

    ALayerAdaptorPtr adaptor;
    const LAYER_TYPE lt = string_to_layer_type(options.type());

    if (lt == CONVOLUTIONAL) {
      adaptor = parse_convolutional(options, params);
    } else if (lt == LOCAL) {
      adaptor = parse_local(options, params);
    } else if (lt == ACTIVE) {
      adaptor = parse_activation(options, params);
    } else if (lt == RNN) {
      adaptor = parse_rnn(options, params);
    } else if (lt == GRU) {
      adaptor = parse_gru(options, params);
    } else if (lt == LSTM) {
      adaptor = parse_lstm(options, params);
    } else if (lt == CONV_LSTM) {
      adaptor = parse_conv_lstm(options, params);
    } else if (lt == HISTORY) {
      adaptor = parse_history(options, params);
    } else if (lt == CRNN) {
      adaptor = parse_crnn(options, params);
    } else if (lt == CONNECTED) {
      adaptor = parse_connected(options, params);
    } else if (lt == CROP) {
      adaptor = parse_crop(options, params);
    } else if (lt == COST) {
      adaptor = parse_cost(options, params);
      adaptor->get()->keep_delta_gpu = 1;
    } else if (lt == REGION) {
      adaptor = parse_region(options, params);
      adaptor->get()->keep_delta_gpu = 1;
    } else if (lt == YOLO) {
      adaptor = parse_yolo(options, params);
      adaptor->get()->keep_delta_gpu = 1;
    } else if (lt == GAUSSIAN_YOLO) {
      adaptor = parse_gaussian_yolo(options, params);
      adaptor->get()->keep_delta_gpu = 1;
    } else if (lt == DETECTION) {
      adaptor = parse_detection(options, params);
    } else if (lt == SOFTMAX) {
      adaptor = parse_softmax(options, params);
      net.hierarchy = adaptor->get()->softmax_tree;
      adaptor->get()->keep_delta_gpu = 1;
    } else if (lt == CONTRASTIVE) {
      adaptor = parse_contrastive(options, params);
      adaptor->get()->keep_delta_gpu = 1;
    } else if (lt == NORMALIZATION) {
      adaptor = parse_normalization(options, params);
    } else if (lt == BATCHNORM) {
      adaptor = parse_batchnorm(options, params);
    } else if (lt == MAXPOOL) {
      adaptor = parse_maxpool(options, params);
    } else if (lt == LOCAL_AVGPOOL) {
      adaptor = parse_local_avgpool(options, params);
    } else if (lt == REORG) {
      adaptor = parse_reorg(options, params);
    } else if (lt == REORG_OLD) {
      adaptor = parse_reorg_old(options, params);
    } else if (lt == AVGPOOL) {
      adaptor = parse_avgpool(options, params);
    } else if (lt == ROUTE) {
      adaptor = parse_route(options, params);
      const layer& l = *adaptor->get();
      for (int k = 0; k < l.n; ++k) {
        net.layers[l.input_layers[k]].use_bin_output = 0;
        if (count >= last_stop_backward) {
          net.layers[l.input_layers[k]].keep_delta_gpu = 1;
        }
      }
    } else if (lt == UPSAMPLE) {
      adaptor = parse_upsample(options, params);
    } else if (lt == SHORTCUT) {
      adaptor = parse_shortcut(options, params);
      const layer& l = *adaptor->get();
      net.layers[count - 1].use_bin_output = 0;
      net.layers[l.index].use_bin_output = 0;
      if (count >= last_stop_backward) net.layers[l.index].keep_delta_gpu = 1;
    } else if (lt == SCALE_CHANNELS) {
      adaptor = parse_scale_channels(options, params);
      const layer& l = *adaptor->get();
      net.layers[count - 1].use_bin_output = 0;
      net.layers[l.index].use_bin_output = 0;
      net.layers[l.index].keep_delta_gpu = 1;
    } else if (lt == SAM) {
      adaptor = parse_sam(options, params);
      const layer& l = *adaptor->get();
      net.layers[count - 1].use_bin_output = 0;
      net.layers[l.index].use_bin_output = 0;
      net.layers[l.index].keep_delta_gpu = 1;
    } else if (lt == IMPLICIT) {
      adaptor = parse_implicit(options, params);
    } else if (lt == DROPOUT) {
      adaptor = parse_dropout(options, params);
      const layer& previous = parsed_layer(params, count - 1);
      adaptor->borrow_output(previous.output);
      adaptor->borrow_delta(previous.delta);
    } else if (lt == EMPTY) {
      adaptor = parse_empty(params);
    } else {
      // the original kept the zeroed layer of the unknown section, a
      // CONVOLUTIONAL one of no size, which crashed the first computation
      CLOGE("Type not recognized: %s", options.type().c_str());
      throw NetworkError("Type not recognized: " + options.type());
    }

    layer& l = *adaptor->get();

    // calculate receptive field
    if (show_receptive_field) {
      const int dilation = max_val_cmp(1, l.dilation);
      const int stride = max_val_cmp(1, l.stride);
      const int size = max_val_cmp(1, l.size);

      if (l.type == UPSAMPLE || (l.type == REORG)) {
        l.receptive_w = receptive_w;
        l.receptive_h = receptive_h;
        l.receptive_w_scale = receptive_w_scale = receptive_w_scale / stride;
        l.receptive_h_scale = receptive_h_scale = receptive_h_scale / stride;
      } else {
        if (l.type == ROUTE) {
          receptive_w = receptive_h = receptive_w_scale = receptive_h_scale = 0;
          for (int k = 0; k < l.n; ++k) {
            const layer& route_l = net.layers[l.input_layers[k]];
            receptive_w = max_val_cmp(receptive_w, route_l.receptive_w);
            receptive_h = max_val_cmp(receptive_h, route_l.receptive_h);
            receptive_w_scale =
                max_val_cmp(receptive_w_scale, route_l.receptive_w_scale);
            receptive_h_scale =
                max_val_cmp(receptive_h_scale, route_l.receptive_h_scale);
          }
        } else {
          int increase_receptive = size + (dilation - 1) * 2 - 1;  // stride;
          increase_receptive = max_val_cmp(0, increase_receptive);

          receptive_w += increase_receptive * receptive_w_scale;
          receptive_h += increase_receptive * receptive_h_scale;
          receptive_w_scale *= stride;
          receptive_h_scale *= stride;
        }

        l.receptive_w = receptive_w;
        l.receptive_h = receptive_h;
        l.receptive_w_scale = receptive_w_scale;
        l.receptive_h_scale = receptive_h_scale;
      }

      CLOGI("%4d - receptive field: %d x %d", count, receptive_w, receptive_h);
    }

    l.clip = options.find_float_quiet("clip", 0);
    l.dynamic_minibatch = net.dynamic_minibatch;
    l.onlyforward = options.find_int_quiet("onlyforward", 0);
    l.dont_update = options.find_int_quiet("dont_update", 0);
    l.burnin_update = options.find_int_quiet("burnin_update", 0);
    l.stopbackward = options.find_int_quiet("stopbackward", 0);
    l.train_only_bn = options.find_int_quiet("train_only_bn", 0);
    l.dontload = options.find_int_quiet("dontload", 0);
    l.dontloadscales = options.find_int_quiet("dontloadscales", 0);
    l.learning_rate_scale = options.find_float_quiet("learning_rate", 1);
    options.unused();

    if (l.stopbackward == 1) {
      CLOGIM(" ------- previous layers are frozen -------");
    }

    // the network keeps the layer in it's slot, the struct darknet indexes
    anet->bind_layer(count, adaptor);

    const layer& bound = net.layers[count];

    if (bound.workspace_size > workspace_size) {
      workspace_size = bound.workspace_size;
    }
    if (static_cast<size_t>(bound.inputs) > max_inputs) {
      max_inputs = static_cast<size_t>(bound.inputs);
    }
    if (static_cast<size_t>(bound.outputs) > max_outputs) {
      max_outputs = static_cast<size_t>(bound.outputs);
    }

    if (section + 1U < sections.size()) {
      if (bound.antialiasing) {
        params.h = bound.input_layer->out_h;
        params.w = bound.input_layer->out_w;
        params.c = bound.input_layer->out_c;
        params.inputs = bound.input_layer->outputs;
      } else {
        params.h = bound.out_h;
        params.w = bound.out_w;
        params.c = bound.out_c;
        params.inputs = bound.outputs;
      }
    }

    if (bound.bflops > 0) bflops += bound.bflops;

    if (bound.w > 1 && bound.h > 1) {
      avg_outputs += bound.outputs;
      avg_counter++;
    }
  }

  if (last_stop_backward > -1) {
    for (int k = 0; k < last_stop_backward; ++k) {
      const layer& l = net.layers[k];

      if (l.keep_delta_gpu && !l.delta) {
        anet->layer_adaptor(k)->delta(layers::LayersBuffers::floats(
            layers::LayersBuffers::count(l.outputs, l.batch)));
      }

      net.layers[k].onlyforward = 1;
      net.layers[k].train = 0;
    }
  }

  set_train_only_bn(net);  // set l.train_only_bn for all required layers

  anet->link_output();

  // the original divided by the zero counter of the network without a layer
  // of the w > 1 and h > 1 input
  if (avg_counter > 0) {
    avg_outputs = avg_outputs / avg_counter;
  }

  CLOGI("Total BFLOPS %5.3f", static_cast<double>(bflops));
  CLOGI("avg_outputs = %d", avg_outputs);

  if (workspace_size) {
    anet->workspace_bytes(workspace_size);
  }

  if (net.n > 0) {
    const LAYER_TYPE lt = net.layers[net.n - 1].type;

    if ((net.w % 32 != 0 || net.h % 32 != 0) &&
        (lt == YOLO || lt == REGION || lt == DETECTION)) {
      CLOGW(
          " Warning: width=%d and height=%d in cfg-file must be divisible by "
          "32 for default networks Yolo v1/v2/v3!!!",
          net.w, net.h);
    }
  }

  return anet;
}

learning_rate_policy OrigDefaultLoader::get_policy(const std::string& s)
{
  if (s == "random") return RANDOM;
  if (s == "poly") return POLY;
  if (s == "constant") return CONSTANT;
  if (s == "step") return STEP;
  if (s == "exp") return EXP;
  if (s == "sigmoid") return SIG;
  if (s == "steps") return STEPS;
  if (s == "sgdr") return SGDR;
  CLOGW("Couldn't find policy %s, going with constant", s.c_str());
  return CONSTANT;
}

void OrigDefaultLoader::parse_net_options(CfgSection& options,
                                          ANetworkAdaptor& anet)
{
  network& net = *anet.get();

  net.max_batches = options.find_int("max_batches", 0);
  net.batch = options.find_int("batch", 1);
  net.learning_rate = options.find_float("learning_rate", .001f);
  net.learning_rate_min =
      options.find_float_quiet("learning_rate_min", .00001f);
  net.batches_per_cycle = options.find_int_quiet("sgdr_cycle", net.max_batches);
  net.batches_cycle_mult = options.find_int_quiet("sgdr_mult", 2);
  net.momentum = options.find_float("momentum", .9f);
  net.decay = options.find_float("decay", .0001f);
  const int subdivs = options.find_int("subdivisions", 1);
  net.time_steps = options.find_int_quiet("time_steps", 1);
  net.track = options.find_int_quiet("track", 0);
  net.augment_speed = options.find_int_quiet("augment_speed", 2);
  net.init_sequential_subdivisions = net.sequential_subdivisions =
      options.find_int_quiet("sequential_subdivisions", subdivs);
  if (net.sequential_subdivisions > subdivs) {
    net.init_sequential_subdivisions = net.sequential_subdivisions = subdivs;
  }
  net.try_fix_nan = options.find_int_quiet("try_fix_nan", 0);

  if (subdivs == 0) {
    throw NetworkError("The subdivisions of the [net] section can't be 0");
  }

  net.batch /= subdivs;  // mini_batch
  const int mini_batch = net.batch;
  net.batch *= net.time_steps;  // mini_batch * time_steps
  net.subdivisions = subdivs;   // number of mini_batches

  net.weights_reject_freq = options.find_int_quiet("weights_reject_freq", 0);
  net.equidistant_point = options.find_int_quiet("equidistant_point", 0);
  net.badlabels_rejection_percentage =
      options.find_float_quiet("badlabels_rejection_percentage", 0);
  net.num_sigmas_reject_badlabels =
      options.find_float_quiet("num_sigmas_reject_badlabels", 0);
  net.ema_alpha = options.find_float_quiet("ema_alpha", 0);
  *net.badlabels_reject_threshold = 0;
  *net.delta_rolling_max = 0;
  *net.delta_rolling_avg = 0;
  *net.delta_rolling_std = 0;
  *net.seen = 0;
  *net.cur_iteration = 0;
  *net.cuda_graph_ready = 0;
  net.use_cuda_graph = options.find_int_quiet("use_cuda_graph", 0);
  net.loss_scale = options.find_float_quiet("loss_scale", 1);
  net.dynamic_minibatch = options.find_int_quiet("dynamic_minibatch", 0);
  net.optimized_memory = options.find_int_quiet("optimized_memory", 0);
  // 1024 MB by default
  net.workspace_size_limit = static_cast<size_t>(
      static_cast<float>(1024 * 1024) *
      options.find_float_quiet("workspace_size_limit_MB", 1024));

  net.adam = options.find_int_quiet("adam", 0);
  if (net.adam) {
    net.B1 = options.find_float("B1", .9f);
    net.B2 = options.find_float("B2", .999f);
    net.eps = options.find_float("eps", .000001f);
  }

  net.h = options.find_int_quiet("height", 0);
  net.w = options.find_int_quiet("width", 0);
  net.c = options.find_int_quiet("channels", 0);
  net.inputs = options.find_int_quiet("inputs", net.h * net.w * net.c);
  net.max_crop = options.find_int_quiet("max_crop", net.w * 2);
  net.min_crop = options.find_int_quiet("min_crop", net.w);
  net.flip = options.find_int_quiet("flip", 1);
  net.blur = options.find_int_quiet("blur", 0);
  net.gaussian_noise = options.find_int_quiet("gaussian_noise", 0);
  net.mixup = options.find_int_quiet("mixup", 0);
  const int cutmix = options.find_int_quiet("cutmix", 0);
  const int mosaic = options.find_int_quiet("mosaic", 0);
  if (mosaic && cutmix) {
    net.mixup = 4;
  } else if (cutmix) {
    net.mixup = 2;
  } else if (mosaic) {
    net.mixup = 3;
  }
  net.letter_box = options.find_int_quiet("letter_box", 0);
  net.mosaic_bound = options.find_int_quiet("mosaic_bound", 0);
  net.contrastive = options.find_int_quiet("contrastive", 0);
  net.contrastive_jit_flip = options.find_int_quiet("contrastive_jit_flip", 0);
  net.contrastive_color = options.find_int_quiet("contrastive_color", 0);
  net.unsupervised = options.find_int_quiet("unsupervised", 0);
  if (net.contrastive && mini_batch < 2) {
    throw NetworkError(
        "Error: mini_batch size (batch/subdivisions) should be higher than 1 "
        "for Contrastive loss!");
  }
  net.label_smooth_eps = options.find_float_quiet("label_smooth_eps", 0.0f);
  net.resize_step =
      static_cast<int>(options.find_float_quiet("resize_step", 32));
  net.attention = options.find_int_quiet("attention", 0);
  net.adversarial_lr = options.find_float_quiet("adversarial_lr", 0);
  net.max_chart_loss = options.find_float_quiet("max_chart_loss", 20.0);

  net.angle = options.find_float_quiet("angle", 0);
  net.aspect = options.find_float_quiet("aspect", 1);
  net.saturation = options.find_float_quiet("saturation", 1);
  net.exposure = options.find_float_quiet("exposure", 1);
  net.hue = options.find_float_quiet("hue", 0);
  net.power = options.find_float_quiet("power", 4);

  if (!net.inputs && !(net.h && net.w && net.c)) {
    throw NetworkError("No input parameters supplied");
  }

  const char* policy_s = options.find_str("policy", "constant");
  net.policy = get_policy(policy_s);
  net.burn_in = options.find_int_quiet("burn_in", 0);

  if (net.policy == STEP) {
    net.step = options.find_int("step", 1);
    net.scale = options.find_float("scale", 1);
  } else if (net.policy == STEPS || net.policy == SGDR) {
    const char* l = options.find("steps");
    const char* p = options.find("scales");
    const char* s = options.find("seq_scales");
    if (net.policy == STEPS && (!l || !p)) {
      throw NetworkError("STEPS policy must have steps and scales in cfg file");
    }

    if (l) {
      const std::vector<std::string> step_values = split(l, true);
      const std::vector<std::string> scale_values = split(p, false);
      const std::vector<std::string> seq_values = split(s, false);
      const size_t n = step_values.size();

      std::vector<int> steps(n);
      std::vector<float> scales(n);
      std::vector<float> seq_scales(n);

      for (size_t i = 0; i < n; ++i) {
        // the missing values are the 1.0, where the original read past the
        // end of the option
        float scale = 1.0;
        if (p && i < scale_values.size()) {
          scale = static_cast<float>(std::atof(scale_values[i].c_str()));
        }
        float sequence_scale = 1.0;
        if (s && i < seq_values.size()) {
          sequence_scale = static_cast<float>(std::atof(seq_values[i].c_str()));
        }
        steps[i] = std::atoi(step_values[i].c_str());
        scales[i] = scale;
        seq_scales[i] = sequence_scale;
      }

      anet.scales(scales);
      anet.steps(steps);
      anet.seq_scales(seq_scales);
      net.num_steps = static_cast<int>(n);
    }
  } else if (net.policy == EXP) {
    net.gamma = options.find_float("gamma", 1);
  } else if (net.policy == SIG) {
    net.gamma = options.find_float("gamma", 1);
    net.step = options.find_int("step", 1);
  }

  anet.names(options.find_str("names", "data/names.list"));
  anet.train_images(options.find_str("train", "data/train.txt"));
  anet.valid_images(options.find_str("valid", net.train_images));
  anet.backup_directory(options.find_str("backup", "backup"));

  CLOGI("images names file: %s", net.names);
  CLOGI("train images file: %s", net.train_images);
  CLOGI("validate images file: %s", net.valid_images);
  CLOGI("backup directory: %s", net.backup_directory);

  // configure when to write weights (the original Darknet defaults).
  // Very useful for smaller datasets!
  net.saveweights =
      options.find_int("saveweights", (net.max_batches < 10000) ? 1000 : 10000);
  net.savelast = options.find_int("savelast", 100);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_local(CfgSection& options,
                                                const size_params& params)
{
  const int n = options.find_int("filters", 1);
  const int size = options.find_int("size", 1);
  const int stride = options.find_int("stride", 1);
  const int pad = options.find_int("pad", 0);
  const char* activation_s = options.find_str("activation", "logistic");
  const ACTIVATION activation = activation_of(activation_s);

  const int h = params.h;
  const int w = params.w;
  const int c = params.c;
  const int batch = params.batch;
  if (!(h && w && c)) {
    throw NetworkError("Layer before local layer must output image.");
  }

  return layers::LocalLayer::make(batch, h, w, c, n, size, stride, pad,
                                  activation);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_convolutional(
    CfgSection& options, const size_params& params)
{
  const int n = options.find_int("filters", 1);
  const int groups = options.find_int_quiet("groups", 1);
  const int size = options.find_int("size", 1);
  int stride = -1;
  int stride_x = options.find_int_quiet("stride_x", -1);
  int stride_y = options.find_int_quiet("stride_y", -1);
  if (stride_x < 1 || stride_y < 1) {
    stride = options.find_int("stride", 1);
    if (stride_x < 1) stride_x = stride;
    if (stride_y < 1) stride_y = stride;
  } else {
    stride = options.find_int_quiet("stride", 1);
  }
  int dilation = options.find_int_quiet("dilation", 1);
  const int antialiasing = options.find_int_quiet("antialiasing", 0);
  if (size == 1) dilation = 1;
  const int pad = options.find_int_quiet("pad", 0);
  int padding = options.find_int_quiet("padding", 0);
  if (pad) padding = size / 2;

  const char* activation_s = options.find_str("activation", "logistic");
  const ACTIVATION activation = activation_of(activation_s);

  const int assisted_excitation =
      static_cast<int>(options.find_float_quiet("assisted_excitation", 0));

  const int share_index = options.find_int_quiet("share_index", -1000000000);
  ALayerAdaptorPtr share_layer;
  if (share_index >= 0) {
    parsed_layer(params, share_index);
    share_layer = params.net->layer_adaptor(share_index);
  } else if (share_index != -1000000000) {
    parsed_layer(params, params.index + share_index);
    share_layer = params.net->layer_adaptor(params.index + share_index);
  }

  const int h = params.h;
  const int w = params.w;
  const int c = params.c;
  const int batch = params.batch;
  if (!(h && w && c)) {
    throw NetworkError("Layer before convolutional layer must output image.");
  }
  int batch_normalize = options.find_int_quiet("batch_normalize", 0);
  const int cbn = options.find_int_quiet("cbn", 0);
  if (cbn) batch_normalize = 2;
  const int binary = options.find_int_quiet("binary", 0);
  const int xnor = options.find_int_quiet("xnor", 0);
  const int use_bin_output = options.find_int_quiet("bin_output", 0);
  const int sway = options.find_int_quiet("sway", 0);
  const int rotate = options.find_int_quiet("rotate", 0);
  const int stretch = options.find_int_quiet("stretch", 0);
  const int stretch_sway = options.find_int_quiet("stretch_sway", 0);
  if ((sway + rotate + stretch + stretch_sway) > 1) {
    throw NetworkError(
        "Error: should be used only 1 param: sway=1, rotate=1 or stretch=1 in "
        "the [convolutional] layer");
  }
  const int deform = sway || rotate || stretch || stretch_sway;
  if (deform && size == 1) {
    throw NetworkError(
        "Error: params (sway=1, rotate=1 or stretch=1) should be used only "
        "with size >=3 in the [convolutional] layer");
  }

  const network& net = *params.net->get();

  ALayerAdaptorPtr adaptor = layers::ConvolutionalLayer::make(
      batch, 1, h, w, c, n, groups, size, stride_x, stride_y, dilation, padding,
      activation, batch_normalize, binary, xnor, net.adam, use_bin_output,
      params.index, antialiasing, share_layer, assisted_excitation, deform,
      params.train);
  layer& l = *adaptor->get();

  l.flipped = options.find_int_quiet("flipped", 0);
  l.dot = options.find_float_quiet("dot", 0);
  l.sway = sway;
  l.rotate = rotate;
  l.stretch = stretch;
  l.stretch_sway = stretch_sway;
  l.angle = options.find_float_quiet("angle", 15);
  l.grad_centr = options.find_int_quiet("grad_centr", 0);
  l.reverse = options.find_float_quiet("reverse", 0);
  l.coordconv = options.find_int_quiet("coordconv", 0);

  l.stream = options.find_int_quiet("stream", -1);
  l.wait_stream_id = options.find_int_quiet("wait_stream", -1);

  if (net.adam) {
    l.B1 = net.B1;
    l.B2 = net.B2;
    l.eps = net.eps;
  }

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_crnn(CfgSection& options,
                                               const size_params& params)
{
  const int size = options.find_int_quiet("size", 3);
  const int stride = options.find_int_quiet("stride", 1);
  const int dilation = options.find_int_quiet("dilation", 1);
  const int pad = options.find_int_quiet("pad", 0);
  int padding = options.find_int_quiet("padding", 0);
  if (pad) padding = size / 2;

  const int output_filters = options.find_int("output", 1);
  const int hidden_filters = options.find_int("hidden", 1);
  const int groups = options.find_int_quiet("groups", 1);
  const char* activation_s = options.find_str("activation", "logistic");
  const ACTIVATION activation = activation_of(activation_s);
  const int batch_normalize = options.find_int_quiet("batch_normalize", 0);
  const int xnor = options.find_int_quiet("xnor", 0);

  ALayerAdaptorPtr adaptor = layers::CrnnLayer::make(
      params.batch, params.h, params.w, params.c, hidden_filters,
      output_filters, groups, params.time_steps, size, stride, dilation,
      padding, activation, batch_normalize, xnor, params.train);

  adaptor->get()->shortcut = options.find_int_quiet("shortcut", 0);

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_rnn(CfgSection& options,
                                              const size_params& params)
{
  const int output = options.find_int("output", 1);
  const int hidden = options.find_int("hidden", 1);
  const char* activation_s = options.find_str("activation", "logistic");
  const ACTIVATION activation = activation_of(activation_s);
  const int batch_normalize = options.find_int_quiet("batch_normalize", 0);
  const int logistic = options.find_int_quiet("logistic", 0);

  ALayerAdaptorPtr adaptor = layers::RnnLayer::make(
      params.batch, params.inputs, hidden, output, params.time_steps,
      activation, batch_normalize, logistic);

  adaptor->get()->shortcut = options.find_int_quiet("shortcut", 0);

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_gru(CfgSection& options,
                                              const size_params& params)
{
  const int output = options.find_int("output", 1);
  const int batch_normalize = options.find_int_quiet("batch_normalize", 0);

  return layers::GruLayer::make(params.batch, params.inputs, output,
                                params.time_steps, batch_normalize);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_lstm(CfgSection& options,
                                               const size_params& params)
{
  const int output = options.find_int("output", 1);
  const int batch_normalize = options.find_int_quiet("batch_normalize", 0);

  return layers::LstmLayer::make(params.batch, params.inputs, output,
                                 params.time_steps, batch_normalize);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_conv_lstm(CfgSection& options,
                                                    const size_params& params)
{
  // a ConvLSTM with a larger transitional kernel should be able to capture
  // faster motions
  const int size = options.find_int_quiet("size", 3);
  const int stride = options.find_int_quiet("stride", 1);
  const int dilation = options.find_int_quiet("dilation", 1);
  const int pad = options.find_int_quiet("pad", 0);
  int padding = options.find_int_quiet("padding", 0);
  if (pad) padding = size / 2;

  const int output_filters = options.find_int("output", 1);
  const int groups = options.find_int_quiet("groups", 1);
  const char* activation_s = options.find_str("activation", "linear");
  const ACTIVATION activation = activation_of(activation_s);
  const int batch_normalize = options.find_int_quiet("batch_normalize", 0);
  const int xnor = options.find_int_quiet("xnor", 0);
  const int peephole = options.find_int_quiet("peephole", 0);
  const int bottleneck = options.find_int_quiet("bottleneck", 0);

  ALayerAdaptorPtr adaptor = layers::ConvLstmLayer::make(
      params.batch, params.h, params.w, params.c, output_filters, groups,
      params.time_steps, size, stride, dilation, padding, activation,
      batch_normalize, peephole, xnor, bottleneck, params.train);
  layer& l = *adaptor->get();

  l.state_constrain =
      options.find_int_quiet("state_constrain", params.time_steps * 32);
  l.shortcut = options.find_int_quiet("shortcut", 0);

  const char* lstm_activation_s = options.find_str("lstm_activation", "tanh");
  l.lstm_activation = activation_of(lstm_activation_s);
  l.time_normalizer = options.find_float_quiet("time_normalizer", 1.0);

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_history(CfgSection& options,
                                                  const size_params& params)
{
  const int history_size = options.find_int("history_size", 4);

  return layers::ConvLstmLayer::make_history(params.batch, params.h, params.w,
                                             params.c, history_size,
                                             params.time_steps, params.train);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_connected(CfgSection& options,
                                                    const size_params& params)
{
  const int output = options.find_int("output", 1);
  const char* activation_s = options.find_str("activation", "logistic");
  const ACTIVATION activation = activation_of(activation_s);
  const int batch_normalize = options.find_int_quiet("batch_normalize", 0);

  return layers::ConnectedLayer::make(params.batch, 1, params.inputs, output,
                                      activation, batch_normalize);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_softmax(CfgSection& options,
                                                  const size_params& params)
{
  const int groups = options.find_int_quiet("groups", 1);

  if (groups == 0) {
    throw NetworkError("The groups of the [softmax] layer can't be 0");
  }

  ALayerAdaptorPtr adaptor =
      layers::SoftmaxLayer::make(params.batch, params.inputs, groups);
  layer& l = *adaptor->get();

  l.temperature = options.find_float_quiet("temperature", 1);
  const char* tree_file = options.find_str("tree", nullptr);
  if (tree_file) adaptor->own_softmax_tree(read_tree(tree_file));
  l.w = params.w;
  l.h = params.h;
  l.c = params.c;
  l.spatial = static_cast<int>(options.find_float_quiet("spatial", 0));
  l.noloss = options.find_int_quiet("noloss", 0);

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_contrastive(CfgSection& options,
                                                      const size_params& params)
{
  const int classes = options.find_int("classes", 1000);
  layer* yolo_layer = nullptr;
  int yolo_layer_id = options.find_int_quiet("yolo_layer", 0);
  if (yolo_layer_id < 0) yolo_layer_id = params.index + yolo_layer_id;
  if (yolo_layer_id != 0) {
    parsed_layer(params, yolo_layer_id);
    yolo_layer = params.net->get()->layers + yolo_layer_id;
  }
  // the original dereferenced the missing yolo_layer= one as well
  if (yolo_layer == nullptr || yolo_layer->type != YOLO) {
    CLOGE(
        " Error: [contrastive] layer should point to the [yolo] layer instead "
        "of %d layer!",
        yolo_layer_id);
    throw NetworkError(
        "The [contrastive] layer should point to the [yolo] layer");
  }

  ALayerAdaptorPtr adaptor = layers::SoftmaxLayer::make_contrastive(
      params.batch, params.w, params.h, params.c, classes, params.inputs,
      yolo_layer);
  layer& l = *adaptor->get();

  l.temperature = options.find_float_quiet("temperature", 1);
  l.steps = params.time_steps;
  l.cls_normalizer = options.find_float_quiet("cls_normalizer", 1);
  l.max_delta = options.find_float_quiet("max_delta", FLT_MAX);  // set 10
  l.contrastive_neg_max = options.find_int_quiet("contrastive_neg_max", 3);

  return adaptor;
}

std::vector<int> OrigDefaultLoader::parse_int_list(const char* a)
{
  const std::vector<std::string> pieces = split(a, true);
  std::vector<int> values(pieces.size());

  for (size_t i = 0; i < pieces.size(); ++i) {
    values[i] = std::atoi(pieces[i].c_str());
  }

  return values;
}

std::vector<float> OrigDefaultLoader::parse_float_list(const char* a,
                                                       bool stop_at_hash)
{
  const std::vector<std::string> pieces = split(a, stop_at_hash);
  std::vector<float> values(pieces.size());

  for (size_t i = 0; i < pieces.size(); ++i) {
    values[i] = static_cast<float>(std::atof(pieces[i].c_str()));
  }

  return values;
}

std::vector<float> OrigDefaultLoader::get_classes_multipliers(const char* cpc,
                                                              int classes,
                                                              float max_delta)
{
  std::vector<float> classes_multipliers;

  if (cpc) {
    std::vector<int> counters_per_class = parse_int_list(cpc);
    const int classes_counters = static_cast<int>(counters_per_class.size());

    if (classes_counters != classes) {
      CLOGE(
          " number of values in counters_per_class = %d doesn't match with "
          "classes = %d",
          classes_counters, classes);
      throw NetworkError(
          "The counters_per_class values don't match the classes");
    }

    float max_counter = 0;
    for (int& counter : counters_per_class) {
      if (counter < 1) counter = 1;
      if (max_counter < static_cast<float>(counter)) {
        max_counter = static_cast<float>(counter);
      }
    }

    classes_multipliers.resize(counters_per_class.size());
    for (size_t i = 0; i < counters_per_class.size(); ++i) {
      classes_multipliers[i] =
          max_counter / static_cast<float>(counters_per_class[i]);
      if (classes_multipliers[i] > max_delta) {
        classes_multipliers[i] = max_delta;
      }
    }

    CLOGIM(" classes_multipliers: ");
    for (size_t i = 0; i < classes_multipliers.size(); ++i) {
      CLOGI("classes_multipliers[%zu] = %.1f", i,
            static_cast<double>(classes_multipliers[i]));
    }
  }

  return classes_multipliers;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_yolo(CfgSection& options,
                                               const size_params& params)
{
  const int classes = options.find_int("classes", 20);
  const int total = options.find_int("num", 1);
  const char* a = options.find_str("mask", nullptr);
  std::vector<int> mask = parse_int_list(a);
  const int num = a ? static_cast<int>(mask.size()) : total;
  const int max_boxes = options.find_int_quiet("max", 200);

  CLOGD("Making yolo layer: %d %d %d %d %d mask=%s %d %d", params.batch,
        params.w, params.h, num, total, a ? a : "none", classes, max_boxes);

  ALayerAdaptorPtr adaptor =
      layers::YoloLayer::make(params.batch, params.w, params.h, num, total,
                              std::move(mask), classes, max_boxes);
  layer& l = *adaptor->get();

  if (l.outputs != params.inputs) {
    CLOGE(
        "Error: l.outputs != params.inputs [convolutional]-layer doesn't "
        "correspond %d != %d",
        l.outputs, params.inputs);
    throw NetworkError(
        "Error: l.outputs == params.inputs, filters= in the "
        "[convolutional]-layer doesn't correspond to classes= or mask= in "
        "[yolo]-layer");
  }

  l.show_details = options.find_int_quiet("show_details", 1);
  l.max_delta = options.find_float_quiet("max_delta", FLT_MAX);  // set 10
  const char* cpc = options.find_str("counters_per_class", nullptr);
  adaptor->classes_multipliers(
      get_classes_multipliers(cpc, classes, l.max_delta));

  l.label_smooth_eps = options.find_float_quiet("label_smooth_eps", 0.0f);
  l.scale_x_y = options.find_float_quiet("scale_x_y", 1);
  l.objectness_smooth = options.find_int_quiet("objectness_smooth", 0);
  l.new_coords = options.find_int_quiet("new_coords", 0);
  l.iou_normalizer = options.find_float_quiet("iou_normalizer", 0.75);
  l.obj_normalizer = options.find_float_quiet("obj_normalizer", 1);
  l.cls_normalizer = options.find_float_quiet("cls_normalizer", 1);
  l.delta_normalizer = options.find_float_quiet("delta_normalizer", 1);
  const std::string iou_loss = options.find_str_quiet("iou_loss", "mse");

  if (iou_loss == "mse") {
    l.iou_loss = MSE;
  } else if (iou_loss == "giou") {
    l.iou_loss = GIOU;
  } else if (iou_loss == "diou") {
    l.iou_loss = DIOU;
  } else if (iou_loss == "ciou") {
    l.iou_loss = CIOU;
  } else {
    l.iou_loss = IOU;
  }

  CLOGI(
      "[yolo] params: iou loss: %s (%d), iou_norm: %2.2f, obj_norm: %2.2f, "
      "cls_norm: %2.2f, delta_norm: %2.2f, scale_x_y: %2.2f",
      iou_loss.c_str(), l.iou_loss, static_cast<double>(l.iou_normalizer),
      static_cast<double>(l.obj_normalizer),
      static_cast<double>(l.cls_normalizer),
      static_cast<double>(l.delta_normalizer),
      static_cast<double>(l.scale_x_y));

  const std::string iou_thresh_kind_str =
      options.find_str_quiet("iou_thresh_kind", "iou");
  if (iou_thresh_kind_str == "iou") {
    l.iou_thresh_kind = IOU;
  } else if (iou_thresh_kind_str == "giou") {
    l.iou_thresh_kind = GIOU;
  } else if (iou_thresh_kind_str == "diou") {
    l.iou_thresh_kind = DIOU;
  } else if (iou_thresh_kind_str == "ciou") {
    l.iou_thresh_kind = CIOU;
  } else {
    CLOGW(" Wrong iou_thresh_kind = %s", iou_thresh_kind_str.c_str());
    l.iou_thresh_kind = IOU;
  }

  l.beta_nms = options.find_float_quiet("beta_nms", 0.6f);
  const std::string nms_kind = options.find_str_quiet("nms_kind", "default");
  if (nms_kind == "default") {
    l.nms_kind = DEFAULT_NMS;
  } else {
    if (nms_kind == "greedynms") {
      l.nms_kind = GREEDY_NMS;
    } else if (nms_kind == "diounms") {
      l.nms_kind = DIOU_NMS;
    } else {
      l.nms_kind = DEFAULT_NMS;
    }
    CLOGI("nms_kind: %s (%d), beta = %f", nms_kind.c_str(), l.nms_kind,
          static_cast<double>(l.beta_nms));
  }

  l.jitter = options.find_float("jitter", .2f);
  l.resize = options.find_float_quiet("resize", 1.0);
  l.focal_loss = options.find_int_quiet("focal_loss", 0);

  l.ignore_thresh = options.find_float("ignore_thresh", .5);
  l.truth_thresh = options.find_float("truth_thresh", 1);
  // recommended to use iou_thresh=0.213 in [yolo]
  l.iou_thresh = options.find_float_quiet("iou_thresh", 1);
  l.random = options.find_float_quiet("random", 0);

  l.track_history_size = options.find_int_quiet("track_history_size", 5);
  l.sim_thresh = options.find_float_quiet("sim_thresh", 0.8f);
  l.dets_for_track = options.find_int_quiet("dets_for_track", 1);
  l.dets_for_show = options.find_int_quiet("dets_for_show", 1);
  l.track_ciou_norm = options.find_float_quiet("track_ciou_norm", 0.01f);
  int embedding_layer_id = options.find_int_quiet("embedding_layer", 999999);
  if (embedding_layer_id < 0) {
    embedding_layer_id = params.index + embedding_layer_id;
  }
  if (embedding_layer_id != 999999) {
    CLOGI(" embedding_layer_id = %d", embedding_layer_id);
    const layer& le = parsed_layer(params, embedding_layer_id);
    l.embedding_layer_id = embedding_layer_id;
    adaptor->embedding_output(layers::LayersBuffers::floats(
        layers::LayersBuffers::count(le.batch, le.outputs)));
    l.embedding_size = le.n / l.n;
    CLOGI(" embedding_size = %d", l.embedding_size);
    if (le.n % l.n != 0) {
      CLOGW(
          "Warning: filters=%d number in embedding_layer=%d isn't divisable "
          "by number of anchors %d",
          le.n, embedding_layer_id, l.n);
    }
  }

  const char* map_file = options.find_str("map", nullptr);
  if (map_file) adaptor->map(read_map(map_file));

  a = options.find_str("anchors", nullptr);
  if (a) {
    const std::vector<float> anchors = parse_float_list(a, true);
    for (int i = 0; i < static_cast<int>(anchors.size()) && i < total * 2;
         ++i) {
      l.biases[i] = anchors[static_cast<size_t>(i)];
    }
  }

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_gaussian_yolo(
    CfgSection& options, const size_params& params)
{
  const int classes = options.find_int("classes", 20);
  const int max_boxes = options.find_int_quiet("max", 200);
  const int total = options.find_int("num", 1);

  const char* a = options.find_str("mask", nullptr);
  std::vector<int> mask = parse_int_list(a);
  const int num = a ? static_cast<int>(mask.size()) : total;

  ALayerAdaptorPtr adaptor = layers::GaussianYoloLayer::make(
      params.batch, params.w, params.h, num, total, std::move(mask), classes,
      max_boxes);
  layer& l = *adaptor->get();

  if (l.outputs != params.inputs) {
    CLOGE(
        "Error: l.outputs != params.inputs [convolutional]-layer doesn't "
        "correspond %d != %d",
        l.outputs, params.inputs);
    throw NetworkError(
        "Error: l.outputs == params.inputs, filters= in the "
        "[convolutional]-layer doesn't correspond to classes= or mask= in "
        "[Gaussian_yolo]-layer");
  }

  l.max_delta = options.find_float_quiet("max_delta", FLT_MAX);  // set 10
  const char* cpc = options.find_str("counters_per_class", nullptr);
  adaptor->classes_multipliers(
      get_classes_multipliers(cpc, classes, l.max_delta));

  l.label_smooth_eps = options.find_float_quiet("label_smooth_eps", 0.0f);
  l.scale_x_y = options.find_float_quiet("scale_x_y", 1);
  l.objectness_smooth = options.find_int_quiet("objectness_smooth", 0);
  l.uc_normalizer = options.find_float_quiet("uc_normalizer", 1.0);
  l.iou_normalizer = options.find_float_quiet("iou_normalizer", 0.75);
  l.obj_normalizer = options.find_float_quiet("obj_normalizer", 1.0);
  l.cls_normalizer = options.find_float_quiet("cls_normalizer", 1);
  l.delta_normalizer = options.find_float_quiet("delta_normalizer", 1);
  const std::string iou_loss = options.find_str_quiet("iou_loss", "mse");

  if (iou_loss == "mse") {
    l.iou_loss = MSE;
  } else if (iou_loss == "giou") {
    l.iou_loss = GIOU;
  } else if (iou_loss == "diou") {
    l.iou_loss = DIOU;
  } else if (iou_loss == "ciou") {
    l.iou_loss = CIOU;
  } else {
    l.iou_loss = IOU;
  }

  const std::string iou_thresh_kind_str =
      options.find_str_quiet("iou_thresh_kind", "iou");
  if (iou_thresh_kind_str == "iou") {
    l.iou_thresh_kind = IOU;
  } else if (iou_thresh_kind_str == "giou") {
    l.iou_thresh_kind = GIOU;
  } else if (iou_thresh_kind_str == "diou") {
    l.iou_thresh_kind = DIOU;
  } else if (iou_thresh_kind_str == "ciou") {
    l.iou_thresh_kind = CIOU;
  } else {
    CLOGE("Wrong iou_thresh_kind = %s", iou_thresh_kind_str.c_str());
    l.iou_thresh_kind = IOU;
  }

  l.beta_nms = options.find_float_quiet("beta_nms", 0.6f);
  const std::string nms_kind = options.find_str_quiet("nms_kind", "default");
  if (nms_kind == "default") {
    l.nms_kind = DEFAULT_NMS;
  } else {
    if (nms_kind == "greedynms") {
      l.nms_kind = GREEDY_NMS;
    } else if (nms_kind == "diounms") {
      l.nms_kind = DIOU_NMS;
    } else if (nms_kind == "cornersnms") {
      l.nms_kind = CORNERS_NMS;
    } else {
      l.nms_kind = DEFAULT_NMS;
    }
    CLOGE("nms_kind: %s (%d), beta = %f", nms_kind.c_str(), l.nms_kind,
          static_cast<double>(l.beta_nms));
  }

  const std::string yolo_point = options.find_str_quiet("yolo_point", "center");
  if (yolo_point == "left_top") {
    l.yolo_point = YOLO_LEFT_TOP;
  } else if (yolo_point == "right_bottom") {
    l.yolo_point = YOLO_RIGHT_BOTTOM;
  } else {
    l.yolo_point = YOLO_CENTER;
  }

  CLOGI(
      "[Gaussian_yolo] iou loss: %s (%d), iou_norm: %2.2f, obj_norm: %2.2f, "
      "cls_norm: %2.2f, delta_norm: %2.2f, scale: %2.2f, point: %d",
      iou_loss.c_str(), l.iou_loss, static_cast<double>(l.iou_normalizer),
      static_cast<double>(l.obj_normalizer),
      static_cast<double>(l.cls_normalizer),
      static_cast<double>(l.delta_normalizer), static_cast<double>(l.scale_x_y),
      l.yolo_point);

  l.jitter = options.find_float("jitter", .2f);
  l.resize = options.find_float_quiet("resize", 1.0);

  l.ignore_thresh = options.find_float("ignore_thresh", .5);
  l.truth_thresh = options.find_float("truth_thresh", 1);
  // recommended to use iou_thresh=0.213 in [yolo]
  l.iou_thresh = options.find_float_quiet("iou_thresh", 1);
  l.random = options.find_float_quiet("random", 0);

  const char* map_file = options.find_str("map", nullptr);
  if (map_file) adaptor->map(read_map(map_file));

  a = options.find_str("anchors", nullptr);
  if (a) {
    // the original wrote every value, past the biases of the total anchors
    // as well, the values past them are ignored here
    const std::vector<float> anchors = parse_float_list(a, false);
    for (int i = 0; i < static_cast<int>(anchors.size()) && i < total * 2;
         ++i) {
      l.biases[i] = anchors[static_cast<size_t>(i)];
    }
  }

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_region(CfgSection& options,
                                                 const size_params& params)
{
  const int coords = options.find_int("coords", 4);
  const int classes = options.find_int("classes", 20);
  const int num = options.find_int("num", 1);
  const int max_boxes = options.find_int_quiet("max", 200);

  ALayerAdaptorPtr adaptor = layers::RegionLayer::make(
      params.batch, params.w, params.h, num, classes, coords, max_boxes);
  layer& l = *adaptor->get();

  if (l.outputs != params.inputs) {
    CLOGE(
        "Error: l.outputs != params.inputs [convolutional]-layer doesn't "
        "correspond %d != %d",
        l.outputs, params.inputs);
    throw NetworkError(
        "Error: l.outputs == params.inputs, filters= in the "
        "[convolutional]-layer doesn't correspond to classes= or num= in "
        "[region]-layer");
  }

  l.log = options.find_int_quiet("log", 0);
  l.sqrt = options.find_int_quiet("sqrt", 0);

  l.softmax = options.find_int("softmax", 0);
  l.focal_loss = options.find_int_quiet("focal_loss", 0);
  l.jitter = options.find_float("jitter", .2f);
  l.resize = options.find_float_quiet("resize", 1.0);
  l.rescore = options.find_int_quiet("rescore", 0);

  l.thresh = options.find_float("thresh", .5);
  l.classfix = options.find_int_quiet("classfix", 0);
  l.absolute = options.find_int_quiet("absolute", 0);
  l.random = options.find_float_quiet("random", 0);

  l.coord_scale = options.find_float("coord_scale", 1);
  l.object_scale = options.find_float("object_scale", 1);
  l.noobject_scale = options.find_float("noobject_scale", 1);
  l.mask_scale = options.find_float("mask_scale", 1);
  l.class_scale = options.find_float("class_scale", 1);
  l.bias_match = options.find_int_quiet("bias_match", 0);

  const char* tree_file = options.find_str("tree", nullptr);
  if (tree_file) adaptor->own_softmax_tree(read_tree(tree_file));
  const char* map_file = options.find_str("map", nullptr);
  if (map_file) adaptor->map(read_map(map_file));

  const char* a = options.find_str("anchors", nullptr);
  if (a) {
    const std::vector<float> anchors = parse_float_list(a, false);
    for (int i = 0; i < static_cast<int>(anchors.size()) && i < num * 2; ++i) {
      l.biases[i] = anchors[static_cast<size_t>(i)];
    }
  }

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_detection(CfgSection& options,
                                                    const size_params& params)
{
  const int coords = options.find_int("coords", 1);
  const int classes = options.find_int("classes", 1);
  const int rescore = options.find_int("rescore", 0);
  const int num = options.find_int("num", 1);
  const int side = options.find_int("side", 7);

  ALayerAdaptorPtr adaptor = layers::DetectionLayer::make(
      params.batch, params.inputs, num, side, classes, coords, rescore);
  layer& l = *adaptor->get();

  l.softmax = options.find_int("softmax", 0);
  l.sqrt = options.find_int("sqrt", 0);

  l.max_boxes = options.find_int_quiet("max", 200);
  l.coord_scale = options.find_float("coord_scale", 1);
  l.forced = options.find_int("forced", 0);
  l.object_scale = options.find_float("object_scale", 1);
  l.noobject_scale = options.find_float("noobject_scale", 1);
  l.class_scale = options.find_float("class_scale", 1);
  l.jitter = options.find_float("jitter", .2f);
  l.resize = options.find_float_quiet("resize", 1.0);
  l.random = options.find_float_quiet("random", 0);
  l.reorg = options.find_int_quiet("reorg", 0);

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_cost(CfgSection& options,
                                               const size_params& params)
{
  const char* type_s = options.find_str("type", "sse");
  const COST_TYPE type = get_cost_type(c_str(type_s));
  const float scale = options.find_float_quiet("scale", 1);

  ALayerAdaptorPtr adaptor =
      layers::CostLayer::make(params.batch, params.inputs, type, scale);

  adaptor->get()->ratio = options.find_float_quiet("ratio", 0);

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_crop(CfgSection& options,
                                               const size_params& params)
{
  const int crop_height = options.find_int("crop_height", 1);
  const int crop_width = options.find_int("crop_width", 1);
  const int flip = options.find_int("flip", 0);
  const float angle = options.find_float("angle", 0);
  const float saturation = options.find_float("saturation", 1);
  const float exposure = options.find_float("exposure", 1);

  const int h = params.h;
  const int w = params.w;
  const int c = params.c;
  const int batch = params.batch;
  if (!(h && w && c)) {
    throw NetworkError("Layer before crop layer must output image.");
  }

  const int noadjust = options.find_int_quiet("noadjust", 0);

  ALayerAdaptorPtr adaptor =
      layers::CropLayer::make(batch, h, w, c, crop_height, crop_width, flip,
                              angle, saturation, exposure);
  layer& l = *adaptor->get();

  l.shift = options.find_float("shift", 0);
  l.noadjust = noadjust;

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_reorg(CfgSection& options,
                                                const size_params& params)
{
  const int stride = options.find_int("stride", 1);
  const int reverse = options.find_int_quiet("reverse", 0);

  const int h = params.h;
  const int w = params.w;
  const int c = params.c;
  const int batch = params.batch;
  if (!(h && w && c)) {
    throw NetworkError("Layer before reorg layer must output image.");
  }

  return layers::ReorgLayer::make(batch, w, h, c, stride, reverse);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_reorg_old(CfgSection& options,
                                                    const size_params& params)
{
  CLOGIM("reorg_old");

  const int stride = options.find_int("stride", 1);
  const int reverse = options.find_int_quiet("reverse", 0);

  const int h = params.h;
  const int w = params.w;
  const int c = params.c;
  const int batch = params.batch;
  if (!(h && w && c)) {
    throw NetworkError("Layer before reorg layer must output image.");
  }

  return layers::ReorgOldLayer::make(batch, w, h, c, stride, reverse);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_local_avgpool(
    CfgSection& options, const size_params& params)
{
  const int stride = options.find_int("stride", 1);
  const int stride_x = options.find_int_quiet("stride_x", stride);
  const int stride_y = options.find_int_quiet("stride_y", stride);
  const int size = options.find_int("size", stride);
  const int padding = options.find_int_quiet("padding", size - 1);
  const int maxpool_depth = 0;
  const int out_channels = 1;
  const int antialiasing = 0;
  const int avgpool = 1;

  const int h = params.h;
  const int w = params.w;
  const int c = params.c;
  const int batch = params.batch;
  if (!(h && w && c)) {
    throw NetworkError("Layer before [local_avgpool] layer must output image.");
  }

  return layers::MaxpoolLayer::make(batch, h, w, c, size, stride_x, stride_y,
                                    padding, maxpool_depth, out_channels,
                                    antialiasing, avgpool, params.train);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_maxpool(CfgSection& options,
                                                  const size_params& params)
{
  const int stride = options.find_int("stride", 1);
  const int stride_x = options.find_int_quiet("stride_x", stride);
  const int stride_y = options.find_int_quiet("stride_y", stride);
  const int size = options.find_int("size", stride);
  const int padding = options.find_int_quiet("padding", size - 1);
  const int maxpool_depth = options.find_int_quiet("maxpool_depth", 0);
  const int out_channels = options.find_int_quiet("out_channels", 1);
  const int antialiasing = options.find_int_quiet("antialiasing", 0);
  const int avgpool = 0;

  const int h = params.h;
  const int w = params.w;
  const int c = params.c;
  const int batch = params.batch;
  if (!(h && w && c)) {
    throw NetworkError("Layer before [maxpool] layer must output image.");
  }

  ALayerAdaptorPtr adaptor = layers::MaxpoolLayer::make(
      batch, h, w, c, size, stride_x, stride_y, padding, maxpool_depth,
      out_channels, antialiasing, avgpool, params.train);

  adaptor->get()->maxpool_zero_nonmax =
      options.find_int_quiet("maxpool_zero_nonmax", 0);

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_avgpool(CfgSection& options,
                                                  const size_params& params)
{
  (void)options;

  const int w = params.w;
  const int h = params.h;
  const int c = params.c;
  const int batch = params.batch;
  if (!(h && w && c)) {
    throw NetworkError("Layer before avgpool layer must output image.");
  }

  return layers::AvgpoolLayer::make(batch, w, h, c);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_dropout(CfgSection& options,
                                                  const size_params& params)
{
  const float probability = options.find_float("probability", .2f);
  const int dropblock = options.find_int_quiet("dropblock", 0);
  float dropblock_size_rel = options.find_float_quiet("dropblock_size_rel", 0);
  int dropblock_size_abs =
      static_cast<int>(options.find_float_quiet("dropblock_size_abs", 0));
  if (dropblock_size_abs > params.w || dropblock_size_abs > params.h) {
    CLOGW(
        " [dropout] - dropblock_size_abs = %d that is bigger than layer size "
        "%d x %d",
        dropblock_size_abs, params.w, params.h);
    dropblock_size_abs = min_val_cmp(params.w, params.h);
  }
  if (dropblock && !dropblock_size_rel && !dropblock_size_abs) {
    CLOGWM(
        " [dropout] - None of the parameters (dropblock_size_rel or "
        "dropblock_size_abs) are set, will be used: dropblock_size_abs = 7");
    dropblock_size_abs = 7;
  }
  if (dropblock_size_rel && dropblock_size_abs) {
    CLOGW(
        " [dropout] - Both parameters are set, only the parameter will be "
        "used: dropblock_size_abs = %d",
        dropblock_size_abs);
    dropblock_size_rel = 0;
  }

  ALayerAdaptorPtr adaptor = layers::DropoutLayer::make(
      params.batch, params.inputs, probability, dropblock, dropblock_size_rel,
      dropblock_size_abs, params.w, params.h, params.c);
  layer& l = *adaptor->get();

  l.out_w = params.w;
  l.out_h = params.h;
  l.out_c = params.c;

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_normalization(
    CfgSection& options, const size_params& params)
{
  const float alpha = options.find_float("alpha", .0001f);
  const float beta = options.find_float("beta", .75);
  const float kappa = options.find_float("kappa", 1);
  const int size = options.find_int("size", 5);

  return layers::NormalizationLayer::make(params.batch, params.w, params.h,
                                          params.c, size, alpha, beta, kappa);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_batchnorm(CfgSection& options,
                                                    const size_params& params)
{
  (void)options;

  return layers::BatchnormLayer::make(params.batch, params.w, params.h,
                                      params.c, params.train);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_shortcut(CfgSection& options,
                                                   const size_params& params)
{
  const char* activation_s = options.find_str("activation", "linear");
  const ACTIVATION activation = activation_of(activation_s);

  const std::string weights_type_str =
      options.find_str_quiet("weights_type", "none");
  WEIGHTS_TYPE_T weights_type = NO_WEIGHTS;
  if (weights_type_str == "per_feature" || weights_type_str == "per_layer") {
    weights_type = PER_FEATURE;
  } else if (weights_type_str == "per_channel") {
    weights_type = PER_CHANNEL;
  } else if (weights_type_str != "none") {
    CLOGE(
        "Error: Incorrect weights_type = %s Use one of: none, per_feature, "
        "per_channel",
        weights_type_str.c_str());
    throw NetworkError("Incorrect weights_type of the [shortcut] layer");
  }

  const std::string weights_normalization_str =
      options.find_str_quiet("weights_normalization", "none");
  WEIGHTS_NORMALIZATION_T weights_normalization = NO_NORMALIZATION;
  if (weights_normalization_str == "relu" ||
      weights_normalization_str == "avg_relu") {
    weights_normalization = RELU_NORMALIZATION;
  } else if (weights_normalization_str == "softmax") {
    weights_normalization = SOFTMAX_NORMALIZATION;
  } else if (weights_type_str != "none") {
    // the original tests the weights_type here as well
    CLOGE(
        "Error: Incorrect weights_normalization = %s Use one of: none, "
        "relu, softmax",
        weights_normalization_str.c_str());
    throw NetworkError(
        "Incorrect weights_normalization of the [shortcut] layer");
  }

  const char* l = options.find("from");
  if (!l) {
    throw NetworkError("Route Layer must specify input layers: from = ...");
  }

  const std::vector<std::string> pieces = split(l, false);

  // the exact buffers of the original
  std::vector<int> input_layers;
  std::vector<int> sizes;
  adaptors::vrfloats layers_output;
  adaptors::vrfloats layers_delta;
  input_layers.reserve(pieces.size());
  sizes.reserve(pieces.size());
  layers_output.reserve(pieces.size());
  layers_delta.reserve(pieces.size());

  for (const std::string& piece : pieces) {
    int index = std::atoi(piece.c_str());
    if (index < 0) index = params.index + index;

    const layer& from = parsed_layer(params, index);

    input_layers.push_back(index);
    sizes.push_back(from.outputs);
    layers_output.push_back(from.output);
    layers_delta.push_back(from.delta);
  }

  const std::vector<int> indexes = input_layers;

  ALayerAdaptorPtr adaptor = layers::ShortcutLayer::make(
      params.batch, std::move(input_layers), std::move(sizes), params.w,
      params.h, params.c, std::move(layers_output), std::move(layers_delta),
      weights_type, weights_normalization, activation, params.train);

  for (const int index : indexes) {
    const layer& from = params.net->get()->layers[index];

    assert(params.w == from.out_w && params.h == from.out_h);

    if (params.w != from.out_w || params.h != from.out_h ||
        params.c != from.out_c) {
      CLOGI(" (%4d x%4d x%4d) + (%4d x%4d x%4d)", params.w, params.h, params.c,
            from.out_w, from.out_h, from.out_c);
    }
  }

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_scale_channels(
    CfgSection& options, const size_params& params)
{
  const char* l = options.find("from");
  if (!l) {
    throw NetworkError("The [scale_channels] layer must specify from = ...");
  }
  int index = std::atoi(l);
  if (index < 0) index = params.index + index;
  const int scale_wh = options.find_int_quiet("scale_wh", 0);

  const int batch = params.batch;
  const layer& from = parsed_layer(params, index);

  ALayerAdaptorPtr adaptor = layers::ScaleChannelsLayer::make(
      batch, index, params.w, params.h, params.c, from.out_w, from.out_h,
      from.out_c, scale_wh);

  const char* activation_s = options.find_str_quiet("activation", "linear");
  const ACTIVATION activation = activation_of(activation_s);
  adaptor->get()->activation = activation;
  if (activation == SWISH || activation == MISH) {
    CLOGWM(" [scale_channels] layer doesn't support SWISH or MISH activations");
  }

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_sam(CfgSection& options,
                                              const size_params& params)
{
  const char* l = options.find("from");
  if (!l) {
    throw NetworkError("The [sam] layer must specify from = ...");
  }
  int index = std::atoi(l);
  if (index < 0) index = params.index + index;

  const int batch = params.batch;
  const layer& from = parsed_layer(params, index);

  ALayerAdaptorPtr adaptor =
      layers::SamLayer::make(batch, index, params.w, params.h, params.c,
                             from.out_w, from.out_h, from.out_c);

  const char* activation_s = options.find_str_quiet("activation", "linear");
  const ACTIVATION activation = activation_of(activation_s);
  adaptor->get()->activation = activation;
  if (activation == SWISH || activation == MISH) {
    CLOGWM(" [sam] layer doesn't support SWISH or MISH activations");
  }

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_implicit(CfgSection& options,
                                                   const size_params& params)
{
  const float mean_init = options.find_float("mean", 0.0);
  const float std_init = options.find_float("std", 0.2f);
  const int filters = options.find_int("filters", 128);
  const int atoms = options.find_int_quiet("atoms", 1);

  return layers::ImplicitLayer::make(params.batch, params.index, mean_init,
                                     std_init, filters, atoms);
}

ALayerAdaptorPtr OrigDefaultLoader::parse_activation(CfgSection& options,
                                                     const size_params& params)
{
  const char* activation_s = options.find_str("activation", "linear");
  const ACTIVATION activation = activation_of(activation_s);

  ALayerAdaptorPtr adaptor =
      layers::ActivationLayer::make(params.batch, params.inputs, activation);
  layer& l = *adaptor->get();

  l.out_h = params.h;
  l.out_w = params.w;
  l.out_c = params.c;
  l.h = params.h;
  l.w = params.w;
  l.c = params.c;

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_upsample(CfgSection& options,
                                                   const size_params& params)
{
  const int stride = options.find_int("stride", 2);

  ALayerAdaptorPtr adaptor = layers::UpsampleLayer::make(
      params.batch, params.w, params.h, params.c, stride);

  adaptor->get()->scale = options.find_float_quiet("scale", 1);

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_route(CfgSection& options,
                                                const size_params& params)
{
  const char* l = options.find("layers");
  if (!l) throw NetworkError("Route Layer must specify input layers");

  const std::vector<std::string> pieces = split(l, false);

  // the exact buffers of the original
  std::vector<int> input_layers;
  std::vector<int> sizes;
  input_layers.reserve(pieces.size());
  sizes.reserve(pieces.size());

  for (const std::string& piece : pieces) {
    int index = std::atoi(piece.c_str());
    if (index < 0) index = params.index + index;

    input_layers.push_back(index);
    sizes.push_back(parsed_layer(params, index).outputs);
  }

  const int batch = params.batch;

  const int groups = options.find_int_quiet("groups", 1);
  const int group_id = options.find_int_quiet("group_id", 0);

  const network& net = *params.net->get();
  const layer& first = net.layers[input_layers[0]];
  std::vector<int> indexes = input_layers;

  ALayerAdaptorPtr adaptor = layers::RouteLayer::make(
      batch, std::move(input_layers), std::move(sizes), groups, group_id);
  layer& route = *adaptor->get();

  route.out_w = first.out_w;
  route.out_h = first.out_h;
  route.out_c = first.out_c;
  for (size_t i = 1; i < indexes.size(); ++i) {
    const layer& next = net.layers[indexes[i]];
    if (next.out_w == first.out_w && next.out_h == first.out_h) {
      route.out_c += next.out_c;
    } else {
      CLOGWM(" The width and height of the input layers are different.");
      route.out_h = route.out_w = route.out_c = 0;
    }
  }
  route.out_c = route.out_c / route.groups;

  route.w = first.w;
  route.h = first.h;
  route.c = route.out_c;

  route.stream = options.find_int_quiet("stream", -1);
  route.wait_stream_id = options.find_int_quiet("wait_stream", -1);

  if (route.groups > 1) CLOGI("%d/%d", route.group_id, route.groups);

  CLOGI(" -> %4d x%4d x%4d", route.out_w, route.out_h, route.out_c);

  return adaptor;
}

ALayerAdaptorPtr OrigDefaultLoader::parse_empty(const size_params& params)
{
  const layer& previous = parsed_layer(params, params.index - 1);

  return layers::EmptyLayer::make(params.batch, params.w, params.h, params.c,
                                  params.inputs, previous.output,
                                  previous.delta);
}

std::vector<int> OrigDefaultLoader::read_map(const std::string& filename)
{
  std::ifstream file{filename};

  if (!file.is_open()) {
    CLOGE("Couldn't open file: %s", filename.c_str());
    throw NetworkError("Couldn't open the map file: " + filename);
  }

  std::vector<int> map;
  std::string line;

  while (std::getline(file, line)) {
    map.push_back(std::atoi(line.c_str()));
  }

  // the exact buffer of the original
  map.shrink_to_fit();

  return map;
}

std::shared_ptr<tree> OrigDefaultLoader::read_tree(const std::string& filename)
{
  std::ifstream file{filename};

  if (!file.is_open()) {
    CLOGE("Couldn't open file: %s", filename.c_str());
    throw NetworkError("Couldn't open the tree file: " + filename);
  }

  auto storage = std::make_shared<TreeStorage>();

  int last_parent = -1;
  int group_size = 0;
  int groups = 0;
  int n = 0;
  std::string line;

  while (std::getline(file, line)) {
    std::istringstream fields{line};
    std::string id;
    int parent = -1;

    fields >> id >> parent;

    storage->parent.push_back(parent);
    storage->names.push_back(id);

    if (parent != last_parent) {
      ++groups;
      storage->group_offset.push_back(n - group_size);
      storage->group_size.push_back(group_size);
      group_size = 0;
      last_parent = parent;
    }

    storage->group.push_back(groups);
    ++n;
    ++group_size;
  }

  ++groups;
  storage->group_offset.push_back(n - group_size);
  storage->group_size.push_back(group_size);

  storage->leaf.assign(static_cast<size_t>(n), 1);
  for (const int parent : storage->parent) {
    // the original wrote past the leaves for the parent out of the tree
    if (parent >= 0 && parent < n)
      storage->leaf[static_cast<size_t>(parent)] = 0;
  }

  for (std::string& name : storage->names) {
    storage->name_pointers.push_back(name.data());
  }

  tree& t = storage->view;
  t.n = n;
  t.groups = groups;
  t.leaf = adaptors::StructAdaptorHelper::data_or_null(storage->leaf);
  t.parent = adaptors::StructAdaptorHelper::data_or_null(storage->parent);
  t.child = nullptr;
  t.group = adaptors::StructAdaptorHelper::data_or_null(storage->group);
  t.name = adaptors::StructAdaptorHelper::data_or_null(storage->name_pointers);
  t.group_size =
      adaptors::StructAdaptorHelper::data_or_null(storage->group_size);
  t.group_offset =
      adaptors::StructAdaptorHelper::data_or_null(storage->group_offset);

  // an aliasing pointer: the tree view keeps it's storage alive
  return std::shared_ptr<tree>(storage, &storage->view);
}

LAYER_TYPE OrigDefaultLoader::string_to_layer_type(const std::string& type)
{
  static const std::map<std::string, LAYER_TYPE> section2type{
      {"[shortcut]", SHORTCUT},
      {"[scale_channels]", SCALE_CHANNELS},
      {"[sam]", SAM},
      {"[crop]", CROP},
      {"[cost]", COST},
      {"[detection]", DETECTION},
      {"[region]", REGION},
      {"[yolo]", YOLO},
      {"[Gaussian_yolo]", GAUSSIAN_YOLO},
      {"[local]", LOCAL},
      {"[conv]", CONVOLUTIONAL},
      {"[convolutional]", CONVOLUTIONAL},
      {"[activation]", ACTIVE},
      {"[net]", NETWORK},
      {"[network]", NETWORK},
      {"[crnn]", CRNN},
      {"[gru]", GRU},
      {"[lstm]", LSTM},
      {"[conv_lstm]", CONV_LSTM},
      {"[history]", HISTORY},
      {"[rnn]", RNN},
      {"[conn]", CONNECTED},
      {"[connected]", CONNECTED},
      {"[max]", MAXPOOL},
      {"[maxpool]", MAXPOOL},
      {"[local_avg]", LOCAL_AVGPOOL},
      {"[local_avgpool]", LOCAL_AVGPOOL},
      {"[reorg3d]", REORG},
      {"[reorg]", REORG_OLD},
      {"[avg]", AVGPOOL},
      {"[avgpool]", AVGPOOL},
      {"[dropout]", DROPOUT},
      {"[lrn]", NORMALIZATION},
      {"[normalization]", NORMALIZATION},
      {"[batchnorm]", BATCHNORM},
      {"[soft]", SOFTMAX},
      {"[softmax]", SOFTMAX},
      {"[contrastive]", CONTRASTIVE},
      {"[route]", ROUTE},
      {"[upsample]", UPSAMPLE},
      {"[empty]", EMPTY},
      {"[silence]", EMPTY},
      {"[implicit]", IMPLICIT}};

  const auto found = section2type.find(type);

  return found == section2type.end() ? BLANK : found->second;
}

const layer& OrigDefaultLoader::parsed_layer(const size_params& params,
                                             int index)
{
  if (index < 0 || index >= params.index) {
    CLOGE("The layer %d refers to the layer %d, which isn't parsed before it",
          params.index, index);
    throw NetworkError("The layer " + std::to_string(params.index) +
                       " refers to the not parsed layer " +
                       std::to_string(index));
  }

  return params.net->get()->layers[index];
}

void OrigDefaultLoader::set_train_only_bn(network& net)
{
  int train_only_bn = 0;

  for (int i = net.n - 1; i >= 0; --i) {
    // set l.train_only_bn for all previous layers
    if (net.layers[i].train_only_bn) {
      train_only_bn = net.layers[i].train_only_bn;
    }

    if (train_only_bn) {
      net.layers[i].train_only_bn = train_only_bn;

      if (net.layers[i].type == CONV_LSTM) {
        net.layers[i].wf->train_only_bn = train_only_bn;
        net.layers[i].wi->train_only_bn = train_only_bn;
        net.layers[i].wg->train_only_bn = train_only_bn;
        net.layers[i].wo->train_only_bn = train_only_bn;
        net.layers[i].uf->train_only_bn = train_only_bn;
        net.layers[i].ui->train_only_bn = train_only_bn;
        net.layers[i].ug->train_only_bn = train_only_bn;
        net.layers[i].uo->train_only_bn = train_only_bn;
        if (net.layers[i].peephole) {
          net.layers[i].vf->train_only_bn = train_only_bn;
          net.layers[i].vi->train_only_bn = train_only_bn;
          net.layers[i].vo->train_only_bn = train_only_bn;
        }
      } else if (net.layers[i].type == CRNN) {
        net.layers[i].input_layer->train_only_bn = train_only_bn;
        net.layers[i].self_layer->train_only_bn = train_only_bn;
        net.layers[i].output_layer->train_only_bn = train_only_bn;
      }
    }
  }
}

}  // namespace darknet_adaptor::loaders
