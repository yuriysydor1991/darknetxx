#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"

#include <cassert>
#include <cstring>
#include <memory>

#include "src/darknet-adaptor/adaptors/IDarknetAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"
#include "src/log/log.h"

namespace darknet_adaptor::adaptors
{

ALayerAdaptor::ALayerAdaptor() : l{create_layer()} { assert(l != nullptr); }

bool ALayerAdaptor::perform([[maybe_unused]] DarknetContextPtr dctx)
{
  LOGE("Do not call this dummy method!");

  return false;
}

ALayerAdaptorPtr ALayerAdaptor::create()
{
  return std::make_shared<ALayerAdaptor>();
}

layerptr ALayerAdaptor::get() { return l; }

layerptr ALayerAdaptor::create_layer()
{
  layerptr nlayer = std::make_shared<layer>();

  std::memset(nlayer.get(), 0, sizeof(layer));

  return nlayer;
}

LAYER_TYPE ALayerAdaptor::type()
{
  assert(l != nullptr);
  return l->type;
}

ACTIVATION ALayerAdaptor::activation()
{
  assert(l != nullptr);
  return l->activation;
}

ACTIVATION ALayerAdaptor::lstm_activation()
{
  assert(l != nullptr);
  return l->lstm_activation;
}

COST_TYPE ALayerAdaptor::cost_type()
{
  assert(l != nullptr);
  return l->cost_type;
}

//  void (*forward)(struct layer, struct network_state);
//  void (*backward)(struct layer, struct network_state);
//  void (*update)(struct layer, int, float, float, float);
//  void (*forward_gpu)(struct layer, struct network_state);
//  void (*backward_gpu)(struct layer, struct network_state);
//  void (*update_gpu)(struct layer, int, float, float, float, float);

layerptr ALayerAdaptor::share_layer()
{
  assert(l != nullptr);
  return mshare_layer;
}

int ALayerAdaptor::train()
{
  assert(l != nullptr);
  return l->train;
}

int ALayerAdaptor::avgpool()
{
  assert(l != nullptr);
  return l->avgpool;
}

int ALayerAdaptor::batch_normalize()
{
  assert(l != nullptr);
  return l->batch_normalize;
}

int ALayerAdaptor::shortcut()
{
  assert(l != nullptr);
  return l->shortcut;
}

int ALayerAdaptor::batch()
{
  assert(l != nullptr);
  return l->batch;
}

int ALayerAdaptor::dynamic_minibatch()
{
  assert(l != nullptr);
  return l->dynamic_minibatch;
}

int ALayerAdaptor::forced()
{
  assert(l != nullptr);
  return l->forced;
}

int ALayerAdaptor::flipped()
{
  assert(l != nullptr);
  return l->flipped;
}

int ALayerAdaptor::inputs()
{
  assert(l != nullptr);
  return l->inputs;
}

int ALayerAdaptor::outputs()
{
  assert(l != nullptr);
  return l->outputs;
}

float ALayerAdaptor::mean_alpha()
{
  assert(l != nullptr);
  return l->mean_alpha;
}

int ALayerAdaptor::nweights()
{
  assert(l != nullptr);
  return l->nweights;
}

int ALayerAdaptor::nbiases()
{
  assert(l != nullptr);
  return l->nbiases;
}

int ALayerAdaptor::extra()
{
  assert(l != nullptr);
  return l->extra;
}

int ALayerAdaptor::truths()
{
  assert(l != nullptr);
  return l->truths;
}

int ALayerAdaptor::h()
{
  assert(l != nullptr);
  return l->h;
}

int ALayerAdaptor::w()
{
  assert(l != nullptr);
  return l->w;
}

int ALayerAdaptor::c()
{
  assert(l != nullptr);
  return l->c;
}

int ALayerAdaptor::out_h()
{
  assert(l != nullptr);
  return l->out_h;
}

int ALayerAdaptor::out_w()
{
  assert(l != nullptr);
  return l->out_w;
}

int ALayerAdaptor::out_c()
{
  assert(l != nullptr);
  return l->out_c;
}

int ALayerAdaptor::n()
{
  assert(l != nullptr);
  return l->n;
}

int ALayerAdaptor::max_boxes()
{
  assert(l != nullptr);
  return l->max_boxes;
}

int ALayerAdaptor::truth_size()
{
  assert(l != nullptr);
  return l->truth_size;
}

int ALayerAdaptor::groups()
{
  assert(l != nullptr);
  return l->groups;
}

int ALayerAdaptor::group_id()
{
  assert(l != nullptr);
  return l->group_id;
}

int ALayerAdaptor::size()
{
  assert(l != nullptr);
  return l->size;
}

int ALayerAdaptor::side()
{
  assert(l != nullptr);
  return l->side;
}

int ALayerAdaptor::stride()
{
  assert(l != nullptr);
  return l->stride;
}

int ALayerAdaptor::stride_x()
{
  assert(l != nullptr);
  return l->stride_x;
}

int ALayerAdaptor::stride_y()
{
  assert(l != nullptr);
  return l->stride_y;
}

int ALayerAdaptor::dilation()
{
  assert(l != nullptr);
  return l->dilation;
}

int ALayerAdaptor::antialiasing()
{
  assert(l != nullptr);
  return l->antialiasing;
}

int ALayerAdaptor::maxpool_depth()
{
  assert(l != nullptr);
  return l->maxpool_depth;
}

int ALayerAdaptor::maxpool_zero_nonmax()
{
  assert(l != nullptr);
  return l->maxpool_zero_nonmax;
}

int ALayerAdaptor::out_channels()
{
  assert(l != nullptr);
  return l->out_channels;
}

float ALayerAdaptor::reverse()
{
  assert(l != nullptr);
  return l->reverse;
}

int ALayerAdaptor::coordconv()
{
  assert(l != nullptr);
  return l->coordconv;
}

int ALayerAdaptor::flatten()
{
  assert(l != nullptr);
  return l->flatten;
}

int ALayerAdaptor::spatial()
{
  assert(l != nullptr);
  return l->spatial;
}

int ALayerAdaptor::pad()
{
  assert(l != nullptr);
  return l->pad;
}

int ALayerAdaptor::sqrt()
{
  assert(l != nullptr);
  return l->sqrt;
}

int ALayerAdaptor::flip()
{
  assert(l != nullptr);
  return l->flip;
}

int ALayerAdaptor::index()
{
  assert(l != nullptr);
  return l->index;
}

int ALayerAdaptor::scale_wh()
{
  assert(l != nullptr);
  return l->scale_wh;
}

int ALayerAdaptor::binary()
{
  assert(l != nullptr);
  return l->binary;
}

int ALayerAdaptor::xnor()
{
  assert(l != nullptr);
  return l->xnor;
}

int ALayerAdaptor::peephole()
{
  assert(l != nullptr);
  return l->peephole;
}

int ALayerAdaptor::use_bin_output()
{
  assert(l != nullptr);
  return l->use_bin_output;
}

int ALayerAdaptor::keep_delta_gpu()
{
  assert(l != nullptr);
  return l->keep_delta_gpu;
}

int ALayerAdaptor::optimized_memory()
{
  assert(l != nullptr);
  return l->optimized_memory;
}

int ALayerAdaptor::steps()
{
  assert(l != nullptr);
  return l->steps;
}

int ALayerAdaptor::history_size()
{
  assert(l != nullptr);
  return l->history_size;
}

int ALayerAdaptor::bottleneck()
{
  assert(l != nullptr);
  return l->bottleneck;
}

float ALayerAdaptor::time_normalizer()
{
  assert(l != nullptr);
  return l->time_normalizer;
}

int ALayerAdaptor::state_constrain()
{
  assert(l != nullptr);
  return l->state_constrain;
}

int ALayerAdaptor::hidden()
{
  assert(l != nullptr);
  return l->hidden;
}

int ALayerAdaptor::truth()
{
  assert(l != nullptr);
  return l->truth;
}

float ALayerAdaptor::smooth()
{
  assert(l != nullptr);
  return l->smooth;
}

float ALayerAdaptor::dot()
{
  assert(l != nullptr);
  return l->dot;
}

int ALayerAdaptor::deform()
{
  assert(l != nullptr);
  return l->deform;
}

int ALayerAdaptor::grad_centr()
{
  assert(l != nullptr);
  return l->grad_centr;
}

int ALayerAdaptor::sway()
{
  assert(l != nullptr);
  return l->sway;
}

int ALayerAdaptor::rotate()
{
  assert(l != nullptr);
  return l->rotate;
}

int ALayerAdaptor::stretch()
{
  assert(l != nullptr);
  return l->stretch;
}

int ALayerAdaptor::stretch_sway()
{
  assert(l != nullptr);
  return l->stretch_sway;
}

float ALayerAdaptor::angle()
{
  assert(l != nullptr);
  return l->angle;
}

float ALayerAdaptor::jitter()
{
  assert(l != nullptr);
  return l->jitter;
}

float ALayerAdaptor::resize()
{
  assert(l != nullptr);
  return l->resize;
}

float ALayerAdaptor::saturation()
{
  assert(l != nullptr);
  return l->saturation;
}

float ALayerAdaptor::exposure()
{
  assert(l != nullptr);
  return l->exposure;
}

float ALayerAdaptor::shift()
{
  assert(l != nullptr);
  return l->shift;
}

float ALayerAdaptor::ratio()
{
  assert(l != nullptr);
  return l->ratio;
}

float ALayerAdaptor::learning_rate_scale()
{
  assert(l != nullptr);
  return l->learning_rate_scale;
}

float ALayerAdaptor::clip()
{
  assert(l != nullptr);
  return l->clip;
}

int ALayerAdaptor::focal_loss()
{
  assert(l != nullptr);
  return l->focal_loss;
}

std::vector<float>& ALayerAdaptor::classes_multipliers()
{
  assert(l != nullptr);
  return vclasses_multipliers;
}

float ALayerAdaptor::label_smooth_eps()
{
  assert(l != nullptr);
  return l->label_smooth_eps;
}

int ALayerAdaptor::noloss()
{
  assert(l != nullptr);
  return l->noloss;
}

int ALayerAdaptor::softmax()
{
  assert(l != nullptr);
  return l->softmax;
}

int ALayerAdaptor::classes()
{
  assert(l != nullptr);
  return l->classes;
}

int ALayerAdaptor::detection()
{
  assert(l != nullptr);
  return l->detection;
}

int ALayerAdaptor::embedding_layer_id()
{
  assert(l != nullptr);
  return l->embedding_layer_id;
}

std::vector<float>& ALayerAdaptor::embedding_output()
{
  assert(l != nullptr);
  return vembedding_output;
}

int ALayerAdaptor::embedding_size()
{
  assert(l != nullptr);
  return l->embedding_size;
}

float ALayerAdaptor::sim_thresh()
{
  assert(l != nullptr);
  return l->sim_thresh;
}

int ALayerAdaptor::track_history_size()
{
  assert(l != nullptr);
  return l->track_history_size;
}

int ALayerAdaptor::dets_for_track()
{
  assert(l != nullptr);
  return l->dets_for_track;
}

int ALayerAdaptor::dets_for_show()
{
  assert(l != nullptr);
  return l->dets_for_show;
}

float ALayerAdaptor::track_ciou_norm()
{
  assert(l != nullptr);
  return l->track_ciou_norm;
}

int ALayerAdaptor::coords()
{
  assert(l != nullptr);
  return l->coords;
}

int ALayerAdaptor::background()
{
  assert(l != nullptr);
  return l->background;
}

int ALayerAdaptor::rescore()
{
  assert(l != nullptr);
  return l->rescore;
}

int ALayerAdaptor::objectness()
{
  assert(l != nullptr);
  return l->objectness;
}

int ALayerAdaptor::does_cost()
{
  assert(l != nullptr);
  return l->does_cost;
}

int ALayerAdaptor::joint()
{
  assert(l != nullptr);
  return l->joint;
}

int ALayerAdaptor::noadjust()
{
  assert(l != nullptr);
  return l->noadjust;
}

int ALayerAdaptor::reorg()
{
  assert(l != nullptr);
  return l->reorg;
}

int ALayerAdaptor::log()
{
  assert(l != nullptr);
  return l->log;
}

int ALayerAdaptor::tanh()
{
  assert(l != nullptr);
  return l->tanh;
}

std::vector<int>& ALayerAdaptor::mask()
{
  assert(l != nullptr);
  return vmask;
}

int ALayerAdaptor::total()
{
  assert(l != nullptr);
  return l->total;
}

float ALayerAdaptor::bflops()
{
  assert(l != nullptr);
  return l->bflops;
}

int ALayerAdaptor::adam()
{
  assert(l != nullptr);
  return l->adam;
}

float ALayerAdaptor::B1()
{
  assert(l != nullptr);
  return l->B1;
}

float ALayerAdaptor::B2()
{
  assert(l != nullptr);
  return l->B2;
}

float ALayerAdaptor::eps()
{
  assert(l != nullptr);
  return l->eps;
}

int ALayerAdaptor::t()
{
  assert(l != nullptr);
  return l->t;
}

float ALayerAdaptor::alpha()
{
  assert(l != nullptr);
  return l->alpha;
}

float ALayerAdaptor::beta()
{
  assert(l != nullptr);
  return l->beta;
}

float ALayerAdaptor::kappa()
{
  assert(l != nullptr);
  return l->kappa;
}

float ALayerAdaptor::coord_scale()
{
  assert(l != nullptr);
  return l->coord_scale;
}

float ALayerAdaptor::object_scale()
{
  assert(l != nullptr);
  return l->object_scale;
}

float ALayerAdaptor::noobject_scale()
{
  assert(l != nullptr);
  return l->noobject_scale;
}

float ALayerAdaptor::mask_scale()
{
  assert(l != nullptr);
  return l->mask_scale;
}

float ALayerAdaptor::class_scale()
{
  assert(l != nullptr);
  return l->class_scale;
}

int ALayerAdaptor::bias_match()
{
  assert(l != nullptr);
  return l->bias_match;
}

float ALayerAdaptor::random()
{
  assert(l != nullptr);
  return l->random;
}

float ALayerAdaptor::ignore_thresh()
{
  assert(l != nullptr);
  return l->ignore_thresh;
}

float ALayerAdaptor::truth_thresh()
{
  assert(l != nullptr);
  return l->truth_thresh;
}

float ALayerAdaptor::iou_thresh()
{
  assert(l != nullptr);
  return l->iou_thresh;
}

float ALayerAdaptor::thresh()
{
  assert(l != nullptr);
  return l->thresh;
}

float ALayerAdaptor::focus()
{
  assert(l != nullptr);
  return l->focus;
}

int ALayerAdaptor::classfix()
{
  assert(l != nullptr);
  return l->classfix;
}

int ALayerAdaptor::absolute()
{
  assert(l != nullptr);
  return l->absolute;
}

int ALayerAdaptor::assisted_excitation()
{
  assert(l != nullptr);
  return l->assisted_excitation;
}

int ALayerAdaptor::onlyforward()
{
  assert(l != nullptr);
  return l->onlyforward;
}

int ALayerAdaptor::stopbackward()
{
  assert(l != nullptr);
  return l->stopbackward;
}

int ALayerAdaptor::train_only_bn()
{
  assert(l != nullptr);
  return l->train_only_bn;
}

int ALayerAdaptor::dont_update()
{
  assert(l != nullptr);
  return l->dont_update;
}

int ALayerAdaptor::burnin_update()
{
  assert(l != nullptr);
  return l->burnin_update;
}

int ALayerAdaptor::dontload()
{
  assert(l != nullptr);
  return l->dontload;
}

int ALayerAdaptor::dontsave()
{
  assert(l != nullptr);
  return l->dontsave;
}

int ALayerAdaptor::dontloadscales()
{
  assert(l != nullptr);
  return l->dontloadscales;
}

int ALayerAdaptor::numload()
{
  assert(l != nullptr);
  return l->numload;
}

float ALayerAdaptor::temperature()
{
  assert(l != nullptr);
  return l->temperature;
}

float ALayerAdaptor::probability()
{
  assert(l != nullptr);
  return l->probability;
}

float ALayerAdaptor::dropblock_size_rel()
{
  assert(l != nullptr);
  return l->dropblock_size_rel;
}

int ALayerAdaptor::dropblock_size_abs()
{
  assert(l != nullptr);
  return l->dropblock_size_abs;
}

int ALayerAdaptor::dropblock()
{
  assert(l != nullptr);
  return l->dropblock;
}

float ALayerAdaptor::scale()
{
  assert(l != nullptr);
  return l->scale;
}

int ALayerAdaptor::receptive_w()
{
  assert(l != nullptr);
  return l->receptive_w;
}

int ALayerAdaptor::receptive_h()
{
  assert(l != nullptr);
  return l->receptive_h;
}

int ALayerAdaptor::receptive_w_scale()
{
  assert(l != nullptr);
  return l->receptive_w_scale;
}

int ALayerAdaptor::receptive_h_scale()
{
  assert(l != nullptr);
  return l->receptive_h_scale;
}

std::string ALayerAdaptor::cweights()
{
  assert(l != nullptr);
  return scweights;
}

std::vector<int>& ALayerAdaptor::indexes()
{
  assert(l != nullptr);
  return vindexes;
}

std::vector<int>& ALayerAdaptor::input_layers()
{
  assert(l != nullptr);
  return vinput_layers;
}

std::vector<int>& ALayerAdaptor::input_sizes()
{
  assert(l != nullptr);
  return vinput_sizes;
}

ALayerAdaptor::vvfloats ALayerAdaptor::layers_output()
{
  assert(l != nullptr);
  return vlayers_output;
}

ALayerAdaptor::vvfloats ALayerAdaptor::layers_delta()
{
  assert(l != nullptr);
  return vlayers_delta;
}

WEIGHTS_TYPE_T ALayerAdaptor::weights_type()
{
  assert(l != nullptr);
  return l->weights_type;
}

WEIGHTS_NORMALIZATION_T ALayerAdaptor::weights_normalization()
{
  assert(l != nullptr);
  return l->weights_normalization;
}

std::vector<int>& ALayerAdaptor::map()
{
  assert(l != nullptr);
  return vmap;
}

std::vector<int>& ALayerAdaptor::counts()
{
  assert(l != nullptr);
  return vcounts;
}

ALayerAdaptor::vvfloats ALayerAdaptor::sums()
{
  assert(l != nullptr);
  return vsums;
}

std::vector<float>& ALayerAdaptor::rand()
{
  assert(l != nullptr);
  return vrand;
}

std::vector<float>& ALayerAdaptor::cost()
{
  assert(l != nullptr);
  return vcost;
}

std::vector<int>& ALayerAdaptor::labels()
{
  assert(l != nullptr);
  return vlabels;
}

std::vector<int>& ALayerAdaptor::class_ids()
{
  assert(l != nullptr);
  return vclass_ids;
}

int ALayerAdaptor::contrastive_neg_max()
{
  assert(l != nullptr);
  return l->contrastive_neg_max;
}

std::vector<float>& ALayerAdaptor::cos_sim()
{
  assert(l != nullptr);
  return vcos_sim;
}

std::vector<float>& ALayerAdaptor::exp_cos_sim()
{
  assert(l != nullptr);
  return vexp_cos_sim;
}

std::vector<float>& ALayerAdaptor::p_constrastive()
{
  assert(l != nullptr);
  return vp_constrastive;
}

ALayerAdaptor::vcontrastive_params ALayerAdaptor::contrast_p_gpu()
{
  assert(l != nullptr);
  return scontrast_p_gpu;
}

std::vector<float>& ALayerAdaptor::state()
{
  assert(l != nullptr);
  return vstate;
}

std::vector<float>& ALayerAdaptor::prev_state()
{
  assert(l != nullptr);
  return vprev_state;
}

std::vector<float>& ALayerAdaptor::forgot_state()
{
  assert(l != nullptr);
  return vforgot_state;
}

std::vector<float>& ALayerAdaptor::forgot_delta()
{
  assert(l != nullptr);
  return vforgot_delta;
}

std::vector<float>& ALayerAdaptor::state_delta()
{
  assert(l != nullptr);
  return vstate_delta;
}

std::vector<float>& ALayerAdaptor::combine_cpu()
{
  assert(l != nullptr);
  return vcombine_cpu;
}

std::vector<float>& ALayerAdaptor::combine_delta_cpu()
{
  assert(l != nullptr);
  return vcombine_delta_cpu;
}

std::vector<float>& ALayerAdaptor::concat()
{
  assert(l != nullptr);
  return vconcat;
}

std::vector<float>& ALayerAdaptor::concat_delta()
{
  assert(l != nullptr);
  return vconcat_delta;
}

std::vector<float>& ALayerAdaptor::binary_weights()
{
  assert(l != nullptr);
  return vbinary_weights;
}

std::vector<float>& ALayerAdaptor::biases()
{
  assert(l != nullptr);
  return vbiases;
}

std::vector<float>& ALayerAdaptor::bias_updates()
{
  assert(l != nullptr);
  return vbias_updates;
}

std::vector<float>& ALayerAdaptor::scales()
{
  assert(l != nullptr);
  return vscales;
}

std::vector<float>& ALayerAdaptor::scale_updates()
{
  assert(l != nullptr);
  return vscale_updates;
}

std::vector<float>& ALayerAdaptor::weights_ema()
{
  assert(l != nullptr);
  return vweights_ema;
}

std::vector<float>& ALayerAdaptor::biases_ema()
{
  assert(l != nullptr);
  return vbiases_ema;
}

std::vector<float>& ALayerAdaptor::scales_ema()
{
  assert(l != nullptr);
  return vscales_ema;
}

std::vector<float>& ALayerAdaptor::weights()
{
  assert(l != nullptr);
  return vweights;
}

std::vector<float>& ALayerAdaptor::weight_updates()
{
  assert(l != nullptr);
  return vweight_updates;
}

float ALayerAdaptor::scale_x_y()
{
  assert(l != nullptr);
  return l->scale_x_y;
}

int ALayerAdaptor::objectness_smooth()
{
  assert(l != nullptr);
  return l->objectness_smooth;
}

int ALayerAdaptor::new_coords()
{
  assert(l != nullptr);
  return l->new_coords;
}

int ALayerAdaptor::show_details()
{
  assert(l != nullptr);
  return l->show_details;
}

float ALayerAdaptor::max_delta()
{
  assert(l != nullptr);
  return l->max_delta;
}

float ALayerAdaptor::uc_normalizer()
{
  assert(l != nullptr);
  return l->uc_normalizer;
}

float ALayerAdaptor::iou_normalizer()
{
  assert(l != nullptr);
  return l->iou_normalizer;
}

float ALayerAdaptor::obj_normalizer()
{
  assert(l != nullptr);
  return l->obj_normalizer;
}

float ALayerAdaptor::cls_normalizer()
{
  assert(l != nullptr);
  return l->cls_normalizer;
}

float ALayerAdaptor::delta_normalizer()
{
  assert(l != nullptr);
  return l->delta_normalizer;
}

IOU_LOSS ALayerAdaptor::iou_loss()
{
  assert(l != nullptr);
  return l->iou_loss;
}

IOU_LOSS ALayerAdaptor::iou_thresh_kind()
{
  assert(l != nullptr);
  return l->iou_thresh_kind;
}

NMS_KIND ALayerAdaptor::nms_kind()
{
  assert(l != nullptr);
  return l->nms_kind;
}

float ALayerAdaptor::beta_nms()
{
  assert(l != nullptr);
  return l->beta_nms;
}

YOLO_POINT ALayerAdaptor::yolo_point()
{
  assert(l != nullptr);
  return l->yolo_point;
}

std::string ALayerAdaptor::align_bit_weights_gpu()
{
  assert(l != nullptr);
  return salign_bit_weights_gpu;
}

std::vector<float>& ALayerAdaptor::mean_arr_gpu()
{
  assert(l != nullptr);
  return vmean_arr_gpu;
}

std::vector<float>& ALayerAdaptor::align_workspace_gpu()
{
  assert(l != nullptr);
  return valign_workspace_gpu;
}

std::vector<float>& ALayerAdaptor::transposed_align_workspace_gpu()
{
  assert(l != nullptr);
  return vtransposed_align_workspace_gpu;
}

int ALayerAdaptor::align_workspace_size()
{
  assert(l != nullptr);
  return l->align_workspace_size;
}

std::string ALayerAdaptor::align_bit_weights()
{
  assert(l != nullptr);
  return salign_bit_weights;
}

std::vector<float>& ALayerAdaptor::mean_arr()
{
  assert(l != nullptr);
  return vmean_arr;
}

int ALayerAdaptor::align_bit_weights_size()
{
  assert(l != nullptr);
  return l->align_bit_weights_size;
}

int ALayerAdaptor::lda_align()
{
  assert(l != nullptr);
  return l->lda_align;
}

int ALayerAdaptor::new_lda()
{
  assert(l != nullptr);
  return l->new_lda;
}

int ALayerAdaptor::bit_align()
{
  assert(l != nullptr);
  return l->bit_align;
}

std::vector<float>& ALayerAdaptor::col_image()
{
  assert(l != nullptr);
  return vcol_image;
}

std::vector<float>& ALayerAdaptor::delta()
{
  assert(l != nullptr);
  return vdelta;
}

std::vector<float>& ALayerAdaptor::output()
{
  assert(l != nullptr);
  return voutput;
}

std::vector<float>& ALayerAdaptor::activation_input()
{
  assert(l != nullptr);
  return vactivation_input;
}

int ALayerAdaptor::delta_pinned()
{
  assert(l != nullptr);
  return l->delta_pinned;
}

int ALayerAdaptor::output_pinned()
{
  assert(l != nullptr);
  return l->output_pinned;
}

std::vector<float>& ALayerAdaptor::loss()
{
  assert(l != nullptr);
  return vloss;
}

std::vector<float>& ALayerAdaptor::squared()
{
  assert(l != nullptr);
  return vsquared;
}

std::vector<float>& ALayerAdaptor::norms()
{
  assert(l != nullptr);
  return vnorms;
}

std::vector<float>& ALayerAdaptor::spatial_mean()
{
  assert(l != nullptr);
  return vspatial_mean;
}

std::vector<float>& ALayerAdaptor::mean()
{
  assert(l != nullptr);
  return vmean;
}

std::vector<float>& ALayerAdaptor::variance()
{
  assert(l != nullptr);
  return vvariance;
}

std::vector<float>& ALayerAdaptor::mean_delta()
{
  assert(l != nullptr);
  return vmean_delta;
}

std::vector<float>& ALayerAdaptor::variance_delta()
{
  assert(l != nullptr);
  return vvariance_delta;
}

std::vector<float>& ALayerAdaptor::rolling_mean()
{
  assert(l != nullptr);
  return vrolling_mean;
}

std::vector<float>& ALayerAdaptor::rolling_variance()
{
  assert(l != nullptr);
  return vrolling_variance;
}

std::vector<float>& ALayerAdaptor::x()
{
  assert(l != nullptr);
  return vx;
}

std::vector<float>& ALayerAdaptor::x_norm()
{
  assert(l != nullptr);
  return vx_norm;
}

std::vector<float>& ALayerAdaptor::m()
{
  assert(l != nullptr);
  return vm;
}

std::vector<float>& ALayerAdaptor::v()
{
  assert(l != nullptr);
  return vv;
}

std::vector<float>& ALayerAdaptor::bias_m()
{
  assert(l != nullptr);
  return vbias_m;
}

std::vector<float>& ALayerAdaptor::bias_v()
{
  assert(l != nullptr);
  return vbias_v;
}

std::vector<float>& ALayerAdaptor::scale_m()
{
  assert(l != nullptr);
  return vscale_m;
}

std::vector<float>& ALayerAdaptor::scale_v()
{
  assert(l != nullptr);
  return vscale_v;
}

std::vector<float>& ALayerAdaptor::z_cpu()
{
  assert(l != nullptr);
  return vz_cpu;
}

std::vector<float>& ALayerAdaptor::r_cpu()
{
  assert(l != nullptr);
  return vr_cpu;
}

std::vector<float>& ALayerAdaptor::h_cpu()
{
  assert(l != nullptr);
  return vh_cpu;
}

std::vector<float>& ALayerAdaptor::stored_h_cpu()
{
  assert(l != nullptr);
  return vstored_h_cpu;
}

std::vector<float>& ALayerAdaptor::prev_state_cpu()
{
  assert(l != nullptr);
  return vprev_state_cpu;
}

std::vector<float>& ALayerAdaptor::temp_cpu()
{
  assert(l != nullptr);
  return vtemp_cpu;
}

std::vector<float>& ALayerAdaptor::temp2_cpu()
{
  assert(l != nullptr);
  return vtemp2_cpu;
}

std::vector<float>& ALayerAdaptor::temp3_cpu()
{
  assert(l != nullptr);
  return vtemp3_cpu;
}

std::vector<float>& ALayerAdaptor::dh_cpu()
{
  assert(l != nullptr);
  return vdh_cpu;
}

std::vector<float>& ALayerAdaptor::hh_cpu()
{
  assert(l != nullptr);
  return vhh_cpu;
}

std::vector<float>& ALayerAdaptor::prev_cell_cpu()
{
  assert(l != nullptr);
  return vprev_cell_cpu;
}

std::vector<float>& ALayerAdaptor::cell_cpu()
{
  assert(l != nullptr);
  return vcell_cpu;
}

std::vector<float>& ALayerAdaptor::f_cpu()
{
  assert(l != nullptr);
  return vf_cpu;
}

std::vector<float>& ALayerAdaptor::i_cpu()
{
  assert(l != nullptr);
  return vi_cpu;
}

std::vector<float>& ALayerAdaptor::g_cpu()
{
  assert(l != nullptr);
  return vg_cpu;
}

std::vector<float>& ALayerAdaptor::o_cpu()
{
  assert(l != nullptr);
  return vo_cpu;
}

std::vector<float>& ALayerAdaptor::c_cpu()
{
  assert(l != nullptr);
  return vc_cpu;
}

std::vector<float>& ALayerAdaptor::stored_c_cpu()
{
  assert(l != nullptr);
  return vstored_c_cpu;
}

std::vector<float>& ALayerAdaptor::dc_cpu()
{
  assert(l != nullptr);
  return vdc_cpu;
}

std::vector<float>& ALayerAdaptor::binary_input()
{
  assert(l != nullptr);
  return vbinary_input;
}

std::vector<uint32_t> ALayerAdaptor::bin_re_packed_input()
{
  assert(l != nullptr);
  return vbin_re_packed_input;
}

std::string ALayerAdaptor::t_bit_input()
{
  assert(l != nullptr);
  return st_bit_input;
}

layerptr ALayerAdaptor::input_layer()
{
  assert(l != nullptr);
  return minput_layer;
}

layerptr ALayerAdaptor::self_layer()
{
  assert(l != nullptr);
  return mself_layer;
}

layerptr ALayerAdaptor::output_layer()
{
  assert(l != nullptr);
  return moutput_layer;
}

layerptr ALayerAdaptor::reset_layer()
{
  assert(l != nullptr);
  return mreset_layer;
}

layerptr ALayerAdaptor::update_layer()
{
  assert(l != nullptr);
  return mupdate_layer;
}

layerptr ALayerAdaptor::state_layer()
{
  assert(l != nullptr);
  return mstate_layer;
}

layerptr ALayerAdaptor::input_gate_layer()
{
  assert(l != nullptr);
  return minput_gate_layer;
}

layerptr ALayerAdaptor::state_gate_layer()
{
  assert(l != nullptr);
  return mstate_gate_layer;
}

layerptr ALayerAdaptor::input_save_layer()
{
  assert(l != nullptr);
  return minput_save_layer;
}

layerptr ALayerAdaptor::state_save_layer()
{
  assert(l != nullptr);
  return mstate_save_layer;
}

layerptr ALayerAdaptor::input_state_layer()
{
  assert(l != nullptr);
  return minput_state_layer;
}

layerptr ALayerAdaptor::state_state_layer()
{
  assert(l != nullptr);
  return mstate_state_layer;
}

layerptr ALayerAdaptor::input_z_layer()
{
  assert(l != nullptr);
  return minput_z_layer;
}

layerptr ALayerAdaptor::state_z_layer()
{
  assert(l != nullptr);
  return mstate_z_layer;
}

layerptr ALayerAdaptor::input_r_layer()
{
  assert(l != nullptr);
  return minput_r_layer;
}

layerptr ALayerAdaptor::state_r_layer()
{
  assert(l != nullptr);
  return mstate_r_layer;
}

layerptr ALayerAdaptor::input_h_layer()
{
  assert(l != nullptr);
  return minput_h_layer;
}

layerptr ALayerAdaptor::state_h_layer()
{
  assert(l != nullptr);
  return mstate_h_layer;
}

layerptr ALayerAdaptor::wz()
{
  assert(l != nullptr);
  return mwz;
}

layerptr ALayerAdaptor::uz()
{
  assert(l != nullptr);
  return muz;
}

layerptr ALayerAdaptor::wr()
{
  assert(l != nullptr);
  return mwr;
}

layerptr ALayerAdaptor::ur()
{
  assert(l != nullptr);
  return mur;
}

layerptr ALayerAdaptor::wh()
{
  assert(l != nullptr);
  return mwh;
}

layerptr ALayerAdaptor::uh()
{
  assert(l != nullptr);
  return muh;
}

layerptr ALayerAdaptor::uo()
{
  assert(l != nullptr);
  return muo;
}

layerptr ALayerAdaptor::wo()
{
  assert(l != nullptr);
  return mwo;
}

layerptr ALayerAdaptor::vo()
{
  assert(l != nullptr);
  return mvo;
}

layerptr ALayerAdaptor::uf()
{
  assert(l != nullptr);
  return muf;
}

layerptr ALayerAdaptor::wf()
{
  assert(l != nullptr);
  return mwf;
}

layerptr ALayerAdaptor::vf()
{
  assert(l != nullptr);
  return mvf;
}

layerptr ALayerAdaptor::ui()
{
  assert(l != nullptr);
  return mui;
}

layerptr ALayerAdaptor::wi()
{
  assert(l != nullptr);
  return mwi;
}

layerptr ALayerAdaptor::vi()
{
  assert(l != nullptr);
  return mvi;
}

layerptr ALayerAdaptor::ug()
{
  assert(l != nullptr);
  return mug;
}

layerptr ALayerAdaptor::wg()
{
  assert(l != nullptr);
  return mwg;
}

tree* ALayerAdaptor::softmax_tree()
{
  assert(l != nullptr);
  return l->softmax_tree;
}

size_t ALayerAdaptor::workspace_size()
{
  assert(l != nullptr);
  return l->workspace_size;
}

//#ifdef GPU
std::vector<int>& ALayerAdaptor::indexes_gpu()
{
  assert(l != nullptr);
  return vindexes_gpu;
}

int ALayerAdaptor::stream()
{
  assert(l != nullptr);
  return l->stream;
}

int ALayerAdaptor::wait_stream_id()
{
  assert(l != nullptr);
  return l->wait_stream_id;
}

std::vector<float>& ALayerAdaptor::z_gpu()
{
  assert(l != nullptr);
  return vz_gpu;
}

std::vector<float>& ALayerAdaptor::r_gpu()
{
  assert(l != nullptr);
  return vr_gpu;
}

std::vector<float>& ALayerAdaptor::h_gpu()
{
  assert(l != nullptr);
  return vh_gpu;
}

std::vector<float>& ALayerAdaptor::stored_h_gpu()
{
  assert(l != nullptr);
  return vstored_h_gpu;
}

std::vector<float>& ALayerAdaptor::bottelneck_hi_gpu()
{
  assert(l != nullptr);
  return vbottelneck_hi_gpu;
}

std::vector<float>& ALayerAdaptor::bottelneck_delta_gpu()
{
  assert(l != nullptr);
  return vbottelneck_delta_gpu;
}

std::vector<float>& ALayerAdaptor::temp_gpu()
{
  assert(l != nullptr);
  return vtemp_gpu;
}

std::vector<float>& ALayerAdaptor::temp2_gpu()
{
  assert(l != nullptr);
  return vtemp2_gpu;
}

std::vector<float>& ALayerAdaptor::temp3_gpu()
{
  assert(l != nullptr);
  return vtemp3_gpu;
}

std::vector<float>& ALayerAdaptor::dh_gpu()
{
  assert(l != nullptr);
  return vdh_gpu;
}

std::vector<float>& ALayerAdaptor::hh_gpu()
{
  assert(l != nullptr);
  return vhh_gpu;
}

std::vector<float>& ALayerAdaptor::prev_cell_gpu()
{
  assert(l != nullptr);
  return vprev_cell_gpu;
}

std::vector<float>& ALayerAdaptor::prev_state_gpu()
{
  assert(l != nullptr);
  return vprev_state_gpu;
}

std::vector<float>& ALayerAdaptor::last_prev_state_gpu()
{
  assert(l != nullptr);
  return vlast_prev_state_gpu;
}

std::vector<float>& ALayerAdaptor::last_prev_cell_gpu()
{
  assert(l != nullptr);
  return vlast_prev_cell_gpu;
}

std::vector<float>& ALayerAdaptor::cell_gpu()
{
  assert(l != nullptr);
  return vcell_gpu;
}

std::vector<float>& ALayerAdaptor::f_gpu()
{
  assert(l != nullptr);
  return vf_gpu;
}

std::vector<float>& ALayerAdaptor::i_gpu()
{
  assert(l != nullptr);
  return vi_gpu;
}

std::vector<float>& ALayerAdaptor::g_gpu()
{
  assert(l != nullptr);
  return vg_gpu;
}

std::vector<float>& ALayerAdaptor::o_gpu()
{
  assert(l != nullptr);
  return vo_gpu;
}

std::vector<float>& ALayerAdaptor::c_gpu()
{
  assert(l != nullptr);
  return vc_gpu;
}

std::vector<float>& ALayerAdaptor::stored_c_gpu()
{
  assert(l != nullptr);
  return vstored_c_gpu;
}

std::vector<float>& ALayerAdaptor::dc_gpu()
{
  assert(l != nullptr);
  return vdc_gpu;
}

// adam
std::vector<float>& ALayerAdaptor::m_gpu()
{
  assert(l != nullptr);
  return vm_gpu;
}

std::vector<float>& ALayerAdaptor::v_gpu()
{
  assert(l != nullptr);
  return vv_gpu;
}

std::vector<float>& ALayerAdaptor::bias_m_gpu()
{
  assert(l != nullptr);
  return vbias_m_gpu;
}

std::vector<float>& ALayerAdaptor::scale_m_gpu()
{
  assert(l != nullptr);
  return vscale_m_gpu;
}

std::vector<float>& ALayerAdaptor::bias_v_gpu()
{
  assert(l != nullptr);
  return vbias_v_gpu;
}

std::vector<float>& ALayerAdaptor::scale_v_gpu()
{
  assert(l != nullptr);
  return vscale_v_gpu;
}

std::vector<float>& ALayerAdaptor::combine_gpu()
{
  assert(l != nullptr);
  return vcombine_gpu;
}

std::vector<float>& ALayerAdaptor::combine_delta_gpu()
{
  assert(l != nullptr);
  return vcombine_delta_gpu;
}

std::vector<float>& ALayerAdaptor::forgot_state_gpu()
{
  assert(l != nullptr);
  return vforgot_state_gpu;
}

std::vector<float>& ALayerAdaptor::forgot_delta_gpu()
{
  assert(l != nullptr);
  return vforgot_delta_gpu;
}

std::vector<float>& ALayerAdaptor::state_gpu()
{
  assert(l != nullptr);
  return vstate_gpu;
}

std::vector<float>& ALayerAdaptor::state_delta_gpu()
{
  assert(l != nullptr);
  return vstate_delta_gpu;
}

std::vector<float>& ALayerAdaptor::gate_gpu()
{
  assert(l != nullptr);
  return vgate_gpu;
}

std::vector<float>& ALayerAdaptor::gate_delta_gpu()
{
  assert(l != nullptr);
  return vgate_delta_gpu;
}

std::vector<float>& ALayerAdaptor::save_gpu()
{
  assert(l != nullptr);
  return vsave_gpu;
}

std::vector<float>& ALayerAdaptor::save_delta_gpu()
{
  assert(l != nullptr);
  return vsave_delta_gpu;
}

std::vector<float>& ALayerAdaptor::concat_gpu()
{
  assert(l != nullptr);
  return vconcat_gpu;
}

std::vector<float>& ALayerAdaptor::concat_delta_gpu()
{
  assert(l != nullptr);
  return vconcat_delta_gpu;
}

std::vector<float>& ALayerAdaptor::binary_input_gpu()
{
  assert(l != nullptr);
  return vbinary_input_gpu;
}

std::vector<float>& ALayerAdaptor::binary_weights_gpu()
{
  assert(l != nullptr);
  return vbinary_weights_gpu;
}

std::vector<float>& ALayerAdaptor::bin_conv_shortcut_in_gpu()
{
  assert(l != nullptr);
  return vbin_conv_shortcut_in_gpu;
}

std::vector<float>& ALayerAdaptor::bin_conv_shortcut_out_gpu()
{
  assert(l != nullptr);
  return vbin_conv_shortcut_out_gpu;
}

std::vector<float>& ALayerAdaptor::mean_gpu()
{
  assert(l != nullptr);
  return vmean_gpu;
}

std::vector<float>& ALayerAdaptor::variance_gpu()
{
  assert(l != nullptr);
  return vvariance_gpu;
}

std::vector<float>& ALayerAdaptor::m_cbn_avg_gpu()
{
  assert(l != nullptr);
  return vm_cbn_avg_gpu;
}

std::vector<float>& ALayerAdaptor::v_cbn_avg_gpu()
{
  assert(l != nullptr);
  return vv_cbn_avg_gpu;
}

std::vector<float>& ALayerAdaptor::rolling_mean_gpu()
{
  assert(l != nullptr);
  return vrolling_mean_gpu;
}

std::vector<float>& ALayerAdaptor::rolling_variance_gpu()
{
  assert(l != nullptr);
  return vrolling_variance_gpu;
}

std::vector<float>& ALayerAdaptor::variance_delta_gpu()
{
  assert(l != nullptr);
  return vvariance_delta_gpu;
}

std::vector<float>& ALayerAdaptor::mean_delta_gpu()
{
  assert(l != nullptr);
  return vmean_delta_gpu;
}

std::vector<float>& ALayerAdaptor::col_image_gpu()
{
  assert(l != nullptr);
  return vcol_image_gpu;
}

std::vector<float>& ALayerAdaptor::x_gpu()
{
  assert(l != nullptr);
  return vx_gpu;
}

std::vector<float>& ALayerAdaptor::x_norm_gpu()
{
  assert(l != nullptr);
  return vx_norm_gpu;
}

std::vector<float>& ALayerAdaptor::weights_gpu()
{
  assert(l != nullptr);
  return vweights_gpu;
}

std::vector<float>& ALayerAdaptor::weight_updates_gpu()
{
  assert(l != nullptr);
  return vweight_updates_gpu;
}

std::vector<float>& ALayerAdaptor::weight_deform_gpu()
{
  assert(l != nullptr);
  return vweight_deform_gpu;
}

std::vector<float>& ALayerAdaptor::weight_change_gpu()
{
  assert(l != nullptr);
  return vweight_change_gpu;
}

std::vector<float>& ALayerAdaptor::weights_gpu16()
{
  assert(l != nullptr);
  return vweights_gpu16;
}

std::vector<float>& ALayerAdaptor::weight_updates_gpu16()
{
  assert(l != nullptr);
  return vweight_updates_gpu16;
}

std::vector<float>& ALayerAdaptor::biases_gpu()
{
  assert(l != nullptr);
  return vbiases_gpu;
}

std::vector<float>& ALayerAdaptor::bias_updates_gpu()
{
  assert(l != nullptr);
  return vbias_updates_gpu;
}

std::vector<float>& ALayerAdaptor::bias_change_gpu()
{
  assert(l != nullptr);
  return vbias_change_gpu;
}

std::vector<float>& ALayerAdaptor::scales_gpu()
{
  assert(l != nullptr);
  return vscales_gpu;
}

std::vector<float>& ALayerAdaptor::scale_updates_gpu()
{
  assert(l != nullptr);
  return vscale_updates_gpu;
}

std::vector<float>& ALayerAdaptor::scale_change_gpu()
{
  assert(l != nullptr);
  return vscale_change_gpu;
}

std::vector<float>& ALayerAdaptor::input_antialiasing_gpu()
{
  assert(l != nullptr);
  return vinput_antialiasing_gpu;
}

std::vector<float>& ALayerAdaptor::output_gpu()
{
  assert(l != nullptr);
  return voutput_gpu;
}

std::vector<float>& ALayerAdaptor::output_avg_gpu()
{
  assert(l != nullptr);
  return voutput_avg_gpu;
}

std::vector<float>& ALayerAdaptor::activation_input_gpu()
{
  assert(l != nullptr);
  return vactivation_input_gpu;
}

std::vector<float>& ALayerAdaptor::loss_gpu()
{
  assert(l != nullptr);
  return vloss_gpu;
}

std::vector<float>& ALayerAdaptor::delta_gpu()
{
  assert(l != nullptr);
  return vdelta_gpu;
}

std::vector<float>& ALayerAdaptor::cos_sim_gpu()
{
  assert(l != nullptr);
  return vcos_sim_gpu;
}

std::vector<float>& ALayerAdaptor::rand_gpu()
{
  assert(l != nullptr);
  return vrand_gpu;
}

std::vector<float>& ALayerAdaptor::drop_blocks_scale()
{
  assert(l != nullptr);
  return vdrop_blocks_scale;
}

std::vector<float>& ALayerAdaptor::drop_blocks_scale_gpu()
{
  assert(l != nullptr);
  return vdrop_blocks_scale_gpu;
}

std::vector<float>& ALayerAdaptor::squared_gpu()
{
  assert(l != nullptr);
  return vsquared_gpu;
}

std::vector<float>& ALayerAdaptor::norms_gpu()
{
  assert(l != nullptr);
  return vnorms_gpu;
}

std::vector<float>& ALayerAdaptor::gt_gpu()
{
  assert(l != nullptr);
  return vgt_gpu;
}

std::vector<float>& ALayerAdaptor::a_avg_gpu()
{
  assert(l != nullptr);
  return va_avg_gpu;
}

std::vector<int>& ALayerAdaptor::input_sizes_gpu()
{
  assert(l != nullptr);
  return vinput_sizes_gpu;
}

ALayerAdaptor::vvfloats ALayerAdaptor::layers_output_gpu()
{
  assert(l != nullptr);
  return vlayers_output_gpu;
}

ALayerAdaptor::vvfloats ALayerAdaptor::layers_delta_gpu()
{
  assert(l != nullptr);
  return vlayers_delta_gpu;
}

#ifdef CUDNN
cudnnTensorDescriptor_t srcTensorDesc, dstTensorDesc;
cudnnTensorDescriptor_t srcTensorDesc16, dstTensorDesc16;
cudnnTensorDescriptor_t dsrcTensorDesc, ddstTensorDesc;
cudnnTensorDescriptor_t dsrcTensorDesc16, ddstTensorDesc16;
cudnnTensorDescriptor_t normTensorDesc, normDstTensorDesc, normDstTensorDescF16;
cudnnFilterDescriptor_t weightDesc, weightDesc16;
cudnnFilterDescriptor_t dweightDesc, dweightDesc16;
cudnnConvolutionDescriptor_t convDesc;
cudnnConvolutionFwdAlgo_t fw_algo, fw_algo16;
cudnnConvolutionBwdDataAlgo_t bd_algo, bd_algo16;
cudnnConvolutionBwdFilterAlgo_t bf_algo, bf_algo16;
cudnnPoolingDescriptor_t poolingDesc;
#else   // CUDNN

void* ALayerAdaptor::srcTensorDesc()
{
  assert(l != nullptr);
  return l->srcTensorDesc;
}

void* ALayerAdaptor::dstTensorDesc()
{
  assert(l != nullptr);
  return l->dstTensorDesc;
}

void* ALayerAdaptor::srcTensorDesc16()
{
  assert(l != nullptr);
  return l->srcTensorDesc16;
}

void* ALayerAdaptor::dstTensorDesc16()
{
  assert(l != nullptr);
  return l->dstTensorDesc16;
}

void* ALayerAdaptor::dsrcTensorDesc()
{
  assert(l != nullptr);
  return l->dsrcTensorDesc;
}

void* ALayerAdaptor::ddstTensorDesc()
{
  assert(l != nullptr);
  return l->ddstTensorDesc;
}

void* ALayerAdaptor::dsrcTensorDesc16()
{
  assert(l != nullptr);
  return l->dsrcTensorDesc16;
}

void* ALayerAdaptor::ddstTensorDesc16()
{
  assert(l != nullptr);
  return l->ddstTensorDesc16;
}

void* ALayerAdaptor::normTensorDesc()
{
  assert(l != nullptr);
  return l->normTensorDesc;
}

void* ALayerAdaptor::normDstTensorDesc()
{
  assert(l != nullptr);
  return l->normDstTensorDesc;
}

void* ALayerAdaptor::normDstTensorDescF16()
{
  assert(l != nullptr);
  return l->normDstTensorDescF16;
}

void* ALayerAdaptor::weightDesc()
{
  assert(l != nullptr);
  return l->weightDesc;
}

void* ALayerAdaptor::weightDesc16()
{
  assert(l != nullptr);
  return l->weightDesc16;
}

void* ALayerAdaptor::dweightDesc()
{
  assert(l != nullptr);
  return l->dweightDesc;
}

void* ALayerAdaptor::dweightDesc16()
{
  assert(l != nullptr);
  return l->dweightDesc16;
}

void* ALayerAdaptor::convDesc()
{
  assert(l != nullptr);
  return l->convDesc;
}

UNUSED_ENUM_TYPE ALayerAdaptor::fw_algo()
{
  assert(l != nullptr);
  return l->fw_algo;
}

UNUSED_ENUM_TYPE ALayerAdaptor::fw_algo16()
{
  assert(l != nullptr);
  return l->fw_algo16;
}

UNUSED_ENUM_TYPE ALayerAdaptor::bd_algo()
{
  assert(l != nullptr);
  return l->bd_algo;
}

UNUSED_ENUM_TYPE ALayerAdaptor::bd_algo16()
{
  assert(l != nullptr);
  return l->bd_algo16;
}

UNUSED_ENUM_TYPE ALayerAdaptor::bf_algo()
{
  assert(l != nullptr);
  return l->bf_algo;
}

UNUSED_ENUM_TYPE ALayerAdaptor::bf_algo16()
{
  assert(l != nullptr);
  return l->bf_algo16;
}

void* ALayerAdaptor::poolingDesc()
{
  assert(l != nullptr);
  return l->poolingDesc;
}
#endif  // CUDNN

/*
 * Setters.
 */

void ALayerAdaptor::type(const LAYER_TYPE& nval)
{
  assert(l != nullptr);
  l->type = nval;
}

void ALayerAdaptor::activation(const ACTIVATION& nval)
{
  assert(l != nullptr);
  l->activation = nval;
}

void ALayerAdaptor::lstm_activation(const ACTIVATION& nval)
{
  assert(l != nullptr);
  l->lstm_activation = nval;
}

void ALayerAdaptor::cost_type(const COST_TYPE& nval)
{
  assert(l != nullptr);
  l->cost_type = nval;
}

void ALayerAdaptor::share_layer(const layerptr& nval)
{
  assert(l != nullptr);

  mshare_layer = nval;

  l->share_layer = mshare_layer.get();
}

void ALayerAdaptor::share_layer(layerptr&& nval)
{
  assert(l != nullptr);

  mshare_layer = std::move(nval);

  l->share_layer = mshare_layer.get();
}

void ALayerAdaptor::train(const int& nval)
{
  assert(l != nullptr);
  l->train = nval;
}

void ALayerAdaptor::avgpool(const int& nval)
{
  assert(l != nullptr);
  l->avgpool = nval;
}

void ALayerAdaptor::batch_normalize(const int& nval)
{
  assert(l != nullptr);
  l->batch_normalize = nval;
}

void ALayerAdaptor::shortcut(const int& nval)
{
  assert(l != nullptr);
  l->shortcut = nval;
}

void ALayerAdaptor::batch(const int& nval)
{
  assert(l != nullptr);
  l->batch = nval;
}

void ALayerAdaptor::dynamic_minibatch(const int& nval)
{
  assert(l != nullptr);
  l->dynamic_minibatch = nval;
}

void ALayerAdaptor::forced(const int& nval)
{
  assert(l != nullptr);
  l->forced = nval;
}

void ALayerAdaptor::flipped(const int& nval)
{
  assert(l != nullptr);
  l->flipped = nval;
}

void ALayerAdaptor::inputs(const int& nval)
{
  assert(l != nullptr);
  l->inputs = nval;
}

void ALayerAdaptor::outputs(const int& nval)
{
  assert(l != nullptr);
  l->outputs = nval;
}

void ALayerAdaptor::mean_alpha(const float& nval)
{
  assert(l != nullptr);
  l->mean_alpha = nval;
}

void ALayerAdaptor::nweights(const int& nval)
{
  assert(l != nullptr);
  l->nweights = nval;
}

void ALayerAdaptor::nbiases(const int& nval)
{
  assert(l != nullptr);
  l->nbiases = nval;
}

void ALayerAdaptor::extra(const int& nval)
{
  assert(l != nullptr);
  l->extra = nval;
}

void ALayerAdaptor::truths(const int& nval)
{
  assert(l != nullptr);
  l->truths = nval;
}

void ALayerAdaptor::h(const int& nval)
{
  assert(l != nullptr);
  l->h = nval;
}

void ALayerAdaptor::w(const int& nval)
{
  assert(l != nullptr);
  l->w = nval;
}

void ALayerAdaptor::c(const int& nval)
{
  assert(l != nullptr);
  l->c = nval;
}

void ALayerAdaptor::out_h(const int& nval)
{
  assert(l != nullptr);
  l->out_h = nval;
}

void ALayerAdaptor::out_w(const int& nval)
{
  assert(l != nullptr);
  l->out_w = nval;
}

void ALayerAdaptor::out_c(const int& nval)
{
  assert(l != nullptr);
  l->out_c = nval;
}

void ALayerAdaptor::n(const int& nval)
{
  assert(l != nullptr);
  l->n = nval;
}

void ALayerAdaptor::max_boxes(const int& nval)
{
  assert(l != nullptr);
  l->max_boxes = nval;
}

void ALayerAdaptor::truth_size(const int& nval)
{
  assert(l != nullptr);
  l->truth_size = nval;
}

void ALayerAdaptor::groups(const int& nval)
{
  assert(l != nullptr);
  l->groups = nval;
}

void ALayerAdaptor::group_id(const int& nval)
{
  assert(l != nullptr);
  l->group_id = nval;
}

void ALayerAdaptor::size(const int& nval)
{
  assert(l != nullptr);
  l->size = nval;
}

void ALayerAdaptor::side(const int& nval)
{
  assert(l != nullptr);
  l->side = nval;
}

void ALayerAdaptor::stride(const int& nval)
{
  assert(l != nullptr);
  l->stride = nval;
}

void ALayerAdaptor::stride_x(const int& nval)
{
  assert(l != nullptr);
  l->stride_x = nval;
}

void ALayerAdaptor::stride_y(const int& nval)
{
  assert(l != nullptr);
  l->stride_y = nval;
}

void ALayerAdaptor::dilation(const int& nval)
{
  assert(l != nullptr);
  l->dilation = nval;
}

void ALayerAdaptor::antialiasing(const int& nval)
{
  assert(l != nullptr);
  l->antialiasing = nval;
}

void ALayerAdaptor::maxpool_depth(const int& nval)
{
  assert(l != nullptr);
  l->maxpool_depth = nval;
}

void ALayerAdaptor::maxpool_zero_nonmax(const int& nval)
{
  assert(l != nullptr);
  l->maxpool_zero_nonmax = nval;
}

void ALayerAdaptor::out_channels(const int& nval)
{
  assert(l != nullptr);
  l->out_channels = nval;
}

void ALayerAdaptor::reverse(const float& nval)
{
  assert(l != nullptr);
  l->reverse = nval;
}

void ALayerAdaptor::coordconv(const int& nval)
{
  assert(l != nullptr);
  l->coordconv = nval;
}

void ALayerAdaptor::flatten(const int& nval)
{
  assert(l != nullptr);
  l->flatten = nval;
}

void ALayerAdaptor::spatial(const int& nval)
{
  assert(l != nullptr);
  l->spatial = nval;
}

void ALayerAdaptor::pad(const int& nval)
{
  assert(l != nullptr);
  l->pad = nval;
}

void ALayerAdaptor::sqrt(const int& nval)
{
  assert(l != nullptr);
  l->sqrt = nval;
}

void ALayerAdaptor::flip(const int& nval)
{
  assert(l != nullptr);
  l->flip = nval;
}

void ALayerAdaptor::index(const int& nval)
{
  assert(l != nullptr);
  l->index = nval;
}

void ALayerAdaptor::scale_wh(const int& nval)
{
  assert(l != nullptr);
  l->scale_wh = nval;
}

void ALayerAdaptor::binary(const int& nval)
{
  assert(l != nullptr);
  l->binary = nval;
}

void ALayerAdaptor::xnor(const int& nval)
{
  assert(l != nullptr);
  l->xnor = nval;
}

void ALayerAdaptor::peephole(const int& nval)
{
  assert(l != nullptr);
  l->peephole = nval;
}

void ALayerAdaptor::use_bin_output(const int& nval)
{
  assert(l != nullptr);
  l->use_bin_output = nval;
}

void ALayerAdaptor::keep_delta_gpu(const int& nval)
{
  assert(l != nullptr);
  l->keep_delta_gpu = nval;
}

void ALayerAdaptor::optimized_memory(const int& nval)
{
  assert(l != nullptr);
  l->optimized_memory = nval;
}

void ALayerAdaptor::steps(const int& nval)
{
  assert(l != nullptr);
  l->steps = nval;
}

void ALayerAdaptor::history_size(const int& nval)
{
  assert(l != nullptr);
  l->history_size = nval;
}

void ALayerAdaptor::bottleneck(const int& nval)
{
  assert(l != nullptr);
  l->bottleneck = nval;
}

void ALayerAdaptor::time_normalizer(const float& nval)
{
  assert(l != nullptr);
  l->time_normalizer = nval;
}

void ALayerAdaptor::state_constrain(const int& nval)
{
  assert(l != nullptr);
  l->state_constrain = nval;
}

void ALayerAdaptor::hidden(const int& nval)
{
  assert(l != nullptr);
  l->hidden = nval;
}

void ALayerAdaptor::truth(const int& nval)
{
  assert(l != nullptr);
  l->truth = nval;
}

void ALayerAdaptor::smooth(const float& nval)
{
  assert(l != nullptr);
  l->smooth = nval;
}

void ALayerAdaptor::dot(const float& nval)
{
  assert(l != nullptr);
  l->dot = nval;
}

void ALayerAdaptor::deform(const int& nval)
{
  assert(l != nullptr);
  l->deform = nval;
}

void ALayerAdaptor::grad_centr(const int& nval)
{
  assert(l != nullptr);
  l->grad_centr = nval;
}

void ALayerAdaptor::sway(const int& nval)
{
  assert(l != nullptr);
  l->sway = nval;
}

void ALayerAdaptor::rotate(const int& nval)
{
  assert(l != nullptr);
  l->rotate = nval;
}

void ALayerAdaptor::stretch(const int& nval)
{
  assert(l != nullptr);
  l->stretch = nval;
}

void ALayerAdaptor::stretch_sway(const int& nval)
{
  assert(l != nullptr);
  l->stretch_sway = nval;
}

void ALayerAdaptor::angle(const float& nval)
{
  assert(l != nullptr);
  l->angle = nval;
}

void ALayerAdaptor::jitter(const float& nval)
{
  assert(l != nullptr);
  l->jitter = nval;
}

void ALayerAdaptor::resize(const float& nval)
{
  assert(l != nullptr);
  l->resize = nval;
}

void ALayerAdaptor::saturation(const float& nval)
{
  assert(l != nullptr);
  l->saturation = nval;
}

void ALayerAdaptor::exposure(const float& nval)
{
  assert(l != nullptr);
  l->exposure = nval;
}

void ALayerAdaptor::shift(const float& nval)
{
  assert(l != nullptr);
  l->shift = nval;
}

void ALayerAdaptor::ratio(const float& nval)
{
  assert(l != nullptr);
  l->ratio = nval;
}

void ALayerAdaptor::learning_rate_scale(const float& nval)
{
  assert(l != nullptr);
  l->learning_rate_scale = nval;
}

void ALayerAdaptor::clip(const float& nval)
{
  assert(l != nullptr);
  l->clip = nval;
}

void ALayerAdaptor::focal_loss(const int& nval)
{
  assert(l != nullptr);
  l->focal_loss = nval;
}

void ALayerAdaptor::classes_multipliers(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vclasses_multipliers = nval;
  l->classes_multipliers = vclasses_multipliers.data();
}

void ALayerAdaptor::classes_multipliers(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vclasses_multipliers = std::move(nval);
  l->classes_multipliers = vclasses_multipliers.data();
}

void ALayerAdaptor::label_smooth_eps(const float& nval)
{
  assert(l != nullptr);
  l->label_smooth_eps = nval;
}

void ALayerAdaptor::noloss(const int& nval)
{
  assert(l != nullptr);
  l->noloss = nval;
}

void ALayerAdaptor::softmax(const int& nval)
{
  assert(l != nullptr);
  l->softmax = nval;
}

void ALayerAdaptor::classes(const int& nval)
{
  assert(l != nullptr);
  l->classes = nval;
}

void ALayerAdaptor::detection(const int& nval)
{
  assert(l != nullptr);
  l->detection = nval;
}

void ALayerAdaptor::embedding_layer_id(const int& nval)
{
  assert(l != nullptr);
  l->embedding_layer_id = nval;
}

void ALayerAdaptor::embedding_output(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vembedding_output = nval;
  l->embedding_output = vembedding_output.data();
}

void ALayerAdaptor::embedding_output(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vembedding_output = std::move(nval);
  l->embedding_output = vembedding_output.data();
}

void ALayerAdaptor::embedding_size(const int& nval)
{
  assert(l != nullptr);
  l->embedding_size = nval;
}

void ALayerAdaptor::sim_thresh(const float& nval)
{
  assert(l != nullptr);
  l->sim_thresh = nval;
}

void ALayerAdaptor::track_history_size(const int& nval)
{
  assert(l != nullptr);
  l->track_history_size = nval;
}

void ALayerAdaptor::dets_for_track(const int& nval)
{
  assert(l != nullptr);
  l->dets_for_track = nval;
}

void ALayerAdaptor::dets_for_show(const int& nval)
{
  assert(l != nullptr);
  l->dets_for_show = nval;
}

void ALayerAdaptor::track_ciou_norm(const float& nval)
{
  assert(l != nullptr);
  l->track_ciou_norm = nval;
}

void ALayerAdaptor::coords(const int& nval)
{
  assert(l != nullptr);
  l->coords = nval;
}

void ALayerAdaptor::background(const int& nval)
{
  assert(l != nullptr);
  l->background = nval;
}

void ALayerAdaptor::rescore(const int& nval)
{
  assert(l != nullptr);
  l->rescore = nval;
}

void ALayerAdaptor::objectness(const int& nval)
{
  assert(l != nullptr);
  l->objectness = nval;
}

void ALayerAdaptor::does_cost(const int& nval)
{
  assert(l != nullptr);
  l->does_cost = nval;
}

void ALayerAdaptor::joint(const int& nval)
{
  assert(l != nullptr);
  l->joint = nval;
}

void ALayerAdaptor::noadjust(const int& nval)
{
  assert(l != nullptr);
  l->noadjust = nval;
}

void ALayerAdaptor::reorg(const int& nval)
{
  assert(l != nullptr);
  l->reorg = nval;
}

void ALayerAdaptor::log(const int& nval)
{
  assert(l != nullptr);
  l->log = nval;
}

void ALayerAdaptor::tanh(const int& nval)
{
  assert(l != nullptr);
  l->tanh = nval;
}

void ALayerAdaptor::mask(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vmask = nval;
  l->mask = vmask.data();
}

void ALayerAdaptor::mask(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vmask = std::move(nval);
  l->mask = vmask.data();
}

void ALayerAdaptor::total(const int& nval)
{
  assert(l != nullptr);
  l->total = nval;
}

void ALayerAdaptor::bflops(const float& nval)
{
  assert(l != nullptr);
  l->bflops = nval;
}

void ALayerAdaptor::adam(const int& nval)
{
  assert(l != nullptr);
  l->adam = nval;
}

void ALayerAdaptor::B1(const float& nval)
{
  assert(l != nullptr);
  l->B1 = nval;
}

void ALayerAdaptor::B2(const float& nval)
{
  assert(l != nullptr);
  l->B2 = nval;
}

void ALayerAdaptor::eps(const float& nval)
{
  assert(l != nullptr);
  l->eps = nval;
}

void ALayerAdaptor::t(const int& nval)
{
  assert(l != nullptr);
  l->t = nval;
}

void ALayerAdaptor::alpha(const float& nval)
{
  assert(l != nullptr);
  l->alpha = nval;
}

void ALayerAdaptor::beta(const float& nval)
{
  assert(l != nullptr);
  l->beta = nval;
}

void ALayerAdaptor::kappa(const float& nval)
{
  assert(l != nullptr);
  l->kappa = nval;
}

void ALayerAdaptor::coord_scale(const float& nval)
{
  assert(l != nullptr);
  l->coord_scale = nval;
}

void ALayerAdaptor::object_scale(const float& nval)
{
  assert(l != nullptr);
  l->object_scale = nval;
}

void ALayerAdaptor::noobject_scale(const float& nval)
{
  assert(l != nullptr);
  l->noobject_scale = nval;
}

void ALayerAdaptor::mask_scale(const float& nval)
{
  assert(l != nullptr);
  l->mask_scale = nval;
}

void ALayerAdaptor::class_scale(const float& nval)
{
  assert(l != nullptr);
  l->class_scale = nval;
}

void ALayerAdaptor::bias_match(const int& nval)
{
  assert(l != nullptr);
  l->bias_match = nval;
}

void ALayerAdaptor::random(const float& nval)
{
  assert(l != nullptr);
  l->random = nval;
}

void ALayerAdaptor::ignore_thresh(const float& nval)
{
  assert(l != nullptr);
  l->ignore_thresh = nval;
}

void ALayerAdaptor::truth_thresh(const float& nval)
{
  assert(l != nullptr);
  l->truth_thresh = nval;
}

void ALayerAdaptor::iou_thresh(const float& nval)
{
  assert(l != nullptr);
  l->iou_thresh = nval;
}

void ALayerAdaptor::thresh(const float& nval)
{
  assert(l != nullptr);
  l->thresh = nval;
}

void ALayerAdaptor::focus(const float& nval)
{
  assert(l != nullptr);
  l->focus = nval;
}

void ALayerAdaptor::classfix(const int& nval)
{
  assert(l != nullptr);
  l->classfix = nval;
}

void ALayerAdaptor::absolute(const int& nval)
{
  assert(l != nullptr);
  l->absolute = nval;
}

void ALayerAdaptor::assisted_excitation(const int& nval)
{
  assert(l != nullptr);
  l->assisted_excitation = nval;
}

void ALayerAdaptor::onlyforward(const int& nval)
{
  assert(l != nullptr);
  l->onlyforward = nval;
}

void ALayerAdaptor::stopbackward(const int& nval)
{
  assert(l != nullptr);
  l->stopbackward = nval;
}

void ALayerAdaptor::train_only_bn(const int& nval)
{
  assert(l != nullptr);
  l->train_only_bn = nval;
}

void ALayerAdaptor::dont_update(const int& nval)
{
  assert(l != nullptr);
  l->dont_update = nval;
}

void ALayerAdaptor::burnin_update(const int& nval)
{
  assert(l != nullptr);
  l->burnin_update = nval;
}

void ALayerAdaptor::dontload(const int& nval)
{
  assert(l != nullptr);
  l->dontload = nval;
}

void ALayerAdaptor::dontsave(const int& nval)
{
  assert(l != nullptr);
  l->dontsave = nval;
}

void ALayerAdaptor::dontloadscales(const int& nval)
{
  assert(l != nullptr);
  l->dontloadscales = nval;
}

void ALayerAdaptor::numload(const int& nval)
{
  assert(l != nullptr);
  l->numload = nval;
}

void ALayerAdaptor::temperature(const float& nval)
{
  assert(l != nullptr);
  l->temperature = nval;
}

void ALayerAdaptor::probability(const float& nval)
{
  assert(l != nullptr);
  l->probability = nval;
}

void ALayerAdaptor::dropblock_size_rel(const float& nval)
{
  assert(l != nullptr);
  l->dropblock_size_rel = nval;
}

void ALayerAdaptor::dropblock_size_abs(const int& nval)
{
  assert(l != nullptr);
  l->dropblock_size_abs = nval;
}

void ALayerAdaptor::dropblock(const int& nval)
{
  assert(l != nullptr);
  l->dropblock = nval;
}

void ALayerAdaptor::scale(const float& nval)
{
  assert(l != nullptr);
  l->scale = nval;
}

void ALayerAdaptor::receptive_w(const int& nval)
{
  assert(l != nullptr);
  l->receptive_w = nval;
}

void ALayerAdaptor::receptive_h(const int& nval)
{
  assert(l != nullptr);
  l->receptive_h = nval;
}

void ALayerAdaptor::receptive_w_scale(const int& nval)
{
  assert(l != nullptr);
  l->receptive_w_scale = nval;
}

void ALayerAdaptor::receptive_h_scale(const int& nval)
{
  assert(l != nullptr);
  l->receptive_h_scale = nval;
}

void ALayerAdaptor::cweights(const std::string& nval)
{
  assert(l != nullptr);
  scweights = nval;
  l->cweights = scweights.data();
}

void ALayerAdaptor::cweights(std::string&& nval)
{
  assert(l != nullptr);
  scweights = std::move(nval);
  l->cweights = scweights.data();
}

void ALayerAdaptor::indexes(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vindexes = nval;
  l->indexes = vindexes.data();
}

void ALayerAdaptor::indexes(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vindexes = std::move(nval);
  l->indexes = vindexes.data();
}

void ALayerAdaptor::input_layers(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vinput_layers = nval;
  l->input_layers = vinput_layers.data();
}

void ALayerAdaptor::input_layers(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vinput_layers = std::move(nval);
  l->input_layers = vinput_layers.data();
}

void ALayerAdaptor::input_sizes(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vinput_sizes = nval;
  l->input_sizes = vinput_sizes.data();
}

void ALayerAdaptor::input_sizes(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vinput_sizes = std::move(nval);
  l->input_sizes = vinput_sizes.data();
}

void ALayerAdaptor::layers_output(const vvfloats& nval)
{
  assert(l != nullptr);
  vlayers_output = nval;

  place_pointers(vrlayers_output, vlayers_output);

  l->layers_output = vrlayers_output.data();
}

void ALayerAdaptor::layers_delta(const vvfloats& nval)
{
  assert(l != nullptr);
  vlayers_delta = nval;

  place_pointers(vrlayers_delta, vlayers_delta);

  l->layers_delta = vrlayers_delta.data();
}

void ALayerAdaptor::weights_type(const WEIGHTS_TYPE_T& nval)
{
  assert(l != nullptr);
  l->weights_type = nval;
}

void ALayerAdaptor::weights_type(WEIGHTS_TYPE_T&& nval)
{
  assert(l != nullptr);
  l->weights_type = nval;
}

void ALayerAdaptor::weights_normalization(const WEIGHTS_NORMALIZATION_T& nval)
{
  assert(l != nullptr);
  l->weights_normalization = nval;
}

void ALayerAdaptor::weights_normalization(WEIGHTS_NORMALIZATION_T&& nval)
{
  assert(l != nullptr);
  l->weights_normalization = nval;
}

void ALayerAdaptor::map(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vmap = nval;
  l->map = vmap.data();
}

void ALayerAdaptor::map(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vmap = std::move(nval);
  l->map = vmap.data();
}

void ALayerAdaptor::counts(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vcounts = nval;
  l->counts = vcounts.data();
}

void ALayerAdaptor::counts(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vcounts = std::move(nval);
  l->counts = vcounts.data();
}

void ALayerAdaptor::sums(const vvfloats& nval)
{
  assert(l != nullptr);
  vsums = nval;

  place_pointers(vrsums, vsums);

  l->sums = vrsums.data();
}

void ALayerAdaptor::rand(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vrand = nval;
  l->rand = vrand.data();
}

void ALayerAdaptor::rand(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vrand = std::move(nval);
  l->rand = vrand.data();
}

void ALayerAdaptor::cost(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcost = nval;
  l->cost = vcost.data();
}

void ALayerAdaptor::cost(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcost = std::move(nval);
  l->cost = vcost.data();
}

void ALayerAdaptor::labels(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vlabels = nval;
  l->labels = vlabels.data();
}

void ALayerAdaptor::labels(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vlabels = std::move(nval);
  l->labels = vlabels.data();
}

void ALayerAdaptor::class_ids(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vclass_ids = nval;
  l->class_ids = vclass_ids.data();
}

void ALayerAdaptor::class_ids(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vclass_ids = std::move(nval);
  l->class_ids = vclass_ids.data();
}

void ALayerAdaptor::contrastive_neg_max(const int& nval)
{
  assert(l != nullptr);
  l->contrastive_neg_max = nval;
}

void ALayerAdaptor::cos_sim(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcos_sim = nval;
  l->cos_sim = vcos_sim.data();
}

void ALayerAdaptor::cos_sim(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcos_sim = std::move(nval);
  l->cos_sim = vcos_sim.data();
}

void ALayerAdaptor::exp_cos_sim(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vexp_cos_sim = nval;
  l->exp_cos_sim = vexp_cos_sim.data();
}

void ALayerAdaptor::exp_cos_sim(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vexp_cos_sim = std::move(nval);
  l->exp_cos_sim = vexp_cos_sim.data();
}

void ALayerAdaptor::p_constrastive(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vp_constrastive = nval;
  l->p_constrastive = vp_constrastive.data();
}

void ALayerAdaptor::p_constrastive(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vp_constrastive = std::move(nval);
  l->p_constrastive = vp_constrastive.data();
}

void ALayerAdaptor::contrast_p_gpu(const vcontrastive_params& nval)
{
  assert(l != nullptr);
  scontrastive_params = nval;
  l->contrast_p_gpu = scontrastive_params.data();
}

void ALayerAdaptor::contrast_p_gpu(vcontrastive_params&& nval)
{
  assert(l != nullptr);
  scontrast_p_gpu = std::move(nval);
  l->contrast_p_gpu = scontrastive_params.data();
}

void ALayerAdaptor::state(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstate = nval;
  l->state = vstate.data();
}

void ALayerAdaptor::state(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstate = std::move(nval);
  l->state = vstate.data();
}

void ALayerAdaptor::prev_state(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vprev_state = nval;
  l->prev_state = vprev_state.data();
}

void ALayerAdaptor::prev_state(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vprev_state = std::move(nval);
  l->prev_state = vprev_state.data();
}

void ALayerAdaptor::forgot_state(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vforgot_state = nval;
  l->forgot_state = vforgot_state.data();
}

void ALayerAdaptor::forgot_state(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vforgot_state = std::move(nval);
  l->forgot_state = vforgot_state.data();
}

void ALayerAdaptor::forgot_delta(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vforgot_delta = nval;
  l->forgot_delta = vforgot_delta.data();
}

void ALayerAdaptor::forgot_delta(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vforgot_delta = std::move(nval);
  l->forgot_delta = vforgot_delta.data();
}

void ALayerAdaptor::state_delta(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstate_delta = nval;
  l->state_delta = vstate_delta.data();
}

void ALayerAdaptor::state_delta(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstate_delta = std::move(nval);
  l->state_delta = vstate_delta.data();
}

void ALayerAdaptor::combine_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcombine_cpu = nval;
  l->combine_cpu = vcombine_cpu.data();
}

void ALayerAdaptor::combine_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcombine_cpu = std::move(nval);
  l->combine_cpu = vcombine_cpu.data();
}

void ALayerAdaptor::combine_delta_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcombine_delta_cpu = nval;
  l->combine_delta_cpu = vcombine_delta_cpu.data();
}

void ALayerAdaptor::combine_delta_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcombine_delta_cpu = std::move(nval);
  l->combine_delta_cpu = vcombine_delta_cpu.data();
}

void ALayerAdaptor::concat(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vconcat = nval;
  l->concat = vconcat.data();
}

void ALayerAdaptor::concat(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vconcat = std::move(nval);
  l->concat = vconcat.data();
}

void ALayerAdaptor::concat_delta(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vconcat_delta = nval;
  l->concat_delta = vconcat_delta.data();
}

void ALayerAdaptor::concat_delta(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vconcat_delta = std::move(nval);
  l->concat_delta = vconcat_delta.data();
}

void ALayerAdaptor::binary_weights(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbinary_weights = nval;
  l->binary_weights = vbinary_weights.data();
}

void ALayerAdaptor::binary_weights(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbinary_weights = std::move(nval);
  l->binary_weights = vbinary_weights.data();
}

void ALayerAdaptor::biases(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbiases = nval;
  l->biases = vbiases.data();
}

void ALayerAdaptor::biases(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbiases = std::move(nval);
  l->biases = vbiases.data();
}

void ALayerAdaptor::bias_updates(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_updates = nval;
  l->bias_updates = vbias_updates.data();
}

void ALayerAdaptor::bias_updates(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_updates = std::move(nval);
  l->bias_updates = vbias_updates.data();
}

void ALayerAdaptor::scales(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscales = nval;
  l->scales = vscales.data();
}

void ALayerAdaptor::scales(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscales = std::move(nval);
  l->scales = vscales.data();
}

void ALayerAdaptor::scale_updates(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_updates = nval;
  l->scale_updates = vscale_updates.data();
}

void ALayerAdaptor::scale_updates(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_updates = std::move(nval);
  l->scale_updates = vscale_updates.data();
}

void ALayerAdaptor::weights_ema(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweights_ema = nval;
  l->weights_ema = vweights_ema.data();
}

void ALayerAdaptor::weights_ema(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweights_ema = std::move(nval);
  l->weights_ema = vweights_ema.data();
}

void ALayerAdaptor::biases_ema(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbiases_ema = nval;
  l->biases_ema = vbiases_ema.data();
}

void ALayerAdaptor::biases_ema(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbiases_ema = std::move(nval);
  l->biases_ema = vbiases_ema.data();
}

void ALayerAdaptor::scales_ema(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscales_ema = nval;
  l->scales_ema = vscales_ema.data();
}

void ALayerAdaptor::scales_ema(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscales_ema = std::move(nval);
  l->scales_ema = vscales_ema.data();
}

void ALayerAdaptor::weights(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweights = nval;
  l->weights = vweights.data();
}

void ALayerAdaptor::weights(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweights = std::move(nval);
  l->weights = vweights.data();
}

void ALayerAdaptor::weight_updates(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweight_updates = nval;
  l->weight_updates = vweight_updates.data();
}

void ALayerAdaptor::weight_updates(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweight_updates = std::move(nval);
  l->weight_updates = vweight_updates.data();
}

void ALayerAdaptor::scale_x_y(const float& nval)
{
  assert(l != nullptr);
  l->scale_x_y = nval;
}

void ALayerAdaptor::objectness_smooth(const int& nval)
{
  assert(l != nullptr);
  l->objectness_smooth = nval;
}

void ALayerAdaptor::new_coords(const int& nval)
{
  assert(l != nullptr);
  l->new_coords = nval;
}

void ALayerAdaptor::show_details(const int& nval)
{
  assert(l != nullptr);
  l->show_details = nval;
}

void ALayerAdaptor::max_delta(const float& nval)
{
  assert(l != nullptr);
  l->max_delta = nval;
}

void ALayerAdaptor::uc_normalizer(const float& nval)
{
  assert(l != nullptr);
  l->uc_normalizer = nval;
}

void ALayerAdaptor::iou_normalizer(const float& nval)
{
  assert(l != nullptr);
  l->iou_normalizer = nval;
}

void ALayerAdaptor::obj_normalizer(const float& nval)
{
  assert(l != nullptr);
  l->obj_normalizer = nval;
}

void ALayerAdaptor::cls_normalizer(const float& nval)
{
  assert(l != nullptr);
  l->cls_normalizer = nval;
}

void ALayerAdaptor::delta_normalizer(const float& nval)
{
  assert(l != nullptr);
  l->delta_normalizer = nval;
}

void ALayerAdaptor::iou_loss(const IOU_LOSS& nval)
{
  assert(l != nullptr);
  l->iou_loss = nval;
}

void ALayerAdaptor::iou_loss(IOU_LOSS&& nval)
{
  assert(l != nullptr);
  l->iou_loss = nval;
}

void ALayerAdaptor::iou_thresh_kind(const IOU_LOSS& nval)
{
  assert(l != nullptr);
  l->iou_thresh_kind = nval;
}

void ALayerAdaptor::iou_thresh_kind(IOU_LOSS&& nval)
{
  assert(l != nullptr);
  l->iou_thresh_kind = nval;
}

void ALayerAdaptor::nms_kind(const NMS_KIND& nval)
{
  assert(l != nullptr);
  l->nms_kind = nval;
}

void ALayerAdaptor::nms_kind(NMS_KIND&& nval)
{
  assert(l != nullptr);
  l->nms_kind = nval;
}

void ALayerAdaptor::beta_nms(const float& nval)
{
  assert(l != nullptr);
  l->beta_nms = nval;
}

void ALayerAdaptor::yolo_point(const YOLO_POINT& nval)
{
  assert(l != nullptr);
  l->yolo_point = nval;
}

void ALayerAdaptor::align_bit_weights_gpu(const std::string& nval)
{
  assert(l != nullptr);
  salign_bit_weights_gpu = nval;
  l->align_bit_weights_gpu = salign_bit_weights_gpu.data();
}

void ALayerAdaptor::align_bit_weights_gpu(std::string&& nval)
{
  assert(l != nullptr);
  salign_bit_weights_gpu = std::move(nval);
  l->align_bit_weights_gpu = salign_bit_weights_gpu.data();
}

void ALayerAdaptor::mean_arr_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vmean_arr_gpu = nval;
  l->mean_arr_gpu = vmean_arr_gpu.data();
}

void ALayerAdaptor::mean_arr_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vmean_arr_gpu = std::move(nval);
  l->mean_arr_gpu = vmean_arr_gpu.data();
}

void ALayerAdaptor::align_workspace_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  valign_workspace_gpu = nval;
  l->align_workspace_gpu = valign_workspace_gpu.data();
}

void ALayerAdaptor::align_workspace_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  valign_workspace_gpu = std::move(nval);
  l->align_workspace_gpu = valign_workspace_gpu.data();
}

void ALayerAdaptor::transposed_align_workspace_gpu(
    const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtransposed_align_workspace_gpu = nval;
  l->transposed_align_workspace_gpu = vtransposed_align_workspace_gpu.data();
}

void ALayerAdaptor::transposed_align_workspace_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtransposed_align_workspace_gpu = std::move(nval);
  l->transposed_align_workspace_gpu = vtransposed_align_workspace_gpu.data();
}

void ALayerAdaptor::align_workspace_size(const int& nval)
{
  assert(l != nullptr);
  l->align_workspace_size = nval;
}

void ALayerAdaptor::align_bit_weights(const std::string& nval)
{
  assert(l != nullptr);
  salign_bit_weights = nval;
  l->align_bit_weights = salign_bit_weights.data();
}

void ALayerAdaptor::align_bit_weights(std::string&& nval)
{
  assert(l != nullptr);
  salign_bit_weights = std::move(nval);
  l->align_bit_weights = salign_bit_weights.data();
}

void ALayerAdaptor::mean_arr(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vmean_arr = nval;
  l->mean_arr = vmean_arr.data();
}

void ALayerAdaptor::mean_arr(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vmean_arr = std::move(nval);
  l->mean_arr = vmean_arr.data();
}

void ALayerAdaptor::align_bit_weights_size(const int& nval)
{
  assert(l != nullptr);
  l->align_bit_weights_size = nval;
}

void ALayerAdaptor::lda_align(const int& nval)
{
  assert(l != nullptr);
  l->lda_align = nval;
}

void ALayerAdaptor::new_lda(const int& nval)
{
  assert(l != nullptr);
  l->new_lda = nval;
}

void ALayerAdaptor::bit_align(const int& nval)
{
  assert(l != nullptr);
  l->bit_align = nval;
}

void ALayerAdaptor::col_image(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcol_image = nval;
  l->col_image = vcol_image.data();
}

void ALayerAdaptor::col_image(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcol_image = std::move(nval);
  l->col_image = vcol_image.data();
}

void ALayerAdaptor::delta(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdelta = nval;
  l->delta = vdelta.data();
}

void ALayerAdaptor::delta(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdelta = std::move(nval);
  l->delta = vdelta.data();
}

void ALayerAdaptor::output(const std::vector<float>& nval)
{
  assert(l != nullptr);
  voutput = nval;
  l->output = voutput.data();
}

void ALayerAdaptor::output(std::vector<float>&& nval)
{
  assert(l != nullptr);
  voutput = std::move(nval);
  l->output = voutput.data();
}

void ALayerAdaptor::activation_input(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vactivation_input = nval;
  l->activation_input = vactivation_input.data();
}

void ALayerAdaptor::activation_input(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vactivation_input = std::move(nval);
  l->activation_input = vactivation_input.data();
}

void ALayerAdaptor::delta_pinned(const int& nval)
{
  assert(l != nullptr);
  l->delta_pinned = nval;
}

void ALayerAdaptor::output_pinned(const int& nval)
{
  assert(l != nullptr);
  l->output_pinned = nval;
}

void ALayerAdaptor::loss(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vloss = nval;
  l->loss = vloss.data();
}

void ALayerAdaptor::loss(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vloss = std::move(nval);
  l->loss = vloss.data();
}

void ALayerAdaptor::squared(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vsquared = nval;
  l->squared = vsquared.data();
}

void ALayerAdaptor::squared(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vsquared = std::move(nval);
  l->squared = vsquared.data();
}

void ALayerAdaptor::norms(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vnorms = nval;
  l->norms = vnorms.data();
}

void ALayerAdaptor::norms(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vnorms = std::move(nval);
  l->norms = vnorms.data();
}

void ALayerAdaptor::spatial_mean(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vspatial_mean = nval;
  l->spatial_mean = vspatial_mean.data();
}

void ALayerAdaptor::spatial_mean(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vspatial_mean = std::move(nval);
  l->spatial_mean = vspatial_mean.data();
}

void ALayerAdaptor::mean(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vmean = nval;
  l->mean = vmean.data();
}

void ALayerAdaptor::mean(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vmean = std::move(nval);
  l->mean = vmean.data();
}

void ALayerAdaptor::variance(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vvariance = nval;
  l->variance = vvariance.data();
}

void ALayerAdaptor::variance(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vvariance = std::move(nval);
  l->variance = vvariance.data();
}

void ALayerAdaptor::mean_delta(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vmean_delta = nval;
  l->mean_delta = vmean_delta.data();
}

void ALayerAdaptor::mean_delta(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vmean_delta = std::move(nval);
  l->mean_delta = vmean_delta.data();
}

void ALayerAdaptor::variance_delta(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vvariance_delta = nval;
  l->variance_delta = vvariance_delta.data();
}

void ALayerAdaptor::variance_delta(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vvariance_delta = std::move(nval);
  l->variance_delta = vvariance_delta.data();
}

void ALayerAdaptor::rolling_mean(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vrolling_mean = nval;
  l->rolling_mean = vrolling_mean.data();
}

void ALayerAdaptor::rolling_mean(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vrolling_mean = std::move(nval);
  l->rolling_mean = vrolling_mean.data();
}

void ALayerAdaptor::rolling_variance(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vrolling_variance = nval;
  l->rolling_variance = vrolling_variance.data();
}

void ALayerAdaptor::rolling_variance(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vrolling_variance = std::move(nval);
  l->rolling_variance = vrolling_variance.data();
}

void ALayerAdaptor::x(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vx = nval;
  l->x = vx.data();
}

void ALayerAdaptor::x(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vx = std::move(nval);
  l->x = vx.data();
}

void ALayerAdaptor::x_norm(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vx_norm = nval;
  l->x_norm = vx_norm.data();
}

void ALayerAdaptor::x_norm(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vx_norm = std::move(nval);
  l->x_norm = vx_norm.data();
}

void ALayerAdaptor::m(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vm = nval;
  l->m = vm.data();
}

void ALayerAdaptor::m(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vm = std::move(nval);
  l->m = vm.data();
}

void ALayerAdaptor::v(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vv = nval;
  l->v = vv.data();
}

void ALayerAdaptor::v(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vv = std::move(nval);
  l->v = vv.data();
}

void ALayerAdaptor::bias_m(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_m = nval;
  l->bias_m = vbias_m.data();
}

void ALayerAdaptor::bias_m(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_m = std::move(nval);
  l->bias_m = vbias_m.data();
}

void ALayerAdaptor::bias_v(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_v = nval;
  l->bias_v = vbias_v.data();
}

void ALayerAdaptor::bias_v(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_v = std::move(nval);
  l->bias_v = vbias_v.data();
}

void ALayerAdaptor::scale_m(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_m = nval;
  l->scale_m = vscale_m.data();
}

void ALayerAdaptor::scale_m(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_m = std::move(nval);
  l->scale_m = vscale_m.data();
}

void ALayerAdaptor::scale_v(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_v = nval;
  l->scale_v = vscale_v.data();
}

void ALayerAdaptor::scale_v(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_v = std::move(nval);
  l->scale_v = vscale_v.data();
}

void ALayerAdaptor::z_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vz_cpu = nval;
  l->z_cpu = vz_cpu.data();
}

void ALayerAdaptor::z_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vz_cpu = std::move(nval);
  l->z_cpu = vz_cpu.data();
}

void ALayerAdaptor::r_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vr_cpu = nval;
  l->r_cpu = vr_cpu.data();
}

void ALayerAdaptor::r_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vr_cpu = std::move(nval);
  l->r_cpu = vr_cpu.data();
}

void ALayerAdaptor::h_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vh_cpu = nval;
  l->h_cpu = vh_cpu.data();
}

void ALayerAdaptor::h_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vh_cpu = std::move(nval);
  l->h_cpu = vh_cpu.data();
}

void ALayerAdaptor::stored_h_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstored_h_cpu = nval;
  l->stored_h_cpu = vstored_h_cpu.data();
}

void ALayerAdaptor::stored_h_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstored_h_cpu = std::move(nval);
  l->stored_h_cpu = vstored_h_cpu.data();
}

void ALayerAdaptor::prev_state_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vprev_state_cpu = nval;
  l->prev_state_cpu = vprev_state_cpu.data();
}

void ALayerAdaptor::prev_state_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vprev_state_cpu = std::move(nval);
  l->prev_state_cpu = vprev_state_cpu.data();
}

void ALayerAdaptor::temp_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtemp_cpu = nval;
  l->temp_cpu = vtemp_cpu.data();
}

void ALayerAdaptor::temp_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtemp_cpu = std::move(nval);
  l->temp_cpu = vtemp_cpu.data();
}

void ALayerAdaptor::temp2_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtemp2_cpu = nval;
  l->temp2_cpu = vtemp2_cpu.data();
}

void ALayerAdaptor::temp2_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtemp2_cpu = std::move(nval);
  l->temp2_cpu = vtemp2_cpu.data();
}

void ALayerAdaptor::temp3_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtemp3_cpu = nval;
  l->temp3_cpu = vtemp3_cpu.data();
}

void ALayerAdaptor::temp3_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtemp3_cpu = std::move(nval);
  l->temp3_cpu = vtemp3_cpu.data();
}

void ALayerAdaptor::dh_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdh_cpu = nval;
  l->dh_cpu = vdh_cpu.data();
}

void ALayerAdaptor::dh_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdh_cpu = std::move(nval);
  l->dh_cpu = vdh_cpu.data();
}

void ALayerAdaptor::hh_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vhh_cpu = nval;
  l->hh_cpu = vhh_cpu.data();
}

void ALayerAdaptor::hh_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vhh_cpu = std::move(nval);
  l->hh_cpu = vhh_cpu.data();
}

void ALayerAdaptor::prev_cell_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vprev_cell_cpu = nval;
  l->prev_cell_cpu = vprev_cell_cpu.data();
}

void ALayerAdaptor::prev_cell_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vprev_cell_cpu = std::move(nval);
  l->prev_cell_cpu = vprev_cell_cpu.data();
}

void ALayerAdaptor::cell_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcell_cpu = nval;
  l->cell_cpu = vcell_cpu.data();
}

void ALayerAdaptor::cell_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcell_cpu = std::move(nval);
  l->cell_cpu = vcell_cpu.data();
}

void ALayerAdaptor::f_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vf_cpu = nval;
  l->f_cpu = vf_cpu.data();
}

void ALayerAdaptor::f_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vf_cpu = std::move(nval);
  l->f_cpu = vf_cpu.data();
}

void ALayerAdaptor::i_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vi_cpu = nval;
  l->i_cpu = vi_cpu.data();
}

void ALayerAdaptor::i_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vi_cpu = std::move(nval);
  l->i_cpu = vi_cpu.data();
}

void ALayerAdaptor::g_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vg_cpu = nval;
  l->g_cpu = vg_cpu.data();
}

void ALayerAdaptor::g_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vg_cpu = std::move(nval);
  l->g_cpu = vg_cpu.data();
}

void ALayerAdaptor::o_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vo_cpu = nval;
  l->o_cpu = vo_cpu.data();
}

void ALayerAdaptor::o_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vo_cpu = std::move(nval);
  l->o_cpu = vo_cpu.data();
}

void ALayerAdaptor::c_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vc_cpu = nval;
  l->c_cpu = vc_cpu.data();
}

void ALayerAdaptor::c_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vc_cpu = std::move(nval);
  l->c_cpu = vc_cpu.data();
}

void ALayerAdaptor::stored_c_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstored_c_cpu = nval;
  l->stored_c_cpu = vstored_c_cpu.data();
}

void ALayerAdaptor::stored_c_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstored_c_cpu = std::move(nval);
  l->stored_c_cpu = vstored_c_cpu.data();
}

void ALayerAdaptor::dc_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdc_cpu = nval;
  l->dc_cpu = vdc_cpu.data();
}

void ALayerAdaptor::dc_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdc_cpu = std::move(nval);
  l->dc_cpu = vdc_cpu.data();
}

void ALayerAdaptor::binary_input(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbinary_input = nval;
  l->binary_input = vbinary_input.data();
}

void ALayerAdaptor::binary_input(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbinary_input = std::move(nval);
  l->binary_input = vbinary_input.data();
}

void ALayerAdaptor::bin_re_packed_input(const std::vector<uint32_t>& nval)
{
  assert(l != nullptr);
  vbin_re_packed_input = nval;
  l->bin_re_packed_input = vbin_re_packed_input.data();
}

void ALayerAdaptor::bin_re_packed_input(std::vector<uint32_t>&& nval)
{
  assert(l != nullptr);
  vbin_re_packed_input = std::move(nval);
  l->bin_re_packed_input = vbin_re_packed_input.data();
}

void ALayerAdaptor::t_bit_input(const std::string& nval)
{
  assert(l != nullptr);
  st_bit_input = nval;
  l->t_bit_input = st_bit_input.data();
}

void ALayerAdaptor::t_bit_input(std::string&& nval)
{
  assert(l != nullptr);
  st_bit_input = std::move(nval);
  l->t_bit_input = st_bit_input.data();
}

void ALayerAdaptor::input_layer(const layerptr& nval)
{
  assert(l != nullptr);
  minput_layer = nval;
  l->input_layer = minput_layer.get();
}

void ALayerAdaptor::input_layer(layerptr&& nval)
{
  assert(l != nullptr);
  minput_layer = std::move(nval);
  l->input_layer = minput_layer.get();
}

void ALayerAdaptor::self_layer(const layerptr& nval)
{
  assert(l != nullptr);
  mself_layer = nval;
  l->self_layer = mself_layer.get();
}

void ALayerAdaptor::self_layer(layerptr&& nval)
{
  assert(l != nullptr);
  mself_layer = std::move(nval);
  l->self_layer = mself_layer.get();
}

void ALayerAdaptor::output_layer(const layerptr& nval)
{
  assert(l != nullptr);
  moutput_layer = nval;
  l->output_layer = moutput_layer.get();
}

void ALayerAdaptor::output_layer(layerptr&& nval)
{
  assert(l != nullptr);
  moutput_layer = std::move(nval);
  l->output_layer = moutput_layer.get();
}

void ALayerAdaptor::reset_layer(const layerptr& nval)
{
  assert(l != nullptr);
  mreset_layer = nval;
  l->reset_layer = mreset_layer.get();
}

void ALayerAdaptor::reset_layer(layerptr&& nval)
{
  assert(l != nullptr);
  mreset_layer = std::move(nval);
  l->reset_layer = mreset_layer.get();
}

void ALayerAdaptor::update_layer(const layerptr& nval)
{
  assert(l != nullptr);
  mupdate_layer = nval;
  l->update_layer = mupdate_layer.get();
}

void ALayerAdaptor::update_layer(layerptr&& nval)
{
  assert(l != nullptr);
  mupdate_layer = std::move(nval);
  l->update_layer = mupdate_layer.get();
}

void ALayerAdaptor::state_layer(const layerptr& nval)
{
  assert(l != nullptr);
  mstate_layer = nval;
  l->state_layer = mstate_layer.get();
}

void ALayerAdaptor::state_layer(layerptr&& nval)
{
  assert(l != nullptr);
  mstate_layer = std::move(nval);
  l->state_layer = mstate_layer.get();
}

void ALayerAdaptor::input_gate_layer(const layerptr& nval)
{
  assert(l != nullptr);
  minput_gate_layer = nval;
  l->input_gate_layer = minput_gate_layer.get();
}

void ALayerAdaptor::input_gate_layer(layerptr&& nval)
{
  assert(l != nullptr);
  minput_gate_layer = std::move(nval);
  l->input_gate_layer = minput_gate_layer.get();
}

void ALayerAdaptor::state_gate_layer(const layerptr& nval)
{
  assert(l != nullptr);
  mstate_gate_layer = nval;
  l->state_gate_layer = mstate_gate_layer.get();
}

void ALayerAdaptor::state_gate_layer(layerptr&& nval)
{
  assert(l != nullptr);
  mstate_gate_layer = std::move(nval);
  l->state_gate_layer = mstate_gate_layer.get();
}

void ALayerAdaptor::input_save_layer(const layerptr& nval)
{
  assert(l != nullptr);
  minput_save_layer = nval;
  l->input_save_layer = minput_save_layer.get();
}

void ALayerAdaptor::input_save_layer(layerptr&& nval)
{
  assert(l != nullptr);
  minput_save_layer = std::move(nval);
  l->input_save_layer = minput_save_layer.get();
}

void ALayerAdaptor::state_save_layer(const layerptr& nval)
{
  assert(l != nullptr);
  mstate_save_layer = nval;
  l->state_save_layer = mstate_save_layer.get();
}

void ALayerAdaptor::state_save_layer(layerptr&& nval)
{
  assert(l != nullptr);
  mstate_save_layer = std::move(nval);
  l->state_save_layer = mstate_save_layer.get();
}

void ALayerAdaptor::input_state_layer(const layerptr& nval)
{
  assert(l != nullptr);
  minput_state_layer = nval;
  l->input_state_layer = minput_state_layer.get();
}

void ALayerAdaptor::input_state_layer(layerptr&& nval)
{
  assert(l != nullptr);
  minput_state_layer = std::move(nval);
  l->input_state_layer = minput_state_layer.get();
}

void ALayerAdaptor::state_state_layer(const layerptr& nval)
{
  assert(l != nullptr);
  mstate_state_layer = nval;
  l->state_state_layer = mstate_state_layer.get();
}

void ALayerAdaptor::state_state_layer(layerptr&& nval)
{
  assert(l != nullptr);
  mstate_state_layer = std::move(nval);
  l->state_state_layer = mstate_state_layer.get();
}

void ALayerAdaptor::input_z_layer(const layerptr& nval)
{
  assert(l != nullptr);
  minput_z_layer = nval;
  l->input_z_layer = minput_z_layer.get();
}

void ALayerAdaptor::input_z_layer(layerptr&& nval)
{
  assert(l != nullptr);
  minput_z_layer = std::move(nval);
  l->input_z_layer = minput_z_layer.get();
}

void ALayerAdaptor::state_z_layer(const layerptr& nval)
{
  assert(l != nullptr);
  mstate_z_layer = nval;
  l->state_z_layer = mstate_z_layer.get();
}

void ALayerAdaptor::state_z_layer(layerptr&& nval)
{
  assert(l != nullptr);
  mstate_z_layer = std::move(nval);
  l->state_z_layer = mstate_z_layer.get();
}

void ALayerAdaptor::input_r_layer(const layerptr& nval)
{
  assert(l != nullptr);
  minput_r_layer = nval;
  l->input_r_layer = minput_r_layer.get();
}

void ALayerAdaptor::input_r_layer(layerptr&& nval)
{
  assert(l != nullptr);
  minput_r_layer = std::move(nval);
  l->input_r_layer = minput_r_layer.get();
}

void ALayerAdaptor::state_r_layer(const layerptr& nval)
{
  assert(l != nullptr);
  mstate_r_layer = nval;
  l->state_r_layer = mstate_r_layer.get();
}

void ALayerAdaptor::state_r_layer(layerptr&& nval)
{
  assert(l != nullptr);
  mstate_r_layer = std::move(nval);
  l->state_r_layer = mstate_r_layer.get();
}

void ALayerAdaptor::input_h_layer(const layerptr& nval)
{
  assert(l != nullptr);
  minput_h_layer = nval;
  l->input_h_layer = minput_h_layer.get();
}

void ALayerAdaptor::input_h_layer(layerptr&& nval)
{
  assert(l != nullptr);
  minput_h_layer = std::move(nval);
  l->input_h_layer = minput_h_layer.get();
}

void ALayerAdaptor::state_h_layer(const layerptr& nval)
{
  assert(l != nullptr);
  mstate_h_layer = nval;
  l->state_h_layer = mstate_h_layer.get();
}

void ALayerAdaptor::state_h_layer(layerptr&& nval)
{
  assert(l != nullptr);
  mstate_h_layer = std::move(nval);
  l->state_h_layer = mstate_h_layer.get();
}

void ALayerAdaptor::wz(const layerptr& nval)
{
  assert(l != nullptr);
  mwz = nval;
  l->wz = mwz.get();
}

void ALayerAdaptor::wz(layerptr&& nval)
{
  assert(l != nullptr);
  mwz = std::move(nval);
  l->wz = mwz.get();
}

void ALayerAdaptor::uz(const layerptr& nval)
{
  assert(l != nullptr);
  muz = nval;
  l->uz = muz.get();
}

void ALayerAdaptor::uz(layerptr&& nval)
{
  assert(l != nullptr);
  muz = std::move(nval);
  l->uz = muz.get();
}

void ALayerAdaptor::wr(const layerptr& nval)
{
  assert(l != nullptr);
  mwr = nval;
  l->wr = mwr.get();
}

void ALayerAdaptor::wr(layerptr&& nval)
{
  assert(l != nullptr);
  mwr = std::move(nval);
  l->wr = mwr.get();
}

void ALayerAdaptor::ur(const layerptr& nval)
{
  assert(l != nullptr);
  mur = nval;
  l->ur = mur.get();
}

void ALayerAdaptor::ur(layerptr&& nval)
{
  assert(l != nullptr);
  mur = std::move(nval);
  l->ur = mur.get();
}

void ALayerAdaptor::wh(const layerptr& nval)
{
  assert(l != nullptr);
  mwh = nval;
  l->wh = mwh.get();
}

void ALayerAdaptor::wh(layerptr&& nval)
{
  assert(l != nullptr);
  mwh = std::move(nval);
  l->wh = mwh.get();
}

void ALayerAdaptor::uh(const layerptr& nval)
{
  assert(l != nullptr);
  muh = nval;
  l->uh = muh.get();
}

void ALayerAdaptor::uh(layerptr&& nval)
{
  assert(l != nullptr);
  muh = std::move(nval);
  l->uh = muh.get();
}

void ALayerAdaptor::uo(const layerptr& nval)
{
  assert(l != nullptr);
  muo = nval;
  l->uo = muo.get();
}

void ALayerAdaptor::uo(layerptr&& nval)
{
  assert(l != nullptr);
  muo = std::move(nval);
  l->uo = muo.get();
}

void ALayerAdaptor::wo(const layerptr& nval)
{
  assert(l != nullptr);
  mwo = nval;
  l->wo = mwo.get();
}

void ALayerAdaptor::wo(layerptr&& nval)
{
  assert(l != nullptr);
  mwo = std::move(nval);
  l->wo = mwo.get();
}

void ALayerAdaptor::vo(const layerptr& nval)
{
  assert(l != nullptr);
  mvo = nval;
  l->vo = mvo.get();
}

void ALayerAdaptor::vo(layerptr&& nval)
{
  assert(l != nullptr);
  mvo = std::move(nval);
  l->vo = mvo.get();
}

void ALayerAdaptor::uf(const layerptr& nval)
{
  assert(l != nullptr);
  muf = nval;
  l->uf = muf.get();
}

void ALayerAdaptor::uf(layerptr&& nval)
{
  assert(l != nullptr);
  muf = std::move(nval);
  l->uf = muf.get();
}

void ALayerAdaptor::wf(const layerptr& nval)
{
  assert(l != nullptr);
  mwf = nval;
  l->wf = mwf.get();
}

void ALayerAdaptor::wf(layerptr&& nval)
{
  assert(l != nullptr);
  mwf = std::move(nval);
  l->wf = mwf.get();
}

void ALayerAdaptor::vf(const layerptr& nval)
{
  assert(l != nullptr);
  mvf = nval;
  l->vf = mvf.get();
}

void ALayerAdaptor::vf(layerptr&& nval)
{
  assert(l != nullptr);
  mvf = std::move(nval);
  l->vf = mvf.get();
}

void ALayerAdaptor::ui(const layerptr& nval)
{
  assert(l != nullptr);
  mui = nval;
  l->ui = mui.get();
}

void ALayerAdaptor::ui(layerptr&& nval)
{
  assert(l != nullptr);
  mui = std::move(nval);
  l->ui = mui.get();
}

void ALayerAdaptor::wi(const layerptr& nval)
{
  assert(l != nullptr);
  mwi = nval;
  l->wi = mwi.get();
}

void ALayerAdaptor::wi(layerptr&& nval)
{
  assert(l != nullptr);
  mwi = std::move(nval);
  l->wi = mwi.get();
}

void ALayerAdaptor::vi(const layerptr& nval)
{
  assert(l != nullptr);
  mvi = nval;
  l->vi = mvi.get();
}

void ALayerAdaptor::vi(layerptr&& nval)
{
  assert(l != nullptr);
  mvi = std::move(nval);
  l->vi = mvi.get();
}

void ALayerAdaptor::ug(const layerptr& nval)
{
  assert(l != nullptr);
  mug = nval;
  l->ug = mug.get();
}

void ALayerAdaptor::ug(layerptr&& nval)
{
  assert(l != nullptr);
  mug = std::move(nval);
  l->ug = mug.get();
}

void ALayerAdaptor::wg(const layerptr& nval)
{
  assert(l != nullptr);
  mwg = nval;
  l->wg = mwg.get();
}

void ALayerAdaptor::wg(layerptr&& nval)
{
  assert(l != nullptr);
  mwg = std::move(nval);
  l->wg = mwg.get();
}

void ALayerAdaptor::softmax_tree(tree* nval)
{
  assert(l != nullptr);
  l->softmax_tree = nval;
}

void ALayerAdaptor::workspace_size(const size_t& nval)
{
  assert(l != nullptr);
  l->workspace_size = nval;
}

void ALayerAdaptor::workspace_size(size_t&& nval)
{
  assert(l != nullptr);
  l->workspace_size = nval;
}

//#ifdef GPU
void ALayerAdaptor::indexes_gpu(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vindexes_gpu = nval;
  l->indexes_gpu = vindexes_gpu.data();
}

void ALayerAdaptor::indexes_gpu(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vindexes_gpu = std::move(nval);
  l->indexes_gpu = vindexes_gpu.data();
}

void ALayerAdaptor::stream(const int& nval)
{
  assert(l != nullptr);
  l->stream = nval;
}

void ALayerAdaptor::wait_stream_id(const int& nval)
{
  assert(l != nullptr);
  l->wait_stream_id = nval;
}

void ALayerAdaptor::z_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vz_gpu = nval;
  l->z_gpu = vz_gpu.data();
}

void ALayerAdaptor::z_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vz_gpu = std::move(nval);
  l->z_gpu = vz_gpu.data();
}

void ALayerAdaptor::r_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vr_gpu = nval;
  l->r_gpu = vr_gpu.data();
}

void ALayerAdaptor::r_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vr_gpu = std::move(nval);
  l->r_gpu = vr_gpu.data();
}

void ALayerAdaptor::h_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vh_gpu = nval;
  l->h_gpu = vh_gpu.data();
}

void ALayerAdaptor::h_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vh_gpu = std::move(nval);
  l->h_gpu = vh_gpu.data();
}

void ALayerAdaptor::stored_h_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstored_h_gpu = nval;
  l->stored_h_gpu = vstored_h_gpu.data();
}

void ALayerAdaptor::stored_h_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstored_h_gpu = std::move(nval);
  l->stored_h_gpu = vstored_h_gpu.data();
}

void ALayerAdaptor::bottelneck_hi_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbottelneck_hi_gpu = nval;
  l->bottelneck_hi_gpu = vbottelneck_hi_gpu.data();
}

void ALayerAdaptor::bottelneck_hi_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbottelneck_hi_gpu = std::move(nval);
  l->bottelneck_hi_gpu = vbottelneck_hi_gpu.data();
}

void ALayerAdaptor::bottelneck_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbottelneck_delta_gpu = nval;
  l->bottelneck_delta_gpu = vbottelneck_delta_gpu.data();
}

void ALayerAdaptor::bottelneck_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbottelneck_delta_gpu = std::move(nval);
  l->bottelneck_delta_gpu = vbottelneck_delta_gpu.data();
}

void ALayerAdaptor::temp_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtemp_gpu = nval;
  l->temp_gpu = vtemp_gpu.data();
}

void ALayerAdaptor::temp_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtemp_gpu = std::move(nval);
  l->temp_gpu = vtemp_gpu.data();
}

void ALayerAdaptor::temp2_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtemp2_gpu = nval;
  l->temp2_gpu = vtemp2_gpu.data();
}

void ALayerAdaptor::temp2_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtemp2_gpu = std::move(nval);
  l->temp2_gpu = vtemp2_gpu.data();
}

void ALayerAdaptor::temp3_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtemp3_gpu = nval;
  l->temp3_gpu = vtemp3_gpu.data();
}

void ALayerAdaptor::temp3_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtemp3_gpu = std::move(nval);
  l->temp3_gpu = vtemp3_gpu.data();
}

void ALayerAdaptor::dh_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdh_gpu = nval;
  l->dh_gpu = vdh_gpu.data();
}

void ALayerAdaptor::dh_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdh_gpu = std::move(nval);
  l->dh_gpu = vdh_gpu.data();
}

void ALayerAdaptor::hh_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vhh_gpu = nval;
  l->hh_gpu = vhh_gpu.data();
}

void ALayerAdaptor::hh_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vhh_gpu = std::move(nval);
  l->hh_gpu = vhh_gpu.data();
}

void ALayerAdaptor::prev_cell_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vprev_cell_gpu = nval;
  l->prev_cell_gpu = vprev_cell_gpu.data();
}

void ALayerAdaptor::prev_cell_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vprev_cell_gpu = std::move(nval);
  l->prev_cell_gpu = vprev_cell_gpu.data();
}

void ALayerAdaptor::prev_state_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vprev_state_gpu = nval;
  l->prev_state_gpu = vprev_state_gpu.data();
}

void ALayerAdaptor::prev_state_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vprev_state_gpu = std::move(nval);
  l->prev_state_gpu = vprev_state_gpu.data();
}

void ALayerAdaptor::last_prev_state_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vlast_prev_state_gpu = nval;
  l->last_prev_state_gpu = vlast_prev_state_gpu.data();
}

void ALayerAdaptor::last_prev_state_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vlast_prev_state_gpu = std::move(nval);
  l->last_prev_state_gpu = vlast_prev_state_gpu.data();
}

void ALayerAdaptor::last_prev_cell_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vlast_prev_cell_gpu = nval;
  l->last_prev_cell_gpu = vlast_prev_cell_gpu.data();
}

void ALayerAdaptor::last_prev_cell_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vlast_prev_cell_gpu = std::move(nval);
  l->last_prev_cell_gpu = vlast_prev_cell_gpu.data();
}

void ALayerAdaptor::cell_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcell_gpu = nval;
  l->cell_gpu = vcell_gpu.data();
}

void ALayerAdaptor::cell_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcell_gpu = std::move(nval);
  l->cell_gpu = vcell_gpu.data();
}

void ALayerAdaptor::f_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vf_gpu = nval;
  l->f_gpu = vf_gpu.data();
}

void ALayerAdaptor::f_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vf_gpu = std::move(nval);
  l->f_gpu = vf_gpu.data();
}

void ALayerAdaptor::i_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vi_gpu = nval;
  l->i_gpu = vi_gpu.data();
}

void ALayerAdaptor::i_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vi_gpu = std::move(nval);
  l->i_gpu = vi_gpu.data();
}

void ALayerAdaptor::g_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vg_gpu = nval;
  l->g_gpu = vg_gpu.data();
}

void ALayerAdaptor::g_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vg_gpu = std::move(nval);
  l->g_gpu = vg_gpu.data();
}

void ALayerAdaptor::o_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vo_gpu = nval;
  l->o_gpu = vo_gpu.data();
}

void ALayerAdaptor::o_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vo_gpu = std::move(nval);
  l->o_gpu = vo_gpu.data();
}

void ALayerAdaptor::c_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vc_gpu = nval;
  l->c_gpu = vc_gpu.data();
}

void ALayerAdaptor::c_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vc_gpu = std::move(nval);
  l->c_gpu = vc_gpu.data();
}

void ALayerAdaptor::stored_c_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstored_c_gpu = nval;
  l->stored_c_gpu = vstored_c_gpu.data();
}

void ALayerAdaptor::stored_c_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstored_c_gpu = std::move(nval);
  l->stored_c_gpu = vstored_c_gpu.data();
}

void ALayerAdaptor::dc_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdc_gpu = nval;
  l->dc_gpu = vdc_gpu.data();
}

void ALayerAdaptor::dc_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdc_gpu = std::move(nval);
  l->dc_gpu = vdc_gpu.data();
}

// adam
void ALayerAdaptor::m_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vm_gpu = nval;
  l->m_gpu = vm_gpu.data();
}

void ALayerAdaptor::m_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vm_gpu = std::move(nval);
  l->m_gpu = vm_gpu.data();
}

void ALayerAdaptor::v_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vv_gpu = nval;
  l->v_gpu = vv_gpu.data();
}

void ALayerAdaptor::v_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vv_gpu = std::move(nval);
  l->v_gpu = vv_gpu.data();
}

void ALayerAdaptor::bias_m_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_m_gpu = nval;
  l->bias_m_gpu = vbias_m_gpu.data();
}

void ALayerAdaptor::bias_m_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_m_gpu = std::move(nval);
  l->bias_m_gpu = vbias_m_gpu.data();
}

void ALayerAdaptor::scale_m_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_m_gpu = nval;
  l->scale_m_gpu = vscale_m_gpu.data();
}

void ALayerAdaptor::scale_m_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_m_gpu = std::move(nval);
  l->scale_m_gpu = vscale_m_gpu.data();
}

void ALayerAdaptor::bias_v_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_v_gpu = nval;
  l->bias_v_gpu = vbias_v_gpu.data();
}

void ALayerAdaptor::bias_v_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_v_gpu = std::move(nval);
  l->bias_v_gpu = vbias_v_gpu.data();
}

void ALayerAdaptor::scale_v_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_v_gpu = nval;
  l->scale_v_gpu = vscale_v_gpu.data();
}

void ALayerAdaptor::scale_v_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_v_gpu = std::move(nval);
  l->scale_v_gpu = vscale_v_gpu.data();
}

void ALayerAdaptor::combine_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcombine_gpu = nval;
  l->combine_gpu = vcombine_gpu.data();
}

void ALayerAdaptor::combine_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcombine_gpu = std::move(nval);
  l->combine_gpu = vcombine_gpu.data();
}

void ALayerAdaptor::combine_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcombine_delta_gpu = nval;
  l->combine_delta_gpu = vcombine_delta_gpu.data();
}

void ALayerAdaptor::combine_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcombine_delta_gpu = std::move(nval);
  l->combine_delta_gpu = vcombine_delta_gpu.data();
}

void ALayerAdaptor::forgot_state_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vforgot_state_gpu = nval;
  l->forgot_state_gpu = vforgot_state_gpu.data();
}

void ALayerAdaptor::forgot_state_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vforgot_state_gpu = std::move(nval);
  l->forgot_state_gpu = vforgot_state_gpu.data();
}

void ALayerAdaptor::forgot_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vforgot_delta_gpu = nval;
  l->forgot_delta_gpu = vforgot_delta_gpu.data();
}

void ALayerAdaptor::forgot_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vforgot_delta_gpu = std::move(nval);
  l->forgot_delta_gpu = vforgot_delta_gpu.data();
}

void ALayerAdaptor::state_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstate_gpu = nval;
  l->state_gpu = vstate_gpu.data();
}

void ALayerAdaptor::state_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstate_gpu = std::move(nval);
  l->state_gpu = vstate_gpu.data();
}

void ALayerAdaptor::state_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstate_delta_gpu = nval;
  l->state_delta_gpu = vstate_delta_gpu.data();
}

void ALayerAdaptor::state_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstate_delta_gpu = std::move(nval);
  l->state_delta_gpu = vstate_delta_gpu.data();
}

void ALayerAdaptor::gate_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vgate_gpu = nval;
  l->gate_gpu = vgate_gpu.data();
}

void ALayerAdaptor::gate_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vgate_gpu = std::move(nval);
  l->gate_gpu = vgate_gpu.data();
}

void ALayerAdaptor::gate_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vgate_delta_gpu = nval;
  l->gate_delta_gpu = vgate_delta_gpu.data();
}

void ALayerAdaptor::gate_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vgate_delta_gpu = std::move(nval);
  l->gate_delta_gpu = vgate_delta_gpu.data();
}

void ALayerAdaptor::save_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vsave_gpu = nval;
  l->save_gpu = vsave_gpu.data();
}

void ALayerAdaptor::save_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vsave_gpu = std::move(nval);
  l->save_gpu = vsave_gpu.data();
}

void ALayerAdaptor::save_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vsave_delta_gpu = nval;
  l->save_delta_gpu = vsave_delta_gpu.data();
}

void ALayerAdaptor::save_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vsave_delta_gpu = std::move(nval);
  l->save_delta_gpu = vsave_delta_gpu.data();
}

void ALayerAdaptor::concat_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vconcat_gpu = nval;
  l->concat_gpu = vconcat_gpu.data();
}

void ALayerAdaptor::concat_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vconcat_gpu = std::move(nval);
  l->concat_gpu = vconcat_gpu.data();
}

void ALayerAdaptor::concat_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vconcat_delta_gpu = nval;
  l->concat_delta_gpu = vconcat_delta_gpu.data();
}

void ALayerAdaptor::concat_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vconcat_delta_gpu = std::move(nval);
  l->concat_delta_gpu = vconcat_delta_gpu.data();
}

void ALayerAdaptor::binary_input_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbinary_input_gpu = nval;
  l->binary_input_gpu = vbinary_input_gpu.data();
}

void ALayerAdaptor::binary_input_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbinary_input_gpu = std::move(nval);
  l->binary_input_gpu = vbinary_input_gpu.data();
}

void ALayerAdaptor::binary_weights_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbinary_weights_gpu = nval;
  l->binary_weights_gpu = vbinary_weights_gpu.data();
}

void ALayerAdaptor::binary_weights_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbinary_weights_gpu = std::move(nval);
  l->binary_weights_gpu = vbinary_weights_gpu.data();
}

void ALayerAdaptor::bin_conv_shortcut_in_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbin_conv_shortcut_in_gpu = nval;
  l->bin_conv_shortcut_in_gpu = vbin_conv_shortcut_in_gpu.data();
}

void ALayerAdaptor::bin_conv_shortcut_in_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbin_conv_shortcut_in_gpu = std::move(nval);
  l->bin_conv_shortcut_in_gpu = vbin_conv_shortcut_in_gpu.data();
}

void ALayerAdaptor::bin_conv_shortcut_out_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbin_conv_shortcut_out_gpu = nval;
  l->bin_conv_shortcut_out_gpu = vbin_conv_shortcut_out_gpu.data();
}

void ALayerAdaptor::bin_conv_shortcut_out_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbin_conv_shortcut_out_gpu = std::move(nval);
  l->bin_conv_shortcut_out_gpu = vbin_conv_shortcut_out_gpu.data();
}

void ALayerAdaptor::mean_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vmean_gpu = nval;
  l->mean_gpu = vmean_gpu.data();
}

void ALayerAdaptor::mean_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vmean_gpu = std::move(nval);
  l->mean_gpu = vmean_gpu.data();
}

void ALayerAdaptor::variance_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vvariance_gpu = nval;
  l->variance_gpu = vvariance_gpu.data();
}

void ALayerAdaptor::variance_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vvariance_gpu = std::move(nval);
  l->variance_gpu = vvariance_gpu.data();
}

void ALayerAdaptor::m_cbn_avg_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vm_cbn_avg_gpu = nval;
  l->m_cbn_avg_gpu = vm_cbn_avg_gpu.data();
}

void ALayerAdaptor::m_cbn_avg_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vm_cbn_avg_gpu = std::move(nval);
  l->m_cbn_avg_gpu = vm_cbn_avg_gpu.data();
}

void ALayerAdaptor::v_cbn_avg_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vv_cbn_avg_gpu = nval;
  l->v_cbn_avg_gpu = vv_cbn_avg_gpu.data();
}

void ALayerAdaptor::v_cbn_avg_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vv_cbn_avg_gpu = std::move(nval);
  l->v_cbn_avg_gpu = vv_cbn_avg_gpu.data();
}

void ALayerAdaptor::rolling_mean_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vrolling_mean_gpu = nval;
  l->rolling_mean_gpu = vrolling_mean_gpu.data();
}

void ALayerAdaptor::rolling_mean_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vrolling_mean_gpu = std::move(nval);
  l->rolling_mean_gpu = vrolling_mean_gpu.data();
}

void ALayerAdaptor::rolling_variance_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vrolling_variance_gpu = nval;
  l->rolling_variance_gpu = vrolling_variance_gpu.data();
}

void ALayerAdaptor::rolling_variance_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vrolling_variance_gpu = std::move(nval);
  l->rolling_variance_gpu = vrolling_variance_gpu.data();
}

void ALayerAdaptor::variance_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vvariance_delta_gpu = nval;
  l->variance_delta_gpu = vvariance_delta_gpu.data();
}

void ALayerAdaptor::variance_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vvariance_delta_gpu = std::move(nval);
  l->variance_delta_gpu = vvariance_delta_gpu.data();
}

void ALayerAdaptor::mean_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vmean_delta_gpu = nval;
  l->mean_delta_gpu = vmean_delta_gpu.data();
}

void ALayerAdaptor::mean_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vmean_delta_gpu = std::move(nval);
  l->mean_delta_gpu = vmean_delta_gpu.data();
}

void ALayerAdaptor::col_image_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcol_image_gpu = nval;
  l->col_image_gpu = vcol_image_gpu.data();
}

void ALayerAdaptor::col_image_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcol_image_gpu = std::move(nval);
  l->col_image_gpu = vcol_image_gpu.data();
}

void ALayerAdaptor::x_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vx_gpu = nval;
  l->x_gpu = vx_gpu.data();
}

void ALayerAdaptor::x_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vx_gpu = std::move(nval);
  l->x_gpu = vx_gpu.data();
}

void ALayerAdaptor::x_norm_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vx_norm_gpu = nval;
  l->x_norm_gpu = vx_norm_gpu.data();
}

void ALayerAdaptor::x_norm_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vx_norm_gpu = std::move(nval);
  l->x_norm_gpu = vx_norm_gpu.data();
}

void ALayerAdaptor::weights_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweights_gpu = nval;
  l->weights_gpu = vweights_gpu.data();
}

void ALayerAdaptor::weights_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweights_gpu = std::move(nval);
  l->weights_gpu = vweights_gpu.data();
}

void ALayerAdaptor::weight_updates_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweight_updates_gpu = nval;
  l->weight_updates_gpu = vweight_updates_gpu.data();
}

void ALayerAdaptor::weight_updates_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweight_updates_gpu = std::move(nval);
  l->weight_updates_gpu = vweight_updates_gpu.data();
}

void ALayerAdaptor::weight_deform_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweight_deform_gpu = nval;
  l->weight_deform_gpu = vweight_deform_gpu.data();
}

void ALayerAdaptor::weight_deform_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweight_deform_gpu = std::move(nval);
  l->weight_deform_gpu = vweight_deform_gpu.data();
}

void ALayerAdaptor::weight_change_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweight_change_gpu = nval;
  l->weight_change_gpu = vweight_change_gpu.data();
}

void ALayerAdaptor::weight_change_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweight_change_gpu = std::move(nval);
  l->weight_change_gpu = vweight_change_gpu.data();
}

void ALayerAdaptor::weights_gpu16(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweights_gpu16 = nval;
  l->weights_gpu16 = vweights_gpu16.data();
}

void ALayerAdaptor::weights_gpu16(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweights_gpu16 = std::move(nval);
  l->weights_gpu16 = vweights_gpu16.data();
}

void ALayerAdaptor::weight_updates_gpu16(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweight_updates_gpu16 = nval;
  l->weight_updates_gpu16 = vweight_updates_gpu16.data();
}

void ALayerAdaptor::weight_updates_gpu16(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweight_updates_gpu16 = std::move(nval);
  l->weight_updates_gpu16 = vweight_updates_gpu16.data();
}

void ALayerAdaptor::biases_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbiases_gpu = nval;
  l->biases_gpu = vbiases_gpu.data();
}

void ALayerAdaptor::biases_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbiases_gpu = std::move(nval);
  l->biases_gpu = vbiases_gpu.data();
}

void ALayerAdaptor::bias_updates_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_updates_gpu = nval;
  l->bias_updates_gpu = vbias_updates_gpu.data();
}

void ALayerAdaptor::bias_updates_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_updates_gpu = std::move(nval);
  l->bias_updates_gpu = vbias_updates_gpu.data();
}

void ALayerAdaptor::bias_change_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_change_gpu = nval;
  l->bias_change_gpu = vbias_change_gpu.data();
}

void ALayerAdaptor::bias_change_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_change_gpu = std::move(nval);
  l->bias_change_gpu = vbias_change_gpu.data();
}

void ALayerAdaptor::scales_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscales_gpu = nval;
  l->scales_gpu = vscales_gpu.data();
}

void ALayerAdaptor::scales_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscales_gpu = std::move(nval);
  l->scales_gpu = vscales_gpu.data();
}

void ALayerAdaptor::scale_updates_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_updates_gpu = nval;
  l->scale_updates_gpu = vscale_updates_gpu.data();
}

void ALayerAdaptor::scale_updates_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_updates_gpu = std::move(nval);
  l->scale_updates_gpu = vscale_updates_gpu.data();
}

void ALayerAdaptor::scale_change_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_change_gpu = nval;
  l->scale_change_gpu = vscale_change_gpu.data();
}

void ALayerAdaptor::scale_change_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_change_gpu = std::move(nval);
  l->scale_change_gpu = vscale_change_gpu.data();
}

void ALayerAdaptor::input_antialiasing_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vinput_antialiasing_gpu = nval;
  l->input_antialiasing_gpu = vinput_antialiasing_gpu.data();
}

void ALayerAdaptor::input_antialiasing_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vinput_antialiasing_gpu = std::move(nval);
  l->input_antialiasing_gpu = vinput_antialiasing_gpu.data();
}

void ALayerAdaptor::output_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  voutput_gpu = nval;
  l->output_gpu = voutput_gpu.data();
}

void ALayerAdaptor::output_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  voutput_gpu = std::move(nval);
  l->output_gpu = voutput_gpu.data();
}

void ALayerAdaptor::output_avg_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  voutput_avg_gpu = nval;
  l->output_avg_gpu = voutput_avg_gpu.data();
}

void ALayerAdaptor::output_avg_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  voutput_avg_gpu = std::move(nval);
  l->output_avg_gpu = voutput_avg_gpu.data();
}

void ALayerAdaptor::activation_input_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vactivation_input_gpu = nval;
  l->activation_input_gpu = vactivation_input_gpu.data();
}

void ALayerAdaptor::activation_input_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vactivation_input_gpu = std::move(nval);
  l->activation_input_gpu = vactivation_input_gpu.data();
}

void ALayerAdaptor::loss_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vloss_gpu = nval;
  l->loss_gpu = vloss_gpu.data();
}

void ALayerAdaptor::loss_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vloss_gpu = std::move(nval);
  l->loss_gpu = vloss_gpu.data();
}

void ALayerAdaptor::delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdelta_gpu = nval;
  l->delta_gpu = vdelta_gpu.data();
}

void ALayerAdaptor::delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdelta_gpu = std::move(nval);
  l->delta_gpu = vdelta_gpu.data();
}

void ALayerAdaptor::cos_sim_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcos_sim_gpu = nval;
  l->cos_sim_gpu = vcos_sim_gpu.data();
}

void ALayerAdaptor::cos_sim_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcos_sim_gpu = std::move(nval);
  l->cos_sim_gpu = vcos_sim_gpu.data();
}

void ALayerAdaptor::rand_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vrand_gpu = nval;
  l->rand_gpu = vrand_gpu.data();
}

void ALayerAdaptor::rand_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vrand_gpu = std::move(nval);
  l->rand_gpu = vrand_gpu.data();
}

void ALayerAdaptor::drop_blocks_scale(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdrop_blocks_scale = nval;
  l->drop_blocks_scale = vdrop_blocks_scale.data();
}

void ALayerAdaptor::drop_blocks_scale(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdrop_blocks_scale = std::move(nval);
  l->drop_blocks_scale = vdrop_blocks_scale.data();
}

void ALayerAdaptor::drop_blocks_scale_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdrop_blocks_scale_gpu = nval;
  l->drop_blocks_scale_gpu = vdrop_blocks_scale_gpu.data();
}

void ALayerAdaptor::drop_blocks_scale_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdrop_blocks_scale_gpu = std::move(nval);
  l->drop_blocks_scale_gpu = vdrop_blocks_scale_gpu.data();
}

void ALayerAdaptor::squared_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vsquared_gpu = nval;
  l->squared_gpu = vsquared_gpu.data();
}

void ALayerAdaptor::squared_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vsquared_gpu = std::move(nval);
  l->squared_gpu = vsquared_gpu.data();
}

void ALayerAdaptor::norms_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vnorms_gpu = nval;
  l->norms_gpu = vnorms_gpu.data();
}

void ALayerAdaptor::norms_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vnorms_gpu = std::move(nval);
  l->norms_gpu = vnorms_gpu.data();
}

void ALayerAdaptor::gt_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vgt_gpu = nval;
  l->gt_gpu = vgt_gpu.data();
}

void ALayerAdaptor::gt_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vgt_gpu = std::move(nval);
  l->gt_gpu = vgt_gpu.data();
}

void ALayerAdaptor::a_avg_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  va_avg_gpu = nval;
  l->a_avg_gpu = va_avg_gpu.data();
}

void ALayerAdaptor::a_avg_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  va_avg_gpu = std::move(nval);
  l->a_avg_gpu = va_avg_gpu.data();
}

void ALayerAdaptor::input_sizes_gpu(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vinput_sizes_gpu = nval;
  l->input_sizes_gpu = vinput_sizes_gpu.data();
}

void ALayerAdaptor::input_sizes_gpu(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vinput_sizes_gpu = std::move(nval);
  l->input_sizes_gpu = vinput_sizes_gpu.data();
}

void ALayerAdaptor::layers_output_gpu(const vvfloats& nval)
{
  assert(l != nullptr);
  vlayers_output_gpu = nval;

  place_pointers(vrlayers_output_gpu, vlayers_output_gpu);

  l->layers_output_gpu = vrlayers_output_gpu.data();
}

void ALayerAdaptor::layers_delta_gpu(const vvfloats& nval)
{
  assert(l != nullptr);
  vlayers_delta_gpu = nval;

  place_pointers(vrlayers_delta_gpu, vlayers_delta_gpu);

  l->layers_delta_gpu = vrlayers_delta_gpu.data();
}

}  // namespace darknet_adaptor::adaptors
