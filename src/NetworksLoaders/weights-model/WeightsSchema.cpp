#include "src/NetworksLoaders/weights-model/WeightsSchema.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "src/darknet.h"

namespace darknet_adaptor::loaders
{

namespace
{

/// @brief The count of the arrays the owner of the arrays may have.
constexpr const int ARRAYS_PER_OWNER = 5;

const std::vector<std::pair<WeightsArrayKind, std::string>>& array_names()
{
  static const std::vector<std::pair<WeightsArrayKind, std::string>> names{
      {WeightsArrayKind::Biases, "biases"},
      {WeightsArrayKind::Scales, "scales"},
      {WeightsArrayKind::RollingMean, "rolling_mean"},
      {WeightsArrayKind::RollingVariance, "rolling_variance"},
      {WeightsArrayKind::Weights, "weights"}};

  return names;
}

const std::vector<std::pair<WeightsSubLayer, std::string>>& sublayer_names()
{
  static const std::vector<std::pair<WeightsSubLayer, std::string>> names{
      {WeightsSubLayer::None, ""},
      {WeightsSubLayer::InputLayer, "input_layer"},
      {WeightsSubLayer::SelfLayer, "self_layer"},
      {WeightsSubLayer::OutputLayer, "output_layer"},
      {WeightsSubLayer::InputZLayer, "input_z_layer"},
      {WeightsSubLayer::InputRLayer, "input_r_layer"},
      {WeightsSubLayer::InputHLayer, "input_h_layer"},
      {WeightsSubLayer::StateZLayer, "state_z_layer"},
      {WeightsSubLayer::StateRLayer, "state_r_layer"},
      {WeightsSubLayer::StateHLayer, "state_h_layer"},
      {WeightsSubLayer::Wf, "wf"},
      {WeightsSubLayer::Wi, "wi"},
      {WeightsSubLayer::Wg, "wg"},
      {WeightsSubLayer::Wo, "wo"},
      {WeightsSubLayer::Uf, "uf"},
      {WeightsSubLayer::Ui, "ui"},
      {WeightsSubLayer::Ug, "ug"},
      {WeightsSubLayer::Uo, "uo"},
      {WeightsSubLayer::Vf, "vf"},
      {WeightsSubLayer::Vi, "vi"},
      {WeightsSubLayer::Vo, "vo"}};

  return names;
}

/// @brief The cfg section names the OrigDefaultLoader parses into the types
/// (the first one of the synonyms) and the names of the rest of the types.
const std::vector<std::pair<LAYER_TYPE, std::string>>& type_names()
{
  static const std::vector<std::pair<LAYER_TYPE, std::string>> names{
      {CONVOLUTIONAL, "convolutional"},
      {DECONVOLUTIONAL, "deconvolutional"},
      {CONNECTED, "connected"},
      {MAXPOOL, "maxpool"},
      {LOCAL_AVGPOOL, "local_avgpool"},
      {SOFTMAX, "softmax"},
      {DETECTION, "detection"},
      {DROPOUT, "dropout"},
      {CROP, "crop"},
      {ROUTE, "route"},
      {COST, "cost"},
      {NORMALIZATION, "normalization"},
      {AVGPOOL, "avgpool"},
      {LOCAL, "local"},
      {SHORTCUT, "shortcut"},
      {SCALE_CHANNELS, "scale_channels"},
      {SAM, "sam"},
      {ACTIVE, "activation"},
      {RNN, "rnn"},
      {GRU, "gru"},
      {LSTM, "lstm"},
      {CONV_LSTM, "conv_lstm"},
      {HISTORY, "history"},
      {CRNN, "crnn"},
      {BATCHNORM, "batchnorm"},
      {NETWORK, "net"},
      {XNOR, "xnor"},
      {REGION, "region"},
      {YOLO, "yolo"},
      {GAUSSIAN_YOLO, "Gaussian_yolo"},
      {ISEG, "iseg"},
      {REORG, "reorg3d"},
      {REORG_OLD, "reorg"},
      {UPSAMPLE, "upsample"},
      {LOGXENT, "logxent"},
      {L2NORM, "l2norm"},
      {EMPTY, "empty"},
      {BLANK, "blank"},
      {CONTRASTIVE, "contrastive"},
      {IMPLICIT, "implicit"}};

  return names;
}

template <typename T>
const std::string& name_of(const std::vector<std::pair<T, std::string>>& names,
                           const T value)
{
  static const std::string unknown{"unknown"};

  const auto found =
      std::find_if(names.begin(), names.end(),
                   [value](const auto& entry) { return entry.first == value; });

  return found == names.end() ? unknown : found->second;
}

template <typename T>
bool value_of(const std::vector<std::pair<T, std::string>>& names,
              const std::string& name, T& value)
{
  const auto found =
      std::find_if(names.begin(), names.end(),
                   [&name](const auto& entry) { return entry.second == name; });

  if (found == names.end()) {
    return false;
  }

  value = found->first;

  return true;
}

/// @brief The position of the array among the arrays of the owner type.
int position(const std::vector<WeightsArrayKind>& arrays,
             const WeightsArrayKind kind)
{
  const auto found = std::find(arrays.begin(), arrays.end(), kind);

  return found == arrays.end()
             ? -1
             : static_cast<int>(std::distance(arrays.begin(), found));
}

}  // namespace

const std::string& WeightsSchema::name(const WeightsArrayKind kind)
{
  return name_of(array_names(), kind);
}

const std::string& WeightsSchema::name(const WeightsSubLayer sub)
{
  return name_of(sublayer_names(), sub);
}

const std::string& WeightsSchema::name(const LAYER_TYPE type)
{
  return name_of(type_names(), type);
}

bool WeightsSchema::parse(const std::string& name, WeightsArrayKind& kind)
{
  return value_of(array_names(), name, kind);
}

bool WeightsSchema::parse(const std::string& name, WeightsSubLayer& sub)
{
  return !name.empty() && value_of(sublayer_names(), name, sub);
}

bool WeightsSchema::parse(const std::string& name, LAYER_TYPE& type)
{
  return value_of(type_names(), name, type);
}

LAYER_TYPE WeightsSchema::section_type(const std::string& header)
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

  const auto found = section2type.find(header);

  return found == section2type.end() ? BLANK : found->second;
}

bool WeightsSchema::has_weights(const LAYER_TYPE type)
{
  return !arrays(type).empty();
}

const std::vector<WeightsSubLayer>& WeightsSchema::sublayers(
    const LAYER_TYPE type)
{
  static const std::vector<WeightsSubLayer> none;
  static const std::vector<WeightsSubLayer> rnn{WeightsSubLayer::InputLayer,
                                                WeightsSubLayer::SelfLayer,
                                                WeightsSubLayer::OutputLayer};
  static const std::vector<WeightsSubLayer> gru{
      WeightsSubLayer::InputZLayer, WeightsSubLayer::InputRLayer,
      WeightsSubLayer::InputHLayer, WeightsSubLayer::StateZLayer,
      WeightsSubLayer::StateRLayer, WeightsSubLayer::StateHLayer};
  static const std::vector<WeightsSubLayer> lstm{
      WeightsSubLayer::Wf, WeightsSubLayer::Wi, WeightsSubLayer::Wg,
      WeightsSubLayer::Wo, WeightsSubLayer::Uf, WeightsSubLayer::Ui,
      WeightsSubLayer::Ug, WeightsSubLayer::Uo};
  // the peephole ones come first and the bottleneck layers have no wi, wg and
  // wo ones, see the WeightsLayout
  static const std::vector<WeightsSubLayer> conv_lstm{
      WeightsSubLayer::Vf, WeightsSubLayer::Vi, WeightsSubLayer::Vo,
      WeightsSubLayer::Wf, WeightsSubLayer::Wi, WeightsSubLayer::Wg,
      WeightsSubLayer::Wo, WeightsSubLayer::Uf, WeightsSubLayer::Ui,
      WeightsSubLayer::Ug, WeightsSubLayer::Uo};

  switch (type) {
    case RNN:
    case CRNN:
      return rnn;
    case GRU:
      return gru;
    case LSTM:
      return lstm;
    case CONV_LSTM:
      return conv_lstm;
    default:
      return none;
  }
}

LAYER_TYPE WeightsSchema::sublayer_type(const LAYER_TYPE type)
{
  switch (type) {
    case RNN:
    case GRU:
    case LSTM:
      return CONNECTED;
    case CRNN:
    case CONV_LSTM:
      return CONVOLUTIONAL;
    default:
      return type;
  }
}

const std::vector<WeightsArrayKind>& WeightsSchema::arrays(
    const LAYER_TYPE type)
{
  static const std::vector<WeightsArrayKind> none;
  static const std::vector<WeightsArrayKind> convolutional{
      WeightsArrayKind::Biases, WeightsArrayKind::Scales,
      WeightsArrayKind::RollingMean, WeightsArrayKind::RollingVariance,
      WeightsArrayKind::Weights};
  static const std::vector<WeightsArrayKind> connected{
      WeightsArrayKind::Biases, WeightsArrayKind::Weights,
      WeightsArrayKind::Scales, WeightsArrayKind::RollingMean,
      WeightsArrayKind::RollingVariance};
  static const std::vector<WeightsArrayKind> batchnorm{
      WeightsArrayKind::Biases, WeightsArrayKind::Scales,
      WeightsArrayKind::RollingMean, WeightsArrayKind::RollingVariance};
  static const std::vector<WeightsArrayKind> weights{WeightsArrayKind::Weights};
  static const std::vector<WeightsArrayKind> local{WeightsArrayKind::Biases,
                                                   WeightsArrayKind::Weights};

  switch (sublayer_type(type)) {
    case CONVOLUTIONAL:
      return convolutional;
    case CONNECTED:
      return connected;
    case BATCHNORM:
      return batchnorm;
    case SHORTCUT:
    case IMPLICIT:
      return weights;
    case LOCAL:
      return local;
    default:
      return none;
  }
}

int WeightsSchema::rank(const LAYER_TYPE type, const WeightsSubLayer sub,
                        const WeightsArrayKind kind)
{
  const int array = position(arrays(type), kind);

  if (array < 0) {
    return -1;
  }

  const std::vector<WeightsSubLayer>& subs = sublayers(type);

  if (subs.empty()) {
    return sub == WeightsSubLayer::None ? array : -1;
  }

  const auto found = std::find(subs.begin(), subs.end(), sub);

  if (found == subs.end()) {
    return -1;
  }

  return static_cast<int>(std::distance(subs.begin(), found)) *
             ARRAYS_PER_OWNER +
         array;
}

std::string WeightsSchema::describe(const int layer, const WeightsSubLayer sub,
                                    const WeightsArrayKind kind)
{
  std::string text = "the layer " + std::to_string(layer) + " ";

  if (sub != WeightsSubLayer::None) {
    text += name(sub) + " ";
  }

  return text + name(kind);
}

}  // namespace darknet_adaptor::loaders
