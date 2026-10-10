#include "src/lib/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"

#include <algorithm>
#include <cassert>
#include <cfloat>
#include <cmath>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/IDarknetAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/AvgpoolLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/BatchnormLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ConvLstmLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ConvolutionalLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/CostLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/CrnnLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/CropLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/DropoutLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/GaussianYoloLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/MaxpoolLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/NormalizationLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/RegionLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ReorgLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ReorgOldLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/RouteLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/SamLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ScaleChannelsLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ShortcutLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/UpsampleLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/YoloLayer.h"
#include "src/lib/darknet-adaptor/adaptors/NetworkError.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"
#include "src/log/log.h"

namespace darknet_adaptor::adaptors
{

ANetworkAdaptor::ANetworkAdaptor() : net{create_net()}
{
  assert(net != nullptr);

  // the per network counters darknet keeps behind the pointers, so every copy
  // of the network struct shares them, as make_network() allocated them
  seen(std::vector<uint64_t>(1U, 0U));
  cuda_graph_ready(std::vector<int>(1U, 0));
  badlabels_reject_threshold(std::vector<float>(1U, 0.f));
  delta_rolling_max(std::vector<float>(1U, 0.f));
  delta_rolling_avg(std::vector<float>(1U, 0.f));
  delta_rolling_std(std::vector<float>(1U, 0.f));
  cur_iteration(std::vector<int>(1U, 0));
  total_bbox(std::vector<int>(1U, 0));
  rewritten_bbox(std::vector<int>(1U, 0));
}

bool ANetworkAdaptor::perform([[maybe_unused]] DarknetContextPtr dctx)
{
  LOGE("Do not call this dummy method!");

  return false;
}

ANetworkAdaptorPtr ANetworkAdaptor::create()
{
  return std::make_shared<ANetworkAdaptor>();
}

networkptr ANetworkAdaptor::get()
{
  assert(net != nullptr);

  // An aliasing pointer: it points at the struct and owns the adaptor, which
  // owns the buffers the struct points at.
  return networkptr(shared_from_this(), net.get());
}

void ANetworkAdaptor::link_output()
{
  assert(net != nullptr);

  if (net->n <= 0) {
    net->outputs = 0;
    borrow_output(nullptr);
    return;
  }

  net->outputs = get_network_output_size(*net);
  borrow_output(get_network_output(*net));
}

void ANetworkAdaptor::workspace_bytes(size_t bytes)
{
  assert(net != nullptr);

  // a fresh buffer of the exact size, as the original reallocated it
  vworkspace = std::vector<float>((bytes + sizeof(float) - 1U) / sizeof(float));
  net->workspace = data_or_null(vworkspace);
}

void ANetworkAdaptor::resize(int w, int h)
{
  assert(net != nullptr);

  network& dnet = *net;

  dnet.w = w;
  dnet.h = h;
  int inputs = 0;
  size_t workspace_size = 0;

  CLOGI("Resizing to %d x %d...", w, h);

  for (int i = 0; i < dnet.n; ++i) {
    const ALayerAdaptorPtr adaptor = layer_adaptor(i);
    layer& l = dnet.layers[i];

    CLOGT(" (resize %d: layer = %s)", i, layer_type_2_string(l.type));

    if (!adaptor) {
      throw NetworkError("Cannot resize the network of the borrowed layers");
    }

    switch (l.type) {
      case CONVOLUTIONAL:
        layers::ConvolutionalLayer::resize(*adaptor, w, h);
        break;
      case CRNN:
        layers::CrnnLayer::resize(*adaptor, w, h);
        break;
      case CONV_LSTM:
        layers::ConvLstmLayer::resize(*adaptor, w, h);
        break;
      case CROP:
        layers::CropLayer::resize(*adaptor, w, h);
        break;
      case MAXPOOL:
      case LOCAL_AVGPOOL:
        layers::MaxpoolLayer::resize(*adaptor, w, h);
        break;
      case BATCHNORM:
        layers::BatchnormLayer::resize(*adaptor, w, h);
        break;
      case REGION:
        layers::RegionLayer::resize(*adaptor, w, h);
        break;
      case YOLO:
        layers::YoloLayer::resize(*adaptor, w, h);
        break;
      case GAUSSIAN_YOLO:
        layers::GaussianYoloLayer::resize(*adaptor, w, h);
        break;
      case ROUTE:
        layers::RouteLayer::resize(*adaptor, dnet);
        break;
      case SHORTCUT:
        layers::ShortcutLayer::resize(*adaptor, w, h, dnet);
        break;
      case SCALE_CHANNELS:
        layers::ScaleChannelsLayer::resize(*adaptor, dnet);
        break;
      case SAM:
        layers::SamLayer::resize(*adaptor, w, h);
        break;
      case DROPOUT:
        if (i == 0) {
          throw NetworkError("The first [dropout] layer has no input to pass");
        }
        layers::DropoutLayer::resize(*adaptor, inputs);
        l.out_w = l.w = w;
        l.out_h = l.h = h;
        adaptor->borrow_output(dnet.layers[i - 1].output);
        adaptor->borrow_delta(dnet.layers[i - 1].delta);
        break;
      case UPSAMPLE:
        layers::UpsampleLayer::resize(*adaptor, w, h);
        break;
      case REORG:
        layers::ReorgLayer::resize(*adaptor, w, h);
        break;
      case REORG_OLD:
        layers::ReorgOldLayer::resize(*adaptor, w, h);
        break;
      case AVGPOOL:
        layers::AvgpoolLayer::resize(*adaptor, w, h);
        break;
      case NORMALIZATION:
        layers::NormalizationLayer::resize(*adaptor, w, h);
        break;
      case COST:
        layers::CostLayer::resize(*adaptor, inputs);
        break;
      default:
        CLOGE("Cannot resize this type of layer %s",
              layer_type_2_string(l.type));
        throw NetworkError(std::string{"Cannot resize this type of layer "} +
                           layer_type_2_string(l.type));
    }

    if (l.workspace_size > workspace_size) workspace_size = l.workspace_size;

    // the antialiasing layer passes the output of it's blur sub-layer on, the
    // way the parser linked the next layer (the original passed the output
    // size of the layer itself)
    const layer& passed =
        (l.antialiasing && l.input_layer != nullptr) ? *l.input_layer : l;

    inputs = passed.outputs;
    w = passed.out_w;
    h = passed.out_h;
  }

  workspace_bytes(workspace_size);

  // the last layer output may have moved with it's buffer
  link_output();

  CLOGTM("Done!");
}

void ANetworkAdaptor::set_batch(int b)
{
  assert(net != nullptr);

  net->batch = b;

  for (int i = 0; i < net->n; ++i) {
    net->layers[i].batch = b;
  }

  recalculate_workspace_size();
}

void ANetworkAdaptor::recalculate_workspace_size()
{
  assert(net != nullptr);

  size_t workspace_size = 0;

  for (int i = 0; i < net->n; ++i) {
    layer& l = net->layers[i];

    CLOGT(" %d: layer = %d", i, l.type);

    if (l.type == CONVOLUTIONAL) {
      l.workspace_size = get_convolutional_workspace_size(l);
    } else if (l.type == CONNECTED) {
      l.workspace_size = get_connected_workspace_size(l);
    }

    if (l.workspace_size > workspace_size) workspace_size = l.workspace_size;
  }

  workspace_bytes(workspace_size);
}

void ANetworkAdaptor::fuse_conv_batchnorm()
{
  assert(net != nullptr);

  const auto lrelu = [](float src) {
    const float eps = 0.001f;
    if (src > eps) return src;
    return eps;
  };

  for (int j = 0; j < net->n; ++j) {
    layer* l = &net->layers[j];

    if (l->type == CONVOLUTIONAL) {
      CLOGD(" Merges Convolutional-%d and batch_norm", j);

      if (l->share_layer != NULL) {
        l->batch_normalize = 0;
      }

      if (l->batch_normalize) {
        for (int f = 0; f < l->n; ++f) {
          // the double precision arithmetic of the original
          const double deviation =
              std::sqrt(static_cast<double>(l->rolling_variance[f]) + .00001);

          l->biases[f] = static_cast<float>(
              static_cast<double>(l->biases[f]) -
              static_cast<double>(l->scales[f]) *
                  static_cast<double>(l->rolling_mean[f]) / deviation);

          const double precomputed =
              static_cast<double>(l->scales[f]) / deviation;

          const size_t filter_size =
              static_cast<size_t>(l->size * l->size * l->c / l->groups);

          for (size_t i = 0; i < filter_size; ++i) {
            const size_t w_index = static_cast<size_t>(f) * filter_size + i;

            l->weights[w_index] = static_cast<float>(
                static_cast<double>(l->weights[w_index]) * precomputed);
          }
        }

        const ALayerAdaptorPtr adaptor = layer_adaptor(j);

        if (adaptor) {
          layers::ConvolutionalLayer::free_batchnorm(*adaptor);
        }

        l->batch_normalize = 0;
      } else if (l->share_layer != NULL) {
        // the batch normalization buffers of the shared layer are released
        // with it
        const ALayerAdaptorPtr adaptor = layer_adaptor(j);

        if (adaptor && adaptor->borrows_batchnorm()) {
          layers::ConvolutionalLayer::free_batchnorm(*adaptor);
        }
      }
    } else if (l->type == SHORTCUT && l->weights && l->weights_normalization) {
      if (l->nweights > 0) {
        for (int i = 0; i < l->nweights; ++i) {
          CLOGI(" w = %f,", static_cast<double>(l->weights[i]));
        }
        CLOGI(" l->nweights = %d, j = %d", l->nweights, j);
      }

      // nweights - l.n or l.n*l.c or (l.n*l.c*l.h*l.w)
      const int layer_step = l->nweights / (l->n + 1);  // 1 or l.c or l.c*h*w

      for (int chan = 0; chan < layer_step; ++chan) {
        float sum = 1;
        float max_val = -FLT_MAX;

        if (l->weights_normalization == SOFTMAX_NORMALIZATION) {
          for (int i = 0; i < (l->n + 1); ++i) {
            const int w_index = chan + i * layer_step;
            const float w = l->weights[w_index];
            if (max_val < w) max_val = w;
          }
        }

        const float eps = 0.0001f;
        sum = eps;

        for (int i = 0; i < (l->n + 1); ++i) {
          const int w_index = chan + i * layer_step;
          const float w = l->weights[w_index];
          if (l->weights_normalization == RELU_NORMALIZATION) {
            sum += lrelu(w);
          } else if (l->weights_normalization == SOFTMAX_NORMALIZATION) {
            sum += expf(w - max_val);
          }
        }

        for (int i = 0; i < (l->n + 1); ++i) {
          const int w_index = chan + i * layer_step;
          float w = l->weights[w_index];
          if (l->weights_normalization == RELU_NORMALIZATION) {
            w = lrelu(w) / sum;
          } else if (l->weights_normalization == SOFTMAX_NORMALIZATION) {
            w = expf(w - max_val) / sum;
          }
          l->weights[w_index] = w;
        }
      }

      l->weights_normalization = NO_NORMALIZATION;
    } else {
      CLOGT(" Fusion skip layer type: %d", l->type);
    }
  }
}

void ANetworkAdaptor::calculate_binary_weights()
{
  assert(net != nullptr);

  for (int j = 0; j < net->n; ++j) {
    layer* l = &net->layers[j];

    if (l->type == CONVOLUTIONAL) {
      CLOGT(" Merges Convolutional-%d and batch_norm", j);

      if (l->xnor) {
        CLOGT("\n %d \n", j);

        const ALayerAdaptorPtr adaptor = layer_adaptor(j);

        if (!adaptor) {
          throw NetworkError(
              "Cannot calculate the binary weights of a borrowed layer");
        }

        layers::ConvolutionalLayer::binary_align_weights(*adaptor);

        if (net->layers[j].use_bin_output) {
          l->activation = LINEAR;
        }
      }
    }
  }

  CLOGTM(" calculate_binary_weights Done!");
}

void ANetworkAdaptor::copy_weights_net(const network& net_train,
                                       network& net_map)
{
  // the cuDNN descriptors of the net_map layers the original kept are of
  // the GPU builds only
  for (int k = 0; k < net_train.n; ++k) {
    net_map.layers[k] = net_train.layers[k];
    net_map.layers[k].batch = 1;
    net_map.layers[k].steps = 1;
    net_map.layers[k].train = 0;
  }
}

networkptr ANetworkAdaptor::create_net()
{
  networkptr nnet = std::make_shared<network>();

  std::memset(nnet.get(), 0, sizeof(network));

  return nnet;
}

int ANetworkAdaptor::n() const
{
  assert(net != nullptr);
  return net->n;
}

int ANetworkAdaptor::batch() const
{
  assert(net != nullptr);
  return net->batch;
}

const std::vector<uint64_t>& ANetworkAdaptor::seen() const
{
  assert(net != nullptr);
  return vseen;
}

const std::vector<float>& ANetworkAdaptor::badlabels_reject_threshold() const
{
  assert(net != nullptr);
  return vbadlabels_reject_threshold;
}

const std::vector<float>& ANetworkAdaptor::delta_rolling_max() const
{
  assert(net != nullptr);
  return vdelta_rolling_max;
}

const std::vector<float>& ANetworkAdaptor::delta_rolling_avg() const
{
  assert(net != nullptr);
  return vdelta_rolling_avg;
}

const std::vector<float>& ANetworkAdaptor::delta_rolling_std() const
{
  assert(net != nullptr);
  return vdelta_rolling_std;
}

int ANetworkAdaptor::weights_reject_freq() const
{
  assert(net != nullptr);
  return net->weights_reject_freq;
}

int ANetworkAdaptor::equidistant_point() const
{
  assert(net != nullptr);
  return net->equidistant_point;
}

float ANetworkAdaptor::badlabels_rejection_percentage() const
{
  assert(net != nullptr);
  return net->badlabels_rejection_percentage;
}

float ANetworkAdaptor::num_sigmas_reject_badlabels() const
{
  assert(net != nullptr);
  return net->num_sigmas_reject_badlabels;
}

float ANetworkAdaptor::ema_alpha() const
{
  assert(net != nullptr);
  return net->ema_alpha;
}

const std::vector<int>& ANetworkAdaptor::cur_iteration() const
{
  assert(net != nullptr);
  return vcur_iteration;
}

float ANetworkAdaptor::loss_scale() const
{
  assert(net != nullptr);
  return net->loss_scale;
}

const std::vector<int>& ANetworkAdaptor::t() const
{
  assert(net != nullptr);
  return vt;
}

float ANetworkAdaptor::epoch() const
{
  assert(net != nullptr);
  return net->epoch;
}

int ANetworkAdaptor::subdivisions() const
{
  assert(net != nullptr);
  return net->subdivisions;
}

const std::vector<ALayerAdaptorPtr>& ANetworkAdaptor::layers() const
{
  assert(net != nullptr);
  return mlayers;
}

const std::vector<float>& ANetworkAdaptor::output() const
{
  assert(net != nullptr);
  return voutput;
}

learning_rate_policy ANetworkAdaptor::policy() const
{
  assert(net != nullptr);
  return net->policy;
}

int ANetworkAdaptor::benchmark_layers() const
{
  assert(net != nullptr);
  return net->benchmark_layers;
}

const std::vector<int>& ANetworkAdaptor::total_bbox() const
{
  assert(net != nullptr);
  return vtotal_bbox;
}

const std::vector<int>& ANetworkAdaptor::rewritten_bbox() const
{
  assert(net != nullptr);
  return vrewritten_bbox;
}

float ANetworkAdaptor::learning_rate() const
{
  assert(net != nullptr);
  return net->learning_rate;
}

float ANetworkAdaptor::learning_rate_min() const
{
  assert(net != nullptr);
  return net->learning_rate_min;
}

float ANetworkAdaptor::learning_rate_max() const
{
  assert(net != nullptr);
  return net->learning_rate_max;
}

int ANetworkAdaptor::batches_per_cycle() const
{
  assert(net != nullptr);
  return net->batches_per_cycle;
}

int ANetworkAdaptor::batches_cycle_mult() const
{
  assert(net != nullptr);
  return net->batches_cycle_mult;
}

float ANetworkAdaptor::momentum() const
{
  assert(net != nullptr);
  return net->momentum;
}

float ANetworkAdaptor::decay() const
{
  assert(net != nullptr);
  return net->decay;
}

float ANetworkAdaptor::gamma() const
{
  assert(net != nullptr);
  return net->gamma;
}

float ANetworkAdaptor::scale() const
{
  assert(net != nullptr);
  return net->scale;
}

float ANetworkAdaptor::power() const
{
  assert(net != nullptr);
  return net->power;
}

int ANetworkAdaptor::time_steps() const
{
  assert(net != nullptr);
  return net->time_steps;
}

int ANetworkAdaptor::step() const
{
  assert(net != nullptr);
  return net->step;
}

int ANetworkAdaptor::max_batches() const
{
  assert(net != nullptr);
  return net->max_batches;
}

int ANetworkAdaptor::num_boxes() const
{
  assert(net != nullptr);
  return net->num_boxes;
}

int ANetworkAdaptor::train_images_num() const
{
  assert(net != nullptr);
  return net->train_images_num;
}

const std::vector<float>& ANetworkAdaptor::seq_scales() const
{
  assert(net != nullptr);
  return vseq_scales;
}

const std::vector<float>& ANetworkAdaptor::scales() const
{
  assert(net != nullptr);
  return vscales;
}

const std::vector<int>& ANetworkAdaptor::steps() const
{
  assert(net != nullptr);
  return vsteps;
}

int ANetworkAdaptor::num_steps() const
{
  assert(net != nullptr);
  return net->num_steps;
}

int ANetworkAdaptor::burn_in() const
{
  assert(net != nullptr);
  return net->burn_in;
}

int ANetworkAdaptor::cudnn_half() const
{
  assert(net != nullptr);
  return net->cudnn_half;
}

int ANetworkAdaptor::adam() const
{
  assert(net != nullptr);
  return net->adam;
}

float ANetworkAdaptor::B1() const
{
  assert(net != nullptr);
  return net->B1;
}

float ANetworkAdaptor::B2() const
{
  assert(net != nullptr);
  return net->B2;
}

float ANetworkAdaptor::eps() const
{
  assert(net != nullptr);
  return net->eps;
}

int ANetworkAdaptor::inputs() const
{
  assert(net != nullptr);
  return net->inputs;
}

int ANetworkAdaptor::outputs() const
{
  assert(net != nullptr);
  return net->outputs;
}

int ANetworkAdaptor::truths() const
{
  assert(net != nullptr);
  return net->truths;
}

int ANetworkAdaptor::notruth() const
{
  assert(net != nullptr);
  return net->notruth;
}

int ANetworkAdaptor::c() const
{
  assert(net != nullptr);
  return net->c;
}

int ANetworkAdaptor::h() const
{
  assert(net != nullptr);
  return net->h;
}

int ANetworkAdaptor::w() const
{
  assert(net != nullptr);
  return net->w;
}

int ANetworkAdaptor::max_crop() const
{
  assert(net != nullptr);
  return net->max_crop;
}

int ANetworkAdaptor::min_crop() const
{
  assert(net != nullptr);
  return net->min_crop;
}

float ANetworkAdaptor::max_ratio() const
{
  assert(net != nullptr);
  return net->max_ratio;
}

float ANetworkAdaptor::min_ratio() const
{
  assert(net != nullptr);
  return net->min_ratio;
}

int ANetworkAdaptor::center() const
{
  assert(net != nullptr);
  return net->center;
}

int ANetworkAdaptor::flip() const
{
  assert(net != nullptr);
  return net->flip;
}

// horizontal flip 50% probability augmentaiont for classifier
// training (default = 1)
int ANetworkAdaptor::gaussian_noise() const
{
  assert(net != nullptr);
  return net->gaussian_noise;
}

int ANetworkAdaptor::blur() const
{
  assert(net != nullptr);
  return net->blur;
}

int ANetworkAdaptor::mixup() const
{
  assert(net != nullptr);
  return net->mixup;
}

float ANetworkAdaptor::label_smooth_eps() const
{
  assert(net != nullptr);
  return net->label_smooth_eps;
}

int ANetworkAdaptor::resize_step() const
{
  assert(net != nullptr);
  return net->resize_step;
}

int ANetworkAdaptor::attention() const
{
  assert(net != nullptr);
  return net->attention;
}

int ANetworkAdaptor::adversarial() const
{
  assert(net != nullptr);
  return net->adversarial;
}

float ANetworkAdaptor::adversarial_lr() const
{
  assert(net != nullptr);
  return net->adversarial_lr;
}

float ANetworkAdaptor::max_chart_loss() const
{
  assert(net != nullptr);
  return net->max_chart_loss;
}

int ANetworkAdaptor::letter_box() const
{
  assert(net != nullptr);
  return net->letter_box;
}

int ANetworkAdaptor::mosaic_bound() const
{
  assert(net != nullptr);
  return net->mosaic_bound;
}

int ANetworkAdaptor::contrastive() const
{
  assert(net != nullptr);
  return net->contrastive;
}

int ANetworkAdaptor::contrastive_jit_flip() const
{
  assert(net != nullptr);
  return net->contrastive_jit_flip;
}

int ANetworkAdaptor::contrastive_color() const
{
  assert(net != nullptr);
  return net->contrastive_color;
}

int ANetworkAdaptor::unsupervised() const
{
  assert(net != nullptr);
  return net->unsupervised;
}

float ANetworkAdaptor::angle() const
{
  assert(net != nullptr);
  return net->angle;
}

float ANetworkAdaptor::aspect() const
{
  assert(net != nullptr);
  return net->aspect;
}

float ANetworkAdaptor::exposure() const
{
  assert(net != nullptr);
  return net->exposure;
}

float ANetworkAdaptor::saturation() const
{
  assert(net != nullptr);
  return net->saturation;
}

float ANetworkAdaptor::hue() const
{
  assert(net != nullptr);
  return net->hue;
}

int ANetworkAdaptor::random() const
{
  assert(net != nullptr);
  return net->random;
}

int ANetworkAdaptor::track() const
{
  assert(net != nullptr);
  return net->track;
}

int ANetworkAdaptor::augment_speed() const
{
  assert(net != nullptr);
  return net->augment_speed;
}

int ANetworkAdaptor::sequential_subdivisions() const
{
  assert(net != nullptr);
  return net->sequential_subdivisions;
}

int ANetworkAdaptor::init_sequential_subdivisions() const
{
  assert(net != nullptr);
  return net->init_sequential_subdivisions;
}

int ANetworkAdaptor::current_subdivision() const
{
  assert(net != nullptr);
  return net->current_subdivision;
}

int ANetworkAdaptor::try_fix_nan() const
{
  assert(net != nullptr);
  return net->try_fix_nan;
}

int ANetworkAdaptor::gpu_index() const
{
  assert(net != nullptr);
  return net->gpu_index;
}

tree* ANetworkAdaptor::hierarchy() const
{
  assert(net != nullptr);
  return net->hierarchy;
}

const std::vector<float>& ANetworkAdaptor::input() const
{
  assert(net != nullptr);
  return vinput;
}

const std::vector<float>& ANetworkAdaptor::truth() const
{
  assert(net != nullptr);
  return vtruth;
}

const std::vector<float>& ANetworkAdaptor::delta() const
{
  assert(net != nullptr);
  return vdelta;
}

const std::vector<float>& ANetworkAdaptor::workspace() const
{
  assert(net != nullptr);
  return vworkspace;
}

int ANetworkAdaptor::train() const
{
  assert(net != nullptr);
  return net->train;
}

int ANetworkAdaptor::index() const
{
  assert(net != nullptr);
  return net->index;
}

const std::vector<float>& ANetworkAdaptor::cost() const
{
  assert(net != nullptr);
  return vcost;
}

float ANetworkAdaptor::clip() const
{
  assert(net != nullptr);
  return net->clip;
}

// #ifdef GPU
//  float *ANetworkAdaptor::input_gpu()
//{
//   assert(net != nullptr);
//   return net->input_gpu;
// }

// float *ANetworkAdaptor::truth_gpu()
//{
//  assert(net != nullptr);
//  return net->truth_gpu;
//}

const std::vector<float>& ANetworkAdaptor::delta_gpu() const
{
  assert(net != nullptr);
  return vdelta_gpu;
}

const std::vector<float>& ANetworkAdaptor::output_gpu() const
{
  assert(net != nullptr);
  return voutput_gpu;
}

const std::vector<float>& ANetworkAdaptor::input_state_gpu() const
{
  assert(net != nullptr);
  return vinput_state_gpu;
}

const std::vector<float>& ANetworkAdaptor::input_pinned_cpu() const
{
  assert(net != nullptr);
  return vinput_pinned_cpu;
}

int ANetworkAdaptor::input_pinned_cpu_flag() const
{
  assert(net != nullptr);
  return net->input_pinned_cpu_flag;
}

const ANetworkAdaptor::vvfloats& ANetworkAdaptor::input_gpu() const
{
  assert(net != nullptr);
  return vvinput_gpu;
}

const ANetworkAdaptor::vvfloats& ANetworkAdaptor::truth_gpu() const
{
  assert(net != nullptr);
  return vvtruth_gpu;
}

const ANetworkAdaptor::vvfloats& ANetworkAdaptor::input16_gpu() const
{
  assert(net != nullptr);
  return vvinput16_gpu;
}

const ANetworkAdaptor::vvfloats& ANetworkAdaptor::output16_gpu() const
{
  assert(net != nullptr);
  return vvoutput16_gpu;
}

const std::vector<size_t>& ANetworkAdaptor::max_input16_size() const
{
  assert(net != nullptr);
  return vmax_input16_size;
}

const std::vector<size_t>& ANetworkAdaptor::max_output16_size() const
{
  assert(net != nullptr);
  return vmax_output16_size;
}

int ANetworkAdaptor::wait_stream() const
{
  assert(net != nullptr);
  return net->wait_stream;
}

std::shared_ptr<void> ANetworkAdaptor::cuda_graph() const
{
  assert(net != nullptr);
  return mcuda_graph;
}

std::shared_ptr<void> ANetworkAdaptor::cuda_graph_exec() const
{
  assert(net != nullptr);
  return mcuda_graph_exec;
}

int ANetworkAdaptor::use_cuda_graph() const
{
  assert(net != nullptr);
  return net->use_cuda_graph;
}

const std::vector<int>& ANetworkAdaptor::cuda_graph_ready() const
{
  assert(net != nullptr);
  return vcuda_graph_ready;
}

const std::vector<float>& ANetworkAdaptor::global_delta_gpu() const
{
  assert(net != nullptr);
  return vglobal_delta_gpu;
}

const std::vector<float>& ANetworkAdaptor::state_delta_gpu() const
{
  assert(net != nullptr);
  return vstate_delta_gpu;
}

size_t ANetworkAdaptor::max_delta_gpu_size() const
{
  assert(net != nullptr);
  return net->max_delta_gpu_size;
}

// #endif  // GPU
int ANetworkAdaptor::optimized_memory() const
{
  assert(net != nullptr);
  return net->optimized_memory;
}

int ANetworkAdaptor::dynamic_minibatch() const
{
  assert(net != nullptr);
  return net->dynamic_minibatch;
}

size_t ANetworkAdaptor::workspace_size_limit() const
{
  assert(net != nullptr);
  return net->workspace_size_limit;
}

/**
 * @brief Temporary fields for extended network params
 * previously stored in the obj.data file so now
 * only one cfg file is required.
 */
const std::string& ANetworkAdaptor::names() const
{
  assert(net != nullptr);
  return snames;
}

const std::string& ANetworkAdaptor::lastweights() const
{
  assert(net != nullptr);
  return slastweights;
}

const std::string& ANetworkAdaptor::train_images() const
{
  assert(net != nullptr);
  return strain_images;
}

const std::string& ANetworkAdaptor::valid_images() const
{
  assert(net != nullptr);
  return svalid_images;
}

const std::string& ANetworkAdaptor::backup_directory() const
{
  assert(net != nullptr);
  return sbackup_directory;
}

int ANetworkAdaptor::saveweights() const
{
  assert(net != nullptr);
  return net->saveweights;
}

int ANetworkAdaptor::savelast() const
{
  assert(net != nullptr);
  return net->savelast;
}

int ANetworkAdaptor::savechart() const
{
  assert(net != nullptr);
  return net->savechart;
}

int ANetworkAdaptor::classes() const
{
  assert(net != nullptr);
  return net->classes;
}

void ANetworkAdaptor::h(const int& nval)
{
  assert(net != nullptr);
  net->h = nval;
}

void ANetworkAdaptor::w(const int& nval)
{
  assert(net != nullptr);
  net->w = nval;
}

void ANetworkAdaptor::c(const int& nval)
{
  assert(net != nullptr);
  net->c = nval;
}

void ANetworkAdaptor::batch(const int& nval)
{
  assert(net != nullptr);
  net->batch = nval;
}

void ANetworkAdaptor::seen(const std::vector<uint64_t>& nval)
{
  assert(net != nullptr);
  vseen = nval;
  net->seen = data_or_null(vseen);
}

void ANetworkAdaptor::badlabels_reject_threshold(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vbadlabels_reject_threshold = nval;
  net->badlabels_reject_threshold = data_or_null(vbadlabels_reject_threshold);
}

void ANetworkAdaptor::delta_rolling_max(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vdelta_rolling_max = nval;
  net->delta_rolling_max = data_or_null(vdelta_rolling_max);
}

void ANetworkAdaptor::delta_rolling_avg(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vdelta_rolling_avg = nval;
  net->delta_rolling_avg = data_or_null(vdelta_rolling_avg);
}

void ANetworkAdaptor::delta_rolling_std(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vdelta_rolling_std = nval;
  net->delta_rolling_std = data_or_null(vdelta_rolling_std);
}

void ANetworkAdaptor::weights_reject_freq(const int& nval)
{
  assert(net != nullptr);
  net->weights_reject_freq = nval;
}

void ANetworkAdaptor::equidistant_point(const int& nval)
{
  assert(net != nullptr);
  net->equidistant_point = nval;
}

void ANetworkAdaptor::badlabels_rejection_percentage(const float& nval)
{
  assert(net != nullptr);
  net->badlabels_rejection_percentage = nval;
}

void ANetworkAdaptor::num_sigmas_reject_badlabels(const float& nval)
{
  assert(net != nullptr);
  net->num_sigmas_reject_badlabels = nval;
}

void ANetworkAdaptor::ema_alpha(const float& nval)
{
  assert(net != nullptr);
  net->ema_alpha = nval;
}

void ANetworkAdaptor::cur_iteration(const std::vector<int>& nval)
{
  assert(net != nullptr);
  vcur_iteration = nval;
  net->cur_iteration = data_or_null(vcur_iteration);
}

void ANetworkAdaptor::loss_scale(const float& nval)
{
  assert(net != nullptr);
  net->loss_scale = nval;
}

void ANetworkAdaptor::t(const std::vector<int>& nval)
{
  assert(net != nullptr);
  vt = nval;
  net->t = data_or_null(vt);
}

void ANetworkAdaptor::epoch(const float& nval)
{
  assert(net != nullptr);
  net->epoch = nval;
}

void ANetworkAdaptor::subdivisions(const int& nval)
{
  assert(net != nullptr);
  net->subdivisions = nval;
}

void ANetworkAdaptor::layers(const std::vector<ALayerAdaptorPtr>& nval)
{
  assert(net != nullptr);

  allocate_layers(static_cast<int>(nval.size()));

  for (size_t index = 0U; index < nval.size(); ++index) {
    if (nval[index]) {
      bind_layer(static_cast<int>(index), nval[index]);
    }
  }

  // the share_layer fields of the layers point at the bound structs now
  for (const auto& adaptor : mlayers) {
    if (adaptor) {
      adaptor->relink();
    }
  }
}

void ANetworkAdaptor::allocate_layers(int count)
{
  assert(net != nullptr);
  assert(count >= 0);

  const size_t slots = static_cast<size_t>(count > 0 ? count : 0);

  // one zeroed slot at least, so the layers pointer tells the network apart
  // even without a layer
  mslots = std::make_shared<std::vector<layer>>(std::max<size_t>(slots, 1U));
  std::memset(static_cast<void*>(mslots->data()), 0,
              mslots->size() * sizeof(layer));

  mlayers.assign(slots, ALayerAdaptorPtr{});

  net->layers = mslots->data();
  net->n = static_cast<int>(slots);
}

void ANetworkAdaptor::bind_layer(int index, const ALayerAdaptorPtr& adaptor)
{
  assert(net != nullptr);
  assert(mslots != nullptr);
  assert(index >= 0 && static_cast<size_t>(index) < mlayers.size());
  assert(adaptor != nullptr);

  adaptor->bind(layerptr(mslots, mslots->data() + index));
  mlayers[static_cast<size_t>(index)] = adaptor;
}

ALayerAdaptorPtr ANetworkAdaptor::layer_adaptor(int index) const
{
  if (index < 0 || static_cast<size_t>(index) >= mlayers.size()) {
    return {};
  }

  return mlayers[static_cast<size_t>(index)];
}

void ANetworkAdaptor::borrow_output(float* buffer)
{
  assert(net != nullptr);
  voutput = std::vector<float>{};
  net->output = buffer;
}

void ANetworkAdaptor::output(const std::vector<float>& nval)
{
  assert(net != nullptr);
  voutput = nval;
  net->output = data_or_null(voutput);
}

void ANetworkAdaptor::policy(const learning_rate_policy& nval)
{
  assert(net != nullptr);
  net->policy = nval;
}

void ANetworkAdaptor::benchmark_layers(const int& nval)
{
  assert(net != nullptr);
  net->benchmark_layers = nval;
}

void ANetworkAdaptor::total_bbox(const std::vector<int>& nval)
{
  assert(net != nullptr);
  vtotal_bbox = nval;
  net->total_bbox = data_or_null(vtotal_bbox);
}

void ANetworkAdaptor::rewritten_bbox(const std::vector<int>& nval)
{
  assert(net != nullptr);
  vrewritten_bbox = nval;
  net->rewritten_bbox = data_or_null(vrewritten_bbox);
}

void ANetworkAdaptor::learning_rate(const float& nval)
{
  assert(net != nullptr);
  net->learning_rate = nval;
}

void ANetworkAdaptor::learning_rate_min(const float& nval)
{
  assert(net != nullptr);
  net->learning_rate_min = nval;
}

void ANetworkAdaptor::learning_rate_max(const float& nval)
{
  assert(net != nullptr);
  net->learning_rate_max = nval;
}

void ANetworkAdaptor::batches_per_cycle(const int& nval)
{
  assert(net != nullptr);
  net->batches_per_cycle = nval;
}

void ANetworkAdaptor::batches_cycle_mult(const int& nval)
{
  assert(net != nullptr);
  net->batches_cycle_mult = nval;
}

void ANetworkAdaptor::momentum(const float& nval)
{
  assert(net != nullptr);
  net->momentum = nval;
}

void ANetworkAdaptor::decay(const float& nval)
{
  assert(net != nullptr);
  net->decay = nval;
}

void ANetworkAdaptor::gamma(const float& nval)
{
  assert(net != nullptr);
  net->gamma = nval;
}

void ANetworkAdaptor::scale(const float& nval)
{
  assert(net != nullptr);
  net->scale = nval;
}

void ANetworkAdaptor::power(const float& nval)
{
  assert(net != nullptr);
  net->power = nval;
}

void ANetworkAdaptor::time_steps(const int& nval)
{
  assert(net != nullptr);
  net->time_steps = nval;
}

void ANetworkAdaptor::step(const int& nval)
{
  assert(net != nullptr);
  net->step = nval;
}

void ANetworkAdaptor::max_batches(const int& nval)
{
  assert(net != nullptr);
  net->max_batches = nval;
}

void ANetworkAdaptor::num_boxes(const int& nval)
{
  assert(net != nullptr);
  net->num_boxes = nval;
}

void ANetworkAdaptor::train_images_num(const int& nval)
{
  assert(net != nullptr);
  net->train_images_num = nval;
}

void ANetworkAdaptor::seq_scales(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vseq_scales = nval;
  net->seq_scales = data_or_null(vseq_scales);
}

void ANetworkAdaptor::scales(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vscales = nval;
  net->scales = data_or_null(vscales);
}

void ANetworkAdaptor::steps(const std::vector<int>& nval)
{
  assert(net != nullptr);
  vsteps = nval;
  net->steps = data_or_null(vsteps);
}

void ANetworkAdaptor::num_steps(const int& nval)
{
  assert(net != nullptr);
  net->num_steps = nval;
}

void ANetworkAdaptor::burn_in(const int& nval)
{
  assert(net != nullptr);
  net->burn_in = nval;
}

void ANetworkAdaptor::cudnn_half(const int& nval)
{
  assert(net != nullptr);
  net->cudnn_half = nval;
}

void ANetworkAdaptor::adam(const int& nval)
{
  assert(net != nullptr);
  net->adam = nval;
}

void ANetworkAdaptor::B1(const float& nval)
{
  assert(net != nullptr);
  net->B1 = nval;
}

void ANetworkAdaptor::B2(const float& nval)
{
  assert(net != nullptr);
  net->B2 = nval;
}

void ANetworkAdaptor::eps(const float& nval)
{
  assert(net != nullptr);
  net->eps = nval;
}

void ANetworkAdaptor::inputs(const int& nval)
{
  assert(net != nullptr);
  net->inputs = nval;
}

void ANetworkAdaptor::outputs(const int& nval)
{
  assert(net != nullptr);
  net->outputs = nval;
}

void ANetworkAdaptor::truths(const int& nval)
{
  assert(net != nullptr);
  net->truths = nval;
}

void ANetworkAdaptor::notruth(const int& nval)
{
  assert(net != nullptr);
  net->notruth = nval;
}

void ANetworkAdaptor::max_crop(const int& nval)
{
  assert(net != nullptr);
  net->max_crop = nval;
}

void ANetworkAdaptor::min_crop(const int& nval)
{
  assert(net != nullptr);
  net->min_crop = nval;
}

void ANetworkAdaptor::max_ratio(const float& nval)
{
  assert(net != nullptr);
  net->max_ratio = nval;
}

void ANetworkAdaptor::min_ratio(const float& nval)
{
  assert(net != nullptr);
  net->min_ratio = nval;
}

void ANetworkAdaptor::center(const int& nval)
{
  assert(net != nullptr);
  net->center = nval;
}

void ANetworkAdaptor::flip(const int& nval)
{
  assert(net != nullptr);
  net->flip = nval;
}
// horizontal flip 50% probability augmentaiont
// for classifier training (default = 1)
void ANetworkAdaptor::gaussian_noise(const int& nval)
{
  assert(net != nullptr);
  net->gaussian_noise = nval;
}

void ANetworkAdaptor::blur(const int& nval)
{
  assert(net != nullptr);
  net->blur = nval;
}

void ANetworkAdaptor::mixup(const int& nval)
{
  assert(net != nullptr);
  net->mixup = nval;
}

void ANetworkAdaptor::label_smooth_eps(const float& nval)
{
  assert(net != nullptr);
  net->label_smooth_eps = nval;
}

void ANetworkAdaptor::resize_step(const int& nval)
{
  assert(net != nullptr);
  net->resize_step = nval;
}

void ANetworkAdaptor::attention(const int& nval)
{
  assert(net != nullptr);
  net->attention = nval;
}

void ANetworkAdaptor::adversarial(const int& nval)
{
  assert(net != nullptr);
  net->adversarial = nval;
}

void ANetworkAdaptor::adversarial_lr(const float& nval)
{
  assert(net != nullptr);
  net->adversarial_lr = nval;
}

void ANetworkAdaptor::max_chart_loss(const float& nval)
{
  assert(net != nullptr);
  net->max_chart_loss = nval;
}

void ANetworkAdaptor::letter_box(const int& nval)
{
  assert(net != nullptr);
  net->letter_box = nval;
}

void ANetworkAdaptor::mosaic_bound(const int& nval)
{
  assert(net != nullptr);
  net->mosaic_bound = nval;
}

void ANetworkAdaptor::contrastive(const int& nval)
{
  assert(net != nullptr);
  net->contrastive = nval;
}

void ANetworkAdaptor::contrastive_jit_flip(const int& nval)
{
  assert(net != nullptr);
  net->contrastive_jit_flip = nval;
}

void ANetworkAdaptor::contrastive_color(const int& nval)
{
  assert(net != nullptr);
  net->contrastive_color = nval;
}

void ANetworkAdaptor::unsupervised(const int& nval)
{
  assert(net != nullptr);
  net->unsupervised = nval;
}

void ANetworkAdaptor::angle(const float& nval)
{
  assert(net != nullptr);
  net->angle = nval;
}

void ANetworkAdaptor::aspect(const float& nval)
{
  assert(net != nullptr);
  net->aspect = nval;
}

void ANetworkAdaptor::exposure(const float& nval)
{
  assert(net != nullptr);
  net->exposure = nval;
}

void ANetworkAdaptor::saturation(const float& nval)
{
  assert(net != nullptr);
  net->saturation = nval;
}

void ANetworkAdaptor::hue(const float& nval)
{
  assert(net != nullptr);
  net->hue = nval;
}

void ANetworkAdaptor::random(const int& nval)
{
  assert(net != nullptr);
  net->random = nval;
}

void ANetworkAdaptor::track(const int& nval)
{
  assert(net != nullptr);
  net->track = nval;
}

void ANetworkAdaptor::augment_speed(const int& nval)
{
  assert(net != nullptr);
  net->augment_speed = nval;
}

void ANetworkAdaptor::sequential_subdivisions(const int& nval)
{
  assert(net != nullptr);
  net->sequential_subdivisions = nval;
}

void ANetworkAdaptor::init_sequential_subdivisions(const int& nval)
{
  assert(net != nullptr);
  net->init_sequential_subdivisions = nval;
}

void ANetworkAdaptor::current_subdivision(const int& nval)
{
  assert(net != nullptr);
  net->current_subdivision = nval;
}

void ANetworkAdaptor::try_fix_nan(const int& nval)
{
  assert(net != nullptr);
  net->try_fix_nan = nval;
}

void ANetworkAdaptor::gpu_index(const int& nval)
{
  assert(net != nullptr);
  net->gpu_index = nval;
}

void ANetworkAdaptor::hierarchy(tree* nval)
{
  assert(net != nullptr);
  net->hierarchy = nval;
}

void ANetworkAdaptor::input(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vinput = nval;
  net->input = data_or_null(vinput);
}

void ANetworkAdaptor::truth(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vtruth = nval;
  net->truth = data_or_null(vtruth);
}

void ANetworkAdaptor::delta(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vdelta = nval;
  net->delta = data_or_null(vdelta);
}

void ANetworkAdaptor::workspace(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vworkspace = nval;
  net->workspace = data_or_null(vworkspace);
}

void ANetworkAdaptor::train(const int& nval)
{
  assert(net != nullptr);
  net->train = nval;
}

void ANetworkAdaptor::index(const int& nval)
{
  assert(net != nullptr);
  net->index = nval;
}

void ANetworkAdaptor::cost(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vcost = nval;
  net->cost = data_or_null(vcost);
}

void ANetworkAdaptor::clip(const float& nval)
{
  assert(net != nullptr);
  net->clip = nval;
}

// #ifdef GPU
//  void ANetworkAdaptor::input_gpu(const std::vector<float>& nval)
//{
//   assert(net != nullptr);
//   input_gpu = nval;
// }

// void ANetworkAdaptor::truth_gpu(const std::vector<float>& nval)
//{
//  assert(net != nullptr);
//  truth_gpu = nval;
//}

void ANetworkAdaptor::delta_gpu(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vdelta_gpu = nval;
  net->delta_gpu = data_or_null(vdelta_gpu);
}

void ANetworkAdaptor::output_gpu(const std::vector<float>& nval)
{
  assert(net != nullptr);
  voutput_gpu = nval;
  net->output_gpu = data_or_null(voutput_gpu);
}

void ANetworkAdaptor::input_state_gpu(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vinput_state_gpu = nval;
  net->input_state_gpu = data_or_null(vinput_state_gpu);
}

void ANetworkAdaptor::input_pinned_cpu(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vinput_pinned_cpu = nval;
  net->input_pinned_cpu = data_or_null(vinput_pinned_cpu);
}

void ANetworkAdaptor::input_pinned_cpu_flag(const int& nval)
{
  assert(net != nullptr);
  net->input_pinned_cpu_flag = nval;
}

void ANetworkAdaptor::input_gpu(const vvfloats& nval)
{
  assert(net != nullptr);
  vvinput_gpu = nval;
  place_pointers(vrinput_gpu, vvinput_gpu);
  net->input_gpu = data_or_null(vrinput_gpu);
}

void ANetworkAdaptor::truth_gpu(const vvfloats& nval)
{
  assert(net != nullptr);
  vvtruth_gpu = nval;
  place_pointers(vrtruth_gpu, vvtruth_gpu);
  net->truth_gpu = data_or_null(vrtruth_gpu);
}

void ANetworkAdaptor::input16_gpu(const vvfloats& nval)
{
  assert(net != nullptr);
  vvinput16_gpu = nval;
  place_pointers(vrinput16_gpu, vvinput16_gpu);
  net->input16_gpu = data_or_null(vrinput16_gpu);
}

void ANetworkAdaptor::output16_gpu(const vvfloats& nval)
{
  assert(net != nullptr);
  vvoutput16_gpu = nval;
  place_pointers(vroutput16_gpu, vvoutput16_gpu);
  net->output16_gpu = data_or_null(vroutput16_gpu);
}

void ANetworkAdaptor::max_input16_size(const std::vector<size_t>& nval)
{
  assert(net != nullptr);
  vmax_input16_size = nval;
  net->max_input16_size = data_or_null(vmax_input16_size);
}

void ANetworkAdaptor::max_output16_size(const std::vector<size_t>& nval)
{
  assert(net != nullptr);
  vmax_output16_size = nval;
  net->max_output16_size = data_or_null(vmax_output16_size);
}

void ANetworkAdaptor::wait_stream(const int& nval)
{
  assert(net != nullptr);
  net->wait_stream = nval;
}

void ANetworkAdaptor::cuda_graph(const std::shared_ptr<void>& nval)
{
  assert(net != nullptr);
  mcuda_graph = nval;
  net->cuda_graph = mcuda_graph.get();
}

void ANetworkAdaptor::cuda_graph_exec(const std::shared_ptr<void>& nval)
{
  assert(net != nullptr);
  mcuda_graph_exec = nval;
  net->cuda_graph_exec = mcuda_graph_exec.get();
}

void ANetworkAdaptor::use_cuda_graph(const int& nval)
{
  assert(net != nullptr);
  net->use_cuda_graph = nval;
}

void ANetworkAdaptor::cuda_graph_ready(const std::vector<int>& nval)
{
  assert(net != nullptr);
  vcuda_graph_ready = nval;
  net->cuda_graph_ready = data_or_null(vcuda_graph_ready);
}

void ANetworkAdaptor::global_delta_gpu(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vglobal_delta_gpu = nval;
  net->global_delta_gpu = data_or_null(vglobal_delta_gpu);
}

void ANetworkAdaptor::state_delta_gpu(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vstate_delta_gpu = nval;
  net->state_delta_gpu = data_or_null(vstate_delta_gpu);
}

void ANetworkAdaptor::max_delta_gpu_size(const size_t& nval)
{
  assert(net != nullptr);
  net->max_delta_gpu_size = nval;
}

// #endif  // GPU
void ANetworkAdaptor::optimized_memory(const int& nval)
{
  assert(net != nullptr);
  net->optimized_memory = nval;
}

void ANetworkAdaptor::dynamic_minibatch(const int& nval)
{
  assert(net != nullptr);
  net->dynamic_minibatch = nval;
}

void ANetworkAdaptor::workspace_size_limit(const size_t& nval)
{
  assert(net != nullptr);
  net->workspace_size_limit = nval;
}

void ANetworkAdaptor::names(const std::string& nval)
{
  assert(net != nullptr);
  snames = nval;
  net->names = data_or_null(snames);
}

void ANetworkAdaptor::lastweights(const std::string& nval)
{
  assert(net != nullptr);
  slastweights = nval;
  net->lastweights = data_or_null(slastweights);
}

void ANetworkAdaptor::train_images(const std::string& nval)
{
  assert(net != nullptr);
  strain_images = nval;
  net->train_images = data_or_null(strain_images);
}

void ANetworkAdaptor::valid_images(const std::string& nval)
{
  assert(net != nullptr);
  svalid_images = nval;
  net->valid_images = data_or_null(svalid_images);
}

void ANetworkAdaptor::backup_directory(const std::string& nval)
{
  assert(net != nullptr);
  sbackup_directory = nval;
  net->backup_directory = data_or_null(sbackup_directory);
}

void ANetworkAdaptor::saveweights(const int& nval)
{
  assert(net != nullptr);
  net->saveweights = nval;
}

void ANetworkAdaptor::savelast(const int& nval)
{
  assert(net != nullptr);
  net->savelast = nval;
}

void ANetworkAdaptor::savechart(const int& nval)
{
  assert(net != nullptr);
  net->savechart = nval;
}

void ANetworkAdaptor::classes(const int& nval)
{
  assert(net != nullptr);
  net->classes = nval;
}

}  // namespace darknet_adaptor::adaptors
