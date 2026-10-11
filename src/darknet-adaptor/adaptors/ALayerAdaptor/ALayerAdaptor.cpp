#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"

#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <utility>
#include <vector>

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

layerptr ALayerAdaptor::get()
{
  assert(l != nullptr);

  // An aliasing pointer: it points at the struct and owns the adaptor, which
  // owns the buffers the struct points at.
  return layerptr(shared_from_this(), l.get());
}

void ALayerAdaptor::bind(layerptr slot)
{
  assert(l != nullptr);
  assert(slot != nullptr);

  if (slot == l) {
    return;
  }

  *slot = *l;
  l = std::move(slot);
}

void ALayerAdaptor::relink()
{
  assert(l != nullptr);

  l->share_layer = layer_of(mshare_layer);

  l->input_layer = layer_of(minput_layer);
  l->self_layer = layer_of(mself_layer);
  l->output_layer = layer_of(moutput_layer);

  l->reset_layer = layer_of(mreset_layer);
  l->update_layer = layer_of(mupdate_layer);
  l->state_layer = layer_of(mstate_layer);

  l->input_gate_layer = layer_of(minput_gate_layer);
  l->state_gate_layer = layer_of(mstate_gate_layer);
  l->input_save_layer = layer_of(minput_save_layer);
  l->state_save_layer = layer_of(mstate_save_layer);
  l->input_state_layer = layer_of(minput_state_layer);
  l->state_state_layer = layer_of(mstate_state_layer);

  l->input_z_layer = layer_of(minput_z_layer);
  l->state_z_layer = layer_of(mstate_z_layer);

  l->input_r_layer = layer_of(minput_r_layer);
  l->state_r_layer = layer_of(mstate_r_layer);

  l->input_h_layer = layer_of(minput_h_layer);
  l->state_h_layer = layer_of(mstate_h_layer);

  l->wz = layer_of(mwz);
  l->uz = layer_of(muz);
  l->wr = layer_of(mwr);
  l->ur = layer_of(mur);
  l->wh = layer_of(mwh);
  l->uh = layer_of(muh);
  l->uo = layer_of(muo);
  l->wo = layer_of(mwo);
  l->vo = layer_of(mvo);
  l->uf = layer_of(muf);
  l->wf = layer_of(mwf);
  l->vf = layer_of(mvf);
  l->ui = layer_of(mui);
  l->wi = layer_of(mwi);
  l->vi = layer_of(mvi);
  l->ug = layer_of(mug);
  l->wg = layer_of(mwg);
}

bool ALayerAdaptor::borrows_batchnorm() const
{
  assert(l != nullptr);

  return l->rolling_mean != nullptr && vrolling_mean.empty();
}

layerptr ALayerAdaptor::create_layer()
{
  layerptr nlayer = std::make_shared<layer>();

  std::memset(nlayer.get(), 0, sizeof(layer));

  return nlayer;
}

layer* ALayerAdaptor::layer_of(const ALayerAdaptorPtr& adaptor)
{
  return adaptor ? adaptor->l.get() : nullptr;
}

LAYER_TYPE ALayerAdaptor::type() const
{
  assert(l != nullptr);
  return l->type;
}

ACTIVATION ALayerAdaptor::activation() const
{
  assert(l != nullptr);
  return l->activation;
}

ACTIVATION ALayerAdaptor::lstm_activation() const
{
  assert(l != nullptr);
  return l->lstm_activation;
}

COST_TYPE ALayerAdaptor::cost_type() const
{
  assert(l != nullptr);
  return l->cost_type;
}

ALayerAdaptor::forward_fn ALayerAdaptor::forward() const
{
  assert(l != nullptr);
  return l->forward;
}

ALayerAdaptor::backward_fn ALayerAdaptor::backward() const
{
  assert(l != nullptr);
  return l->backward;
}

ALayerAdaptor::update_fn ALayerAdaptor::update() const
{
  assert(l != nullptr);
  return l->update;
}

ALayerAdaptorPtr ALayerAdaptor::share_layer() const
{
  assert(l != nullptr);
  return mshare_layer;
}

int ALayerAdaptor::train() const
{
  assert(l != nullptr);
  return l->train;
}

int ALayerAdaptor::avgpool() const
{
  assert(l != nullptr);
  return l->avgpool;
}

int ALayerAdaptor::batch_normalize() const
{
  assert(l != nullptr);
  return l->batch_normalize;
}

int ALayerAdaptor::shortcut() const
{
  assert(l != nullptr);
  return l->shortcut;
}

int ALayerAdaptor::batch() const
{
  assert(l != nullptr);
  return l->batch;
}

int ALayerAdaptor::dynamic_minibatch() const
{
  assert(l != nullptr);
  return l->dynamic_minibatch;
}

int ALayerAdaptor::forced() const
{
  assert(l != nullptr);
  return l->forced;
}

int ALayerAdaptor::flipped() const
{
  assert(l != nullptr);
  return l->flipped;
}

int ALayerAdaptor::inputs() const
{
  assert(l != nullptr);
  return l->inputs;
}

int ALayerAdaptor::outputs() const
{
  assert(l != nullptr);
  return l->outputs;
}

float ALayerAdaptor::mean_alpha() const
{
  assert(l != nullptr);
  return l->mean_alpha;
}

int ALayerAdaptor::nweights() const
{
  assert(l != nullptr);
  return l->nweights;
}

int ALayerAdaptor::nbiases() const
{
  assert(l != nullptr);
  return l->nbiases;
}

int ALayerAdaptor::extra() const
{
  assert(l != nullptr);
  return l->extra;
}

int ALayerAdaptor::truths() const
{
  assert(l != nullptr);
  return l->truths;
}

int ALayerAdaptor::h() const
{
  assert(l != nullptr);
  return l->h;
}

int ALayerAdaptor::w() const
{
  assert(l != nullptr);
  return l->w;
}

int ALayerAdaptor::c() const
{
  assert(l != nullptr);
  return l->c;
}

int ALayerAdaptor::out_h() const
{
  assert(l != nullptr);
  return l->out_h;
}

int ALayerAdaptor::out_w() const
{
  assert(l != nullptr);
  return l->out_w;
}

int ALayerAdaptor::out_c() const
{
  assert(l != nullptr);
  return l->out_c;
}

int ALayerAdaptor::n() const
{
  assert(l != nullptr);
  return l->n;
}

int ALayerAdaptor::max_boxes() const
{
  assert(l != nullptr);
  return l->max_boxes;
}

int ALayerAdaptor::truth_size() const
{
  assert(l != nullptr);
  return l->truth_size;
}

int ALayerAdaptor::groups() const
{
  assert(l != nullptr);
  return l->groups;
}

int ALayerAdaptor::group_id() const
{
  assert(l != nullptr);
  return l->group_id;
}

int ALayerAdaptor::size() const
{
  assert(l != nullptr);
  return l->size;
}

int ALayerAdaptor::side() const
{
  assert(l != nullptr);
  return l->side;
}

int ALayerAdaptor::stride() const
{
  assert(l != nullptr);
  return l->stride;
}

int ALayerAdaptor::stride_x() const
{
  assert(l != nullptr);
  return l->stride_x;
}

int ALayerAdaptor::stride_y() const
{
  assert(l != nullptr);
  return l->stride_y;
}

int ALayerAdaptor::dilation() const
{
  assert(l != nullptr);
  return l->dilation;
}

int ALayerAdaptor::antialiasing() const
{
  assert(l != nullptr);
  return l->antialiasing;
}

int ALayerAdaptor::maxpool_depth() const
{
  assert(l != nullptr);
  return l->maxpool_depth;
}

int ALayerAdaptor::maxpool_zero_nonmax() const
{
  assert(l != nullptr);
  return l->maxpool_zero_nonmax;
}

int ALayerAdaptor::out_channels() const
{
  assert(l != nullptr);
  return l->out_channels;
}

float ALayerAdaptor::reverse() const
{
  assert(l != nullptr);
  return l->reverse;
}

int ALayerAdaptor::coordconv() const
{
  assert(l != nullptr);
  return l->coordconv;
}

int ALayerAdaptor::flatten() const
{
  assert(l != nullptr);
  return l->flatten;
}

int ALayerAdaptor::spatial() const
{
  assert(l != nullptr);
  return l->spatial;
}

int ALayerAdaptor::pad() const
{
  assert(l != nullptr);
  return l->pad;
}

int ALayerAdaptor::sqrt() const
{
  assert(l != nullptr);
  return l->sqrt;
}

int ALayerAdaptor::flip() const
{
  assert(l != nullptr);
  return l->flip;
}

int ALayerAdaptor::index() const
{
  assert(l != nullptr);
  return l->index;
}

int ALayerAdaptor::scale_wh() const
{
  assert(l != nullptr);
  return l->scale_wh;
}

int ALayerAdaptor::binary() const
{
  assert(l != nullptr);
  return l->binary;
}

int ALayerAdaptor::xnor() const
{
  assert(l != nullptr);
  return l->xnor;
}

int ALayerAdaptor::peephole() const
{
  assert(l != nullptr);
  return l->peephole;
}

int ALayerAdaptor::use_bin_output() const
{
  assert(l != nullptr);
  return l->use_bin_output;
}

int ALayerAdaptor::keep_delta_gpu() const
{
  assert(l != nullptr);
  return l->keep_delta_gpu;
}

int ALayerAdaptor::optimized_memory() const
{
  assert(l != nullptr);
  return l->optimized_memory;
}

int ALayerAdaptor::steps() const
{
  assert(l != nullptr);
  return l->steps;
}

int ALayerAdaptor::history_size() const
{
  assert(l != nullptr);
  return l->history_size;
}

int ALayerAdaptor::bottleneck() const
{
  assert(l != nullptr);
  return l->bottleneck;
}

float ALayerAdaptor::time_normalizer() const
{
  assert(l != nullptr);
  return l->time_normalizer;
}

int ALayerAdaptor::state_constrain() const
{
  assert(l != nullptr);
  return l->state_constrain;
}

int ALayerAdaptor::hidden() const
{
  assert(l != nullptr);
  return l->hidden;
}

int ALayerAdaptor::truth() const
{
  assert(l != nullptr);
  return l->truth;
}

float ALayerAdaptor::smooth() const
{
  assert(l != nullptr);
  return l->smooth;
}

float ALayerAdaptor::dot() const
{
  assert(l != nullptr);
  return l->dot;
}

int ALayerAdaptor::deform() const
{
  assert(l != nullptr);
  return l->deform;
}

int ALayerAdaptor::grad_centr() const
{
  assert(l != nullptr);
  return l->grad_centr;
}

int ALayerAdaptor::sway() const
{
  assert(l != nullptr);
  return l->sway;
}

int ALayerAdaptor::rotate() const
{
  assert(l != nullptr);
  return l->rotate;
}

int ALayerAdaptor::stretch() const
{
  assert(l != nullptr);
  return l->stretch;
}

int ALayerAdaptor::stretch_sway() const
{
  assert(l != nullptr);
  return l->stretch_sway;
}

float ALayerAdaptor::angle() const
{
  assert(l != nullptr);
  return l->angle;
}

float ALayerAdaptor::jitter() const
{
  assert(l != nullptr);
  return l->jitter;
}

float ALayerAdaptor::resize() const
{
  assert(l != nullptr);
  return l->resize;
}

float ALayerAdaptor::saturation() const
{
  assert(l != nullptr);
  return l->saturation;
}

float ALayerAdaptor::exposure() const
{
  assert(l != nullptr);
  return l->exposure;
}

float ALayerAdaptor::shift() const
{
  assert(l != nullptr);
  return l->shift;
}

float ALayerAdaptor::ratio() const
{
  assert(l != nullptr);
  return l->ratio;
}

float ALayerAdaptor::learning_rate_scale() const
{
  assert(l != nullptr);
  return l->learning_rate_scale;
}

float ALayerAdaptor::clip() const
{
  assert(l != nullptr);
  return l->clip;
}

int ALayerAdaptor::focal_loss() const
{
  assert(l != nullptr);
  return l->focal_loss;
}

const std::vector<float>& ALayerAdaptor::classes_multipliers() const
{
  assert(l != nullptr);
  return vclasses_multipliers;
}

float ALayerAdaptor::label_smooth_eps() const
{
  assert(l != nullptr);
  return l->label_smooth_eps;
}

int ALayerAdaptor::noloss() const
{
  assert(l != nullptr);
  return l->noloss;
}

int ALayerAdaptor::softmax() const
{
  assert(l != nullptr);
  return l->softmax;
}

int ALayerAdaptor::classes() const
{
  assert(l != nullptr);
  return l->classes;
}

int ALayerAdaptor::detection() const
{
  assert(l != nullptr);
  return l->detection;
}

int ALayerAdaptor::embedding_layer_id() const
{
  assert(l != nullptr);
  return l->embedding_layer_id;
}

const std::vector<float>& ALayerAdaptor::embedding_output() const
{
  assert(l != nullptr);
  return vembedding_output;
}

int ALayerAdaptor::embedding_size() const
{
  assert(l != nullptr);
  return l->embedding_size;
}

float ALayerAdaptor::sim_thresh() const
{
  assert(l != nullptr);
  return l->sim_thresh;
}

int ALayerAdaptor::track_history_size() const
{
  assert(l != nullptr);
  return l->track_history_size;
}

int ALayerAdaptor::dets_for_track() const
{
  assert(l != nullptr);
  return l->dets_for_track;
}

int ALayerAdaptor::dets_for_show() const
{
  assert(l != nullptr);
  return l->dets_for_show;
}

float ALayerAdaptor::track_ciou_norm() const
{
  assert(l != nullptr);
  return l->track_ciou_norm;
}

int ALayerAdaptor::coords() const
{
  assert(l != nullptr);
  return l->coords;
}

int ALayerAdaptor::background() const
{
  assert(l != nullptr);
  return l->background;
}

int ALayerAdaptor::rescore() const
{
  assert(l != nullptr);
  return l->rescore;
}

int ALayerAdaptor::objectness() const
{
  assert(l != nullptr);
  return l->objectness;
}

int ALayerAdaptor::does_cost() const
{
  assert(l != nullptr);
  return l->does_cost;
}

int ALayerAdaptor::joint() const
{
  assert(l != nullptr);
  return l->joint;
}

int ALayerAdaptor::noadjust() const
{
  assert(l != nullptr);
  return l->noadjust;
}

int ALayerAdaptor::reorg() const
{
  assert(l != nullptr);
  return l->reorg;
}

int ALayerAdaptor::log() const
{
  assert(l != nullptr);
  return l->log;
}

int ALayerAdaptor::tanh() const
{
  assert(l != nullptr);
  return l->tanh;
}

const std::vector<int>& ALayerAdaptor::mask() const
{
  assert(l != nullptr);
  return vmask;
}

int ALayerAdaptor::total() const
{
  assert(l != nullptr);
  return l->total;
}

float ALayerAdaptor::bflops() const
{
  assert(l != nullptr);
  return l->bflops;
}

int ALayerAdaptor::adam() const
{
  assert(l != nullptr);
  return l->adam;
}

float ALayerAdaptor::B1() const
{
  assert(l != nullptr);
  return l->B1;
}

float ALayerAdaptor::B2() const
{
  assert(l != nullptr);
  return l->B2;
}

float ALayerAdaptor::eps() const
{
  assert(l != nullptr);
  return l->eps;
}

int ALayerAdaptor::t() const
{
  assert(l != nullptr);
  return l->t;
}

float ALayerAdaptor::alpha() const
{
  assert(l != nullptr);
  return l->alpha;
}

float ALayerAdaptor::beta() const
{
  assert(l != nullptr);
  return l->beta;
}

float ALayerAdaptor::kappa() const
{
  assert(l != nullptr);
  return l->kappa;
}

float ALayerAdaptor::coord_scale() const
{
  assert(l != nullptr);
  return l->coord_scale;
}

float ALayerAdaptor::object_scale() const
{
  assert(l != nullptr);
  return l->object_scale;
}

float ALayerAdaptor::noobject_scale() const
{
  assert(l != nullptr);
  return l->noobject_scale;
}

float ALayerAdaptor::mask_scale() const
{
  assert(l != nullptr);
  return l->mask_scale;
}

float ALayerAdaptor::class_scale() const
{
  assert(l != nullptr);
  return l->class_scale;
}

int ALayerAdaptor::bias_match() const
{
  assert(l != nullptr);
  return l->bias_match;
}

float ALayerAdaptor::random() const
{
  assert(l != nullptr);
  return l->random;
}

float ALayerAdaptor::ignore_thresh() const
{
  assert(l != nullptr);
  return l->ignore_thresh;
}

float ALayerAdaptor::truth_thresh() const
{
  assert(l != nullptr);
  return l->truth_thresh;
}

float ALayerAdaptor::iou_thresh() const
{
  assert(l != nullptr);
  return l->iou_thresh;
}

float ALayerAdaptor::thresh() const
{
  assert(l != nullptr);
  return l->thresh;
}

float ALayerAdaptor::focus() const
{
  assert(l != nullptr);
  return l->focus;
}

int ALayerAdaptor::classfix() const
{
  assert(l != nullptr);
  return l->classfix;
}

int ALayerAdaptor::absolute() const
{
  assert(l != nullptr);
  return l->absolute;
}

int ALayerAdaptor::assisted_excitation() const
{
  assert(l != nullptr);
  return l->assisted_excitation;
}

int ALayerAdaptor::onlyforward() const
{
  assert(l != nullptr);
  return l->onlyforward;
}

int ALayerAdaptor::stopbackward() const
{
  assert(l != nullptr);
  return l->stopbackward;
}

int ALayerAdaptor::train_only_bn() const
{
  assert(l != nullptr);
  return l->train_only_bn;
}

int ALayerAdaptor::dont_update() const
{
  assert(l != nullptr);
  return l->dont_update;
}

int ALayerAdaptor::burnin_update() const
{
  assert(l != nullptr);
  return l->burnin_update;
}

int ALayerAdaptor::dontload() const
{
  assert(l != nullptr);
  return l->dontload;
}

int ALayerAdaptor::dontsave() const
{
  assert(l != nullptr);
  return l->dontsave;
}

int ALayerAdaptor::dontloadscales() const
{
  assert(l != nullptr);
  return l->dontloadscales;
}

int ALayerAdaptor::numload() const
{
  assert(l != nullptr);
  return l->numload;
}

float ALayerAdaptor::temperature() const
{
  assert(l != nullptr);
  return l->temperature;
}

float ALayerAdaptor::probability() const
{
  assert(l != nullptr);
  return l->probability;
}

float ALayerAdaptor::dropblock_size_rel() const
{
  assert(l != nullptr);
  return l->dropblock_size_rel;
}

int ALayerAdaptor::dropblock_size_abs() const
{
  assert(l != nullptr);
  return l->dropblock_size_abs;
}

int ALayerAdaptor::dropblock() const
{
  assert(l != nullptr);
  return l->dropblock;
}

float ALayerAdaptor::scale() const
{
  assert(l != nullptr);
  return l->scale;
}

int ALayerAdaptor::receptive_w() const
{
  assert(l != nullptr);
  return l->receptive_w;
}

int ALayerAdaptor::receptive_h() const
{
  assert(l != nullptr);
  return l->receptive_h;
}

int ALayerAdaptor::receptive_w_scale() const
{
  assert(l != nullptr);
  return l->receptive_w_scale;
}

int ALayerAdaptor::receptive_h_scale() const
{
  assert(l != nullptr);
  return l->receptive_h_scale;
}

const std::string& ALayerAdaptor::cweights() const
{
  assert(l != nullptr);
  return scweights;
}

const std::vector<int>& ALayerAdaptor::indexes() const
{
  assert(l != nullptr);
  return vindexes;
}

const std::vector<int>& ALayerAdaptor::input_layers() const
{
  assert(l != nullptr);
  return vinput_layers;
}

const std::vector<int>& ALayerAdaptor::input_sizes() const
{
  assert(l != nullptr);
  return vinput_sizes;
}

const ALayerAdaptor::vrfloats& ALayerAdaptor::layers_output() const
{
  assert(l != nullptr);
  return vrlayers_output;
}

const ALayerAdaptor::vrfloats& ALayerAdaptor::layers_delta() const
{
  assert(l != nullptr);
  return vrlayers_delta;
}

WEIGHTS_TYPE_T ALayerAdaptor::weights_type() const
{
  assert(l != nullptr);
  return l->weights_type;
}

WEIGHTS_NORMALIZATION_T ALayerAdaptor::weights_normalization() const
{
  assert(l != nullptr);
  return l->weights_normalization;
}

const std::vector<int>& ALayerAdaptor::map() const
{
  assert(l != nullptr);
  return vmap;
}

const std::vector<int>& ALayerAdaptor::counts() const
{
  assert(l != nullptr);
  return vcounts;
}

const ALayerAdaptor::vvfloats& ALayerAdaptor::sums() const
{
  assert(l != nullptr);
  return vsums;
}

const std::vector<float>& ALayerAdaptor::rand() const
{
  assert(l != nullptr);
  return vrand;
}

const std::vector<float>& ALayerAdaptor::cost() const
{
  assert(l != nullptr);
  return vcost;
}

const std::vector<int>& ALayerAdaptor::labels() const
{
  assert(l != nullptr);
  return vlabels;
}

const std::vector<int>& ALayerAdaptor::class_ids() const
{
  assert(l != nullptr);
  return vclass_ids;
}

int ALayerAdaptor::contrastive_neg_max() const
{
  assert(l != nullptr);
  return l->contrastive_neg_max;
}

const std::vector<float>& ALayerAdaptor::cos_sim() const
{
  assert(l != nullptr);
  return vcos_sim;
}

const std::vector<float>& ALayerAdaptor::exp_cos_sim() const
{
  assert(l != nullptr);
  return vexp_cos_sim;
}

const std::vector<float>& ALayerAdaptor::p_constrastive() const
{
  assert(l != nullptr);
  return vp_constrastive;
}

const ALayerAdaptor::vcontrastive_params& ALayerAdaptor::contrast_p_gpu() const
{
  assert(l != nullptr);
  return vcontrast_p_gpu;
}

const std::vector<float>& ALayerAdaptor::state() const
{
  assert(l != nullptr);
  return vstate;
}

const std::vector<float>& ALayerAdaptor::prev_state() const
{
  assert(l != nullptr);
  return vprev_state;
}

const std::vector<float>& ALayerAdaptor::forgot_state() const
{
  assert(l != nullptr);
  return vforgot_state;
}

const std::vector<float>& ALayerAdaptor::forgot_delta() const
{
  assert(l != nullptr);
  return vforgot_delta;
}

const std::vector<float>& ALayerAdaptor::state_delta() const
{
  assert(l != nullptr);
  return vstate_delta;
}

const std::vector<float>& ALayerAdaptor::combine_cpu() const
{
  assert(l != nullptr);
  return vcombine_cpu;
}

const std::vector<float>& ALayerAdaptor::combine_delta_cpu() const
{
  assert(l != nullptr);
  return vcombine_delta_cpu;
}

const std::vector<float>& ALayerAdaptor::concat() const
{
  assert(l != nullptr);
  return vconcat;
}

const std::vector<float>& ALayerAdaptor::concat_delta() const
{
  assert(l != nullptr);
  return vconcat_delta;
}

const std::vector<float>& ALayerAdaptor::binary_weights() const
{
  assert(l != nullptr);
  return vbinary_weights;
}

const std::vector<float>& ALayerAdaptor::biases() const
{
  assert(l != nullptr);
  return vbiases;
}

const std::vector<float>& ALayerAdaptor::bias_updates() const
{
  assert(l != nullptr);
  return vbias_updates;
}

const std::vector<float>& ALayerAdaptor::scales() const
{
  assert(l != nullptr);
  return vscales;
}

const std::vector<float>& ALayerAdaptor::scale_updates() const
{
  assert(l != nullptr);
  return vscale_updates;
}

const std::vector<float>& ALayerAdaptor::weights_ema() const
{
  assert(l != nullptr);
  return vweights_ema;
}

const std::vector<float>& ALayerAdaptor::biases_ema() const
{
  assert(l != nullptr);
  return vbiases_ema;
}

const std::vector<float>& ALayerAdaptor::scales_ema() const
{
  assert(l != nullptr);
  return vscales_ema;
}

const std::vector<float>& ALayerAdaptor::weights() const
{
  assert(l != nullptr);
  return vweights;
}

const std::vector<float>& ALayerAdaptor::weight_updates() const
{
  assert(l != nullptr);
  return vweight_updates;
}

float ALayerAdaptor::scale_x_y() const
{
  assert(l != nullptr);
  return l->scale_x_y;
}

int ALayerAdaptor::objectness_smooth() const
{
  assert(l != nullptr);
  return l->objectness_smooth;
}

int ALayerAdaptor::new_coords() const
{
  assert(l != nullptr);
  return l->new_coords;
}

int ALayerAdaptor::show_details() const
{
  assert(l != nullptr);
  return l->show_details;
}

float ALayerAdaptor::max_delta() const
{
  assert(l != nullptr);
  return l->max_delta;
}

float ALayerAdaptor::uc_normalizer() const
{
  assert(l != nullptr);
  return l->uc_normalizer;
}

float ALayerAdaptor::iou_normalizer() const
{
  assert(l != nullptr);
  return l->iou_normalizer;
}

float ALayerAdaptor::obj_normalizer() const
{
  assert(l != nullptr);
  return l->obj_normalizer;
}

float ALayerAdaptor::cls_normalizer() const
{
  assert(l != nullptr);
  return l->cls_normalizer;
}

float ALayerAdaptor::delta_normalizer() const
{
  assert(l != nullptr);
  return l->delta_normalizer;
}

IOU_LOSS ALayerAdaptor::iou_loss() const
{
  assert(l != nullptr);
  return l->iou_loss;
}

IOU_LOSS ALayerAdaptor::iou_thresh_kind() const
{
  assert(l != nullptr);
  return l->iou_thresh_kind;
}

NMS_KIND ALayerAdaptor::nms_kind() const
{
  assert(l != nullptr);
  return l->nms_kind;
}

float ALayerAdaptor::beta_nms() const
{
  assert(l != nullptr);
  return l->beta_nms;
}

YOLO_POINT ALayerAdaptor::yolo_point() const
{
  assert(l != nullptr);
  return l->yolo_point;
}

const std::string& ALayerAdaptor::align_bit_weights_gpu() const
{
  assert(l != nullptr);
  return salign_bit_weights_gpu;
}

const std::vector<float>& ALayerAdaptor::mean_arr_gpu() const
{
  assert(l != nullptr);
  return vmean_arr_gpu;
}

const std::vector<float>& ALayerAdaptor::align_workspace_gpu() const
{
  assert(l != nullptr);
  return valign_workspace_gpu;
}

const std::vector<float>& ALayerAdaptor::transposed_align_workspace_gpu() const
{
  assert(l != nullptr);
  return vtransposed_align_workspace_gpu;
}

int ALayerAdaptor::align_workspace_size() const
{
  assert(l != nullptr);
  return l->align_workspace_size;
}

const std::string& ALayerAdaptor::align_bit_weights() const
{
  assert(l != nullptr);
  return salign_bit_weights;
}

const std::vector<float>& ALayerAdaptor::mean_arr() const
{
  assert(l != nullptr);
  return vmean_arr;
}

int ALayerAdaptor::align_bit_weights_size() const
{
  assert(l != nullptr);
  return l->align_bit_weights_size;
}

int ALayerAdaptor::lda_align() const
{
  assert(l != nullptr);
  return l->lda_align;
}

int ALayerAdaptor::new_lda() const
{
  assert(l != nullptr);
  return l->new_lda;
}

int ALayerAdaptor::bit_align() const
{
  assert(l != nullptr);
  return l->bit_align;
}

const std::vector<float>& ALayerAdaptor::col_image() const
{
  assert(l != nullptr);
  return vcol_image;
}

const std::vector<float>& ALayerAdaptor::delta() const
{
  assert(l != nullptr);
  return vdelta;
}

const std::vector<float>& ALayerAdaptor::output() const
{
  assert(l != nullptr);
  return voutput;
}

const std::vector<float>& ALayerAdaptor::activation_input() const
{
  assert(l != nullptr);
  return vactivation_input;
}

int ALayerAdaptor::delta_pinned() const
{
  assert(l != nullptr);
  return l->delta_pinned;
}

int ALayerAdaptor::output_pinned() const
{
  assert(l != nullptr);
  return l->output_pinned;
}

const std::vector<float>& ALayerAdaptor::loss() const
{
  assert(l != nullptr);
  return vloss;
}

const std::vector<float>& ALayerAdaptor::squared() const
{
  assert(l != nullptr);
  return vsquared;
}

const std::vector<float>& ALayerAdaptor::norms() const
{
  assert(l != nullptr);
  return vnorms;
}

const std::vector<float>& ALayerAdaptor::spatial_mean() const
{
  assert(l != nullptr);
  return vspatial_mean;
}

const std::vector<float>& ALayerAdaptor::mean() const
{
  assert(l != nullptr);
  return vmean;
}

const std::vector<float>& ALayerAdaptor::variance() const
{
  assert(l != nullptr);
  return vvariance;
}

const std::vector<float>& ALayerAdaptor::mean_delta() const
{
  assert(l != nullptr);
  return vmean_delta;
}

const std::vector<float>& ALayerAdaptor::variance_delta() const
{
  assert(l != nullptr);
  return vvariance_delta;
}

const std::vector<float>& ALayerAdaptor::rolling_mean() const
{
  assert(l != nullptr);
  return vrolling_mean;
}

const std::vector<float>& ALayerAdaptor::rolling_variance() const
{
  assert(l != nullptr);
  return vrolling_variance;
}

const std::vector<float>& ALayerAdaptor::x() const
{
  assert(l != nullptr);
  return vx;
}

const std::vector<float>& ALayerAdaptor::x_norm() const
{
  assert(l != nullptr);
  return vx_norm;
}

const std::vector<float>& ALayerAdaptor::m() const
{
  assert(l != nullptr);
  return vm;
}

const std::vector<float>& ALayerAdaptor::v() const
{
  assert(l != nullptr);
  return vv;
}

const std::vector<float>& ALayerAdaptor::bias_m() const
{
  assert(l != nullptr);
  return vbias_m;
}

const std::vector<float>& ALayerAdaptor::bias_v() const
{
  assert(l != nullptr);
  return vbias_v;
}

const std::vector<float>& ALayerAdaptor::scale_m() const
{
  assert(l != nullptr);
  return vscale_m;
}

const std::vector<float>& ALayerAdaptor::scale_v() const
{
  assert(l != nullptr);
  return vscale_v;
}

const std::vector<float>& ALayerAdaptor::z_cpu() const
{
  assert(l != nullptr);
  return vz_cpu;
}

const std::vector<float>& ALayerAdaptor::r_cpu() const
{
  assert(l != nullptr);
  return vr_cpu;
}

const std::vector<float>& ALayerAdaptor::h_cpu() const
{
  assert(l != nullptr);
  return vh_cpu;
}

const std::vector<float>& ALayerAdaptor::stored_h_cpu() const
{
  assert(l != nullptr);
  return vstored_h_cpu;
}

const std::vector<float>& ALayerAdaptor::prev_state_cpu() const
{
  assert(l != nullptr);
  return vprev_state_cpu;
}

const std::vector<float>& ALayerAdaptor::temp_cpu() const
{
  assert(l != nullptr);
  return vtemp_cpu;
}

const std::vector<float>& ALayerAdaptor::temp2_cpu() const
{
  assert(l != nullptr);
  return vtemp2_cpu;
}

const std::vector<float>& ALayerAdaptor::temp3_cpu() const
{
  assert(l != nullptr);
  return vtemp3_cpu;
}

const std::vector<float>& ALayerAdaptor::dh_cpu() const
{
  assert(l != nullptr);
  return vdh_cpu;
}

const std::vector<float>& ALayerAdaptor::hh_cpu() const
{
  assert(l != nullptr);
  return vhh_cpu;
}

const std::vector<float>& ALayerAdaptor::prev_cell_cpu() const
{
  assert(l != nullptr);
  return vprev_cell_cpu;
}

const std::vector<float>& ALayerAdaptor::cell_cpu() const
{
  assert(l != nullptr);
  return vcell_cpu;
}

const std::vector<float>& ALayerAdaptor::f_cpu() const
{
  assert(l != nullptr);
  return vf_cpu;
}

const std::vector<float>& ALayerAdaptor::i_cpu() const
{
  assert(l != nullptr);
  return vi_cpu;
}

const std::vector<float>& ALayerAdaptor::g_cpu() const
{
  assert(l != nullptr);
  return vg_cpu;
}

const std::vector<float>& ALayerAdaptor::o_cpu() const
{
  assert(l != nullptr);
  return vo_cpu;
}

const std::vector<float>& ALayerAdaptor::c_cpu() const
{
  assert(l != nullptr);
  return vc_cpu;
}

const std::vector<float>& ALayerAdaptor::stored_c_cpu() const
{
  assert(l != nullptr);
  return vstored_c_cpu;
}

const std::vector<float>& ALayerAdaptor::dc_cpu() const
{
  assert(l != nullptr);
  return vdc_cpu;
}

const std::vector<float>& ALayerAdaptor::binary_input() const
{
  assert(l != nullptr);
  return vbinary_input;
}

const std::vector<uint32_t>& ALayerAdaptor::bin_re_packed_input() const
{
  assert(l != nullptr);
  return vbin_re_packed_input;
}

const std::string& ALayerAdaptor::t_bit_input() const
{
  assert(l != nullptr);
  return st_bit_input;
}

ALayerAdaptorPtr ALayerAdaptor::input_layer() const
{
  assert(l != nullptr);
  return minput_layer;
}

ALayerAdaptorPtr ALayerAdaptor::self_layer() const
{
  assert(l != nullptr);
  return mself_layer;
}

ALayerAdaptorPtr ALayerAdaptor::output_layer() const
{
  assert(l != nullptr);
  return moutput_layer;
}

ALayerAdaptorPtr ALayerAdaptor::reset_layer() const
{
  assert(l != nullptr);
  return mreset_layer;
}

ALayerAdaptorPtr ALayerAdaptor::update_layer() const
{
  assert(l != nullptr);
  return mupdate_layer;
}

ALayerAdaptorPtr ALayerAdaptor::state_layer() const
{
  assert(l != nullptr);
  return mstate_layer;
}

ALayerAdaptorPtr ALayerAdaptor::input_gate_layer() const
{
  assert(l != nullptr);
  return minput_gate_layer;
}

ALayerAdaptorPtr ALayerAdaptor::state_gate_layer() const
{
  assert(l != nullptr);
  return mstate_gate_layer;
}

ALayerAdaptorPtr ALayerAdaptor::input_save_layer() const
{
  assert(l != nullptr);
  return minput_save_layer;
}

ALayerAdaptorPtr ALayerAdaptor::state_save_layer() const
{
  assert(l != nullptr);
  return mstate_save_layer;
}

ALayerAdaptorPtr ALayerAdaptor::input_state_layer() const
{
  assert(l != nullptr);
  return minput_state_layer;
}

ALayerAdaptorPtr ALayerAdaptor::state_state_layer() const
{
  assert(l != nullptr);
  return mstate_state_layer;
}

ALayerAdaptorPtr ALayerAdaptor::input_z_layer() const
{
  assert(l != nullptr);
  return minput_z_layer;
}

ALayerAdaptorPtr ALayerAdaptor::state_z_layer() const
{
  assert(l != nullptr);
  return mstate_z_layer;
}

ALayerAdaptorPtr ALayerAdaptor::input_r_layer() const
{
  assert(l != nullptr);
  return minput_r_layer;
}

ALayerAdaptorPtr ALayerAdaptor::state_r_layer() const
{
  assert(l != nullptr);
  return mstate_r_layer;
}

ALayerAdaptorPtr ALayerAdaptor::input_h_layer() const
{
  assert(l != nullptr);
  return minput_h_layer;
}

ALayerAdaptorPtr ALayerAdaptor::state_h_layer() const
{
  assert(l != nullptr);
  return mstate_h_layer;
}

ALayerAdaptorPtr ALayerAdaptor::wz() const
{
  assert(l != nullptr);
  return mwz;
}

ALayerAdaptorPtr ALayerAdaptor::uz() const
{
  assert(l != nullptr);
  return muz;
}

ALayerAdaptorPtr ALayerAdaptor::wr() const
{
  assert(l != nullptr);
  return mwr;
}

ALayerAdaptorPtr ALayerAdaptor::ur() const
{
  assert(l != nullptr);
  return mur;
}

ALayerAdaptorPtr ALayerAdaptor::wh() const
{
  assert(l != nullptr);
  return mwh;
}

ALayerAdaptorPtr ALayerAdaptor::uh() const
{
  assert(l != nullptr);
  return muh;
}

ALayerAdaptorPtr ALayerAdaptor::uo() const
{
  assert(l != nullptr);
  return muo;
}

ALayerAdaptorPtr ALayerAdaptor::wo() const
{
  assert(l != nullptr);
  return mwo;
}

ALayerAdaptorPtr ALayerAdaptor::vo() const
{
  assert(l != nullptr);
  return mvo;
}

ALayerAdaptorPtr ALayerAdaptor::uf() const
{
  assert(l != nullptr);
  return muf;
}

ALayerAdaptorPtr ALayerAdaptor::wf() const
{
  assert(l != nullptr);
  return mwf;
}

ALayerAdaptorPtr ALayerAdaptor::vf() const
{
  assert(l != nullptr);
  return mvf;
}

ALayerAdaptorPtr ALayerAdaptor::ui() const
{
  assert(l != nullptr);
  return mui;
}

ALayerAdaptorPtr ALayerAdaptor::wi() const
{
  assert(l != nullptr);
  return mwi;
}

ALayerAdaptorPtr ALayerAdaptor::vi() const
{
  assert(l != nullptr);
  return mvi;
}

ALayerAdaptorPtr ALayerAdaptor::ug() const
{
  assert(l != nullptr);
  return mug;
}

ALayerAdaptorPtr ALayerAdaptor::wg() const
{
  assert(l != nullptr);
  return mwg;
}

tree* ALayerAdaptor::softmax_tree() const
{
  assert(l != nullptr);
  return l->softmax_tree;
}

size_t ALayerAdaptor::workspace_size() const
{
  assert(l != nullptr);
  return l->workspace_size;
}

// #ifdef GPU
const std::vector<int>& ALayerAdaptor::indexes_gpu() const
{
  assert(l != nullptr);
  return vindexes_gpu;
}

int ALayerAdaptor::stream() const
{
  assert(l != nullptr);
  return l->stream;
}

int ALayerAdaptor::wait_stream_id() const
{
  assert(l != nullptr);
  return l->wait_stream_id;
}

const std::vector<float>& ALayerAdaptor::z_gpu() const
{
  assert(l != nullptr);
  return vz_gpu;
}

const std::vector<float>& ALayerAdaptor::r_gpu() const
{
  assert(l != nullptr);
  return vr_gpu;
}

const std::vector<float>& ALayerAdaptor::h_gpu() const
{
  assert(l != nullptr);
  return vh_gpu;
}

const std::vector<float>& ALayerAdaptor::stored_h_gpu() const
{
  assert(l != nullptr);
  return vstored_h_gpu;
}

const std::vector<float>& ALayerAdaptor::bottelneck_hi_gpu() const
{
  assert(l != nullptr);
  return vbottelneck_hi_gpu;
}

const std::vector<float>& ALayerAdaptor::bottelneck_delta_gpu() const
{
  assert(l != nullptr);
  return vbottelneck_delta_gpu;
}

const std::vector<float>& ALayerAdaptor::temp_gpu() const
{
  assert(l != nullptr);
  return vtemp_gpu;
}

const std::vector<float>& ALayerAdaptor::temp2_gpu() const
{
  assert(l != nullptr);
  return vtemp2_gpu;
}

const std::vector<float>& ALayerAdaptor::temp3_gpu() const
{
  assert(l != nullptr);
  return vtemp3_gpu;
}

const std::vector<float>& ALayerAdaptor::dh_gpu() const
{
  assert(l != nullptr);
  return vdh_gpu;
}

const std::vector<float>& ALayerAdaptor::hh_gpu() const
{
  assert(l != nullptr);
  return vhh_gpu;
}

const std::vector<float>& ALayerAdaptor::prev_cell_gpu() const
{
  assert(l != nullptr);
  return vprev_cell_gpu;
}

const std::vector<float>& ALayerAdaptor::prev_state_gpu() const
{
  assert(l != nullptr);
  return vprev_state_gpu;
}

const std::vector<float>& ALayerAdaptor::last_prev_state_gpu() const
{
  assert(l != nullptr);
  return vlast_prev_state_gpu;
}

const std::vector<float>& ALayerAdaptor::last_prev_cell_gpu() const
{
  assert(l != nullptr);
  return vlast_prev_cell_gpu;
}

const std::vector<float>& ALayerAdaptor::cell_gpu() const
{
  assert(l != nullptr);
  return vcell_gpu;
}

const std::vector<float>& ALayerAdaptor::f_gpu() const
{
  assert(l != nullptr);
  return vf_gpu;
}

const std::vector<float>& ALayerAdaptor::i_gpu() const
{
  assert(l != nullptr);
  return vi_gpu;
}

const std::vector<float>& ALayerAdaptor::g_gpu() const
{
  assert(l != nullptr);
  return vg_gpu;
}

const std::vector<float>& ALayerAdaptor::o_gpu() const
{
  assert(l != nullptr);
  return vo_gpu;
}

const std::vector<float>& ALayerAdaptor::c_gpu() const
{
  assert(l != nullptr);
  return vc_gpu;
}

const std::vector<float>& ALayerAdaptor::stored_c_gpu() const
{
  assert(l != nullptr);
  return vstored_c_gpu;
}

const std::vector<float>& ALayerAdaptor::dc_gpu() const
{
  assert(l != nullptr);
  return vdc_gpu;
}

// adam
const std::vector<float>& ALayerAdaptor::m_gpu() const
{
  assert(l != nullptr);
  return vm_gpu;
}

const std::vector<float>& ALayerAdaptor::v_gpu() const
{
  assert(l != nullptr);
  return vv_gpu;
}

const std::vector<float>& ALayerAdaptor::bias_m_gpu() const
{
  assert(l != nullptr);
  return vbias_m_gpu;
}

const std::vector<float>& ALayerAdaptor::scale_m_gpu() const
{
  assert(l != nullptr);
  return vscale_m_gpu;
}

const std::vector<float>& ALayerAdaptor::bias_v_gpu() const
{
  assert(l != nullptr);
  return vbias_v_gpu;
}

const std::vector<float>& ALayerAdaptor::scale_v_gpu() const
{
  assert(l != nullptr);
  return vscale_v_gpu;
}

const std::vector<float>& ALayerAdaptor::combine_gpu() const
{
  assert(l != nullptr);
  return vcombine_gpu;
}

const std::vector<float>& ALayerAdaptor::combine_delta_gpu() const
{
  assert(l != nullptr);
  return vcombine_delta_gpu;
}

const std::vector<float>& ALayerAdaptor::forgot_state_gpu() const
{
  assert(l != nullptr);
  return vforgot_state_gpu;
}

const std::vector<float>& ALayerAdaptor::forgot_delta_gpu() const
{
  assert(l != nullptr);
  return vforgot_delta_gpu;
}

const std::vector<float>& ALayerAdaptor::state_gpu() const
{
  assert(l != nullptr);
  return vstate_gpu;
}

const std::vector<float>& ALayerAdaptor::state_delta_gpu() const
{
  assert(l != nullptr);
  return vstate_delta_gpu;
}

const std::vector<float>& ALayerAdaptor::gate_gpu() const
{
  assert(l != nullptr);
  return vgate_gpu;
}

const std::vector<float>& ALayerAdaptor::gate_delta_gpu() const
{
  assert(l != nullptr);
  return vgate_delta_gpu;
}

const std::vector<float>& ALayerAdaptor::save_gpu() const
{
  assert(l != nullptr);
  return vsave_gpu;
}

const std::vector<float>& ALayerAdaptor::save_delta_gpu() const
{
  assert(l != nullptr);
  return vsave_delta_gpu;
}

const std::vector<float>& ALayerAdaptor::concat_gpu() const
{
  assert(l != nullptr);
  return vconcat_gpu;
}

const std::vector<float>& ALayerAdaptor::concat_delta_gpu() const
{
  assert(l != nullptr);
  return vconcat_delta_gpu;
}

const std::vector<float>& ALayerAdaptor::binary_input_gpu() const
{
  assert(l != nullptr);
  return vbinary_input_gpu;
}

const std::vector<float>& ALayerAdaptor::binary_weights_gpu() const
{
  assert(l != nullptr);
  return vbinary_weights_gpu;
}

const std::vector<float>& ALayerAdaptor::bin_conv_shortcut_in_gpu() const
{
  assert(l != nullptr);
  return vbin_conv_shortcut_in_gpu;
}

const std::vector<float>& ALayerAdaptor::bin_conv_shortcut_out_gpu() const
{
  assert(l != nullptr);
  return vbin_conv_shortcut_out_gpu;
}

const std::vector<float>& ALayerAdaptor::mean_gpu() const
{
  assert(l != nullptr);
  return vmean_gpu;
}

const std::vector<float>& ALayerAdaptor::variance_gpu() const
{
  assert(l != nullptr);
  return vvariance_gpu;
}

const std::vector<float>& ALayerAdaptor::m_cbn_avg_gpu() const
{
  assert(l != nullptr);
  return vm_cbn_avg_gpu;
}

const std::vector<float>& ALayerAdaptor::v_cbn_avg_gpu() const
{
  assert(l != nullptr);
  return vv_cbn_avg_gpu;
}

const std::vector<float>& ALayerAdaptor::rolling_mean_gpu() const
{
  assert(l != nullptr);
  return vrolling_mean_gpu;
}

const std::vector<float>& ALayerAdaptor::rolling_variance_gpu() const
{
  assert(l != nullptr);
  return vrolling_variance_gpu;
}

const std::vector<float>& ALayerAdaptor::variance_delta_gpu() const
{
  assert(l != nullptr);
  return vvariance_delta_gpu;
}

const std::vector<float>& ALayerAdaptor::mean_delta_gpu() const
{
  assert(l != nullptr);
  return vmean_delta_gpu;
}

const std::vector<float>& ALayerAdaptor::col_image_gpu() const
{
  assert(l != nullptr);
  return vcol_image_gpu;
}

const std::vector<float>& ALayerAdaptor::x_gpu() const
{
  assert(l != nullptr);
  return vx_gpu;
}

const std::vector<float>& ALayerAdaptor::x_norm_gpu() const
{
  assert(l != nullptr);
  return vx_norm_gpu;
}

const std::vector<float>& ALayerAdaptor::weights_gpu() const
{
  assert(l != nullptr);
  return vweights_gpu;
}

const std::vector<float>& ALayerAdaptor::weight_updates_gpu() const
{
  assert(l != nullptr);
  return vweight_updates_gpu;
}

const std::vector<float>& ALayerAdaptor::weight_deform_gpu() const
{
  assert(l != nullptr);
  return vweight_deform_gpu;
}

const std::vector<float>& ALayerAdaptor::weight_change_gpu() const
{
  assert(l != nullptr);
  return vweight_change_gpu;
}

const std::vector<float>& ALayerAdaptor::weights_gpu16() const
{
  assert(l != nullptr);
  return vweights_gpu16;
}

const std::vector<float>& ALayerAdaptor::weight_updates_gpu16() const
{
  assert(l != nullptr);
  return vweight_updates_gpu16;
}

const std::vector<float>& ALayerAdaptor::biases_gpu() const
{
  assert(l != nullptr);
  return vbiases_gpu;
}

const std::vector<float>& ALayerAdaptor::bias_updates_gpu() const
{
  assert(l != nullptr);
  return vbias_updates_gpu;
}

const std::vector<float>& ALayerAdaptor::bias_change_gpu() const
{
  assert(l != nullptr);
  return vbias_change_gpu;
}

const std::vector<float>& ALayerAdaptor::scales_gpu() const
{
  assert(l != nullptr);
  return vscales_gpu;
}

const std::vector<float>& ALayerAdaptor::scale_updates_gpu() const
{
  assert(l != nullptr);
  return vscale_updates_gpu;
}

const std::vector<float>& ALayerAdaptor::scale_change_gpu() const
{
  assert(l != nullptr);
  return vscale_change_gpu;
}

const std::vector<float>& ALayerAdaptor::input_antialiasing_gpu() const
{
  assert(l != nullptr);
  return vinput_antialiasing_gpu;
}

const std::vector<float>& ALayerAdaptor::output_gpu() const
{
  assert(l != nullptr);
  return voutput_gpu;
}

const std::vector<float>& ALayerAdaptor::output_avg_gpu() const
{
  assert(l != nullptr);
  return voutput_avg_gpu;
}

const std::vector<float>& ALayerAdaptor::activation_input_gpu() const
{
  assert(l != nullptr);
  return vactivation_input_gpu;
}

const std::vector<float>& ALayerAdaptor::loss_gpu() const
{
  assert(l != nullptr);
  return vloss_gpu;
}

const std::vector<float>& ALayerAdaptor::delta_gpu() const
{
  assert(l != nullptr);
  return vdelta_gpu;
}

const std::vector<float>& ALayerAdaptor::cos_sim_gpu() const
{
  assert(l != nullptr);
  return vcos_sim_gpu;
}

const std::vector<float>& ALayerAdaptor::rand_gpu() const
{
  assert(l != nullptr);
  return vrand_gpu;
}

const std::vector<float>& ALayerAdaptor::drop_blocks_scale() const
{
  assert(l != nullptr);
  return vdrop_blocks_scale;
}

const std::vector<float>& ALayerAdaptor::drop_blocks_scale_gpu() const
{
  assert(l != nullptr);
  return vdrop_blocks_scale_gpu;
}

const std::vector<float>& ALayerAdaptor::squared_gpu() const
{
  assert(l != nullptr);
  return vsquared_gpu;
}

const std::vector<float>& ALayerAdaptor::norms_gpu() const
{
  assert(l != nullptr);
  return vnorms_gpu;
}

const std::vector<float>& ALayerAdaptor::gt_gpu() const
{
  assert(l != nullptr);
  return vgt_gpu;
}

const std::vector<float>& ALayerAdaptor::a_avg_gpu() const
{
  assert(l != nullptr);
  return va_avg_gpu;
}

const std::vector<int>& ALayerAdaptor::input_sizes_gpu() const
{
  assert(l != nullptr);
  return vinput_sizes_gpu;
}

const ALayerAdaptor::vrfloats& ALayerAdaptor::layers_output_gpu() const
{
  assert(l != nullptr);
  return vrlayers_output_gpu;
}

const ALayerAdaptor::vrfloats& ALayerAdaptor::layers_delta_gpu() const
{
  assert(l != nullptr);
  return vrlayers_delta_gpu;
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

void* ALayerAdaptor::srcTensorDesc() const
{
  assert(l != nullptr);
  return l->srcTensorDesc;
}

void* ALayerAdaptor::dstTensorDesc() const
{
  assert(l != nullptr);
  return l->dstTensorDesc;
}

void* ALayerAdaptor::srcTensorDesc16() const
{
  assert(l != nullptr);
  return l->srcTensorDesc16;
}

void* ALayerAdaptor::dstTensorDesc16() const
{
  assert(l != nullptr);
  return l->dstTensorDesc16;
}

void* ALayerAdaptor::dsrcTensorDesc() const
{
  assert(l != nullptr);
  return l->dsrcTensorDesc;
}

void* ALayerAdaptor::ddstTensorDesc() const
{
  assert(l != nullptr);
  return l->ddstTensorDesc;
}

void* ALayerAdaptor::dsrcTensorDesc16() const
{
  assert(l != nullptr);
  return l->dsrcTensorDesc16;
}

void* ALayerAdaptor::ddstTensorDesc16() const
{
  assert(l != nullptr);
  return l->ddstTensorDesc16;
}

void* ALayerAdaptor::normTensorDesc() const
{
  assert(l != nullptr);
  return l->normTensorDesc;
}

void* ALayerAdaptor::normDstTensorDesc() const
{
  assert(l != nullptr);
  return l->normDstTensorDesc;
}

void* ALayerAdaptor::normDstTensorDescF16() const
{
  assert(l != nullptr);
  return l->normDstTensorDescF16;
}

void* ALayerAdaptor::weightDesc() const
{
  assert(l != nullptr);
  return l->weightDesc;
}

void* ALayerAdaptor::weightDesc16() const
{
  assert(l != nullptr);
  return l->weightDesc16;
}

void* ALayerAdaptor::dweightDesc() const
{
  assert(l != nullptr);
  return l->dweightDesc;
}

void* ALayerAdaptor::dweightDesc16() const
{
  assert(l != nullptr);
  return l->dweightDesc16;
}

void* ALayerAdaptor::convDesc() const
{
  assert(l != nullptr);
  return l->convDesc;
}

UNUSED_ENUM_TYPE ALayerAdaptor::fw_algo() const
{
  assert(l != nullptr);
  return l->fw_algo;
}

UNUSED_ENUM_TYPE ALayerAdaptor::fw_algo16() const
{
  assert(l != nullptr);
  return l->fw_algo16;
}

UNUSED_ENUM_TYPE ALayerAdaptor::bd_algo() const
{
  assert(l != nullptr);
  return l->bd_algo;
}

UNUSED_ENUM_TYPE ALayerAdaptor::bd_algo16() const
{
  assert(l != nullptr);
  return l->bd_algo16;
}

UNUSED_ENUM_TYPE ALayerAdaptor::bf_algo() const
{
  assert(l != nullptr);
  return l->bf_algo;
}

UNUSED_ENUM_TYPE ALayerAdaptor::bf_algo16() const
{
  assert(l != nullptr);
  return l->bf_algo16;
}

void* ALayerAdaptor::poolingDesc() const
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

void ALayerAdaptor::forward(forward_fn nval)
{
  assert(l != nullptr);
  l->forward = nval;
}

void ALayerAdaptor::backward(backward_fn nval)
{
  assert(l != nullptr);
  l->backward = nval;
}

void ALayerAdaptor::update(update_fn nval)
{
  assert(l != nullptr);
  l->update = nval;
}

void ALayerAdaptor::borrow_output(float* buffer)
{
  assert(l != nullptr);
  voutput = std::vector<float>{};
  l->output = buffer;
}

void ALayerAdaptor::borrow_delta(float* buffer)
{
  assert(l != nullptr);
  vdelta = std::vector<float>{};
  l->delta = buffer;
}

void ALayerAdaptor::borrow_weights(float* buffer)
{
  assert(l != nullptr);
  vweights = std::vector<float>{};
  l->weights = buffer;
}

void ALayerAdaptor::borrow_weight_updates(float* buffer)
{
  assert(l != nullptr);
  vweight_updates = std::vector<float>{};
  l->weight_updates = buffer;
}

void ALayerAdaptor::borrow_biases(float* buffer)
{
  assert(l != nullptr);
  vbiases = std::vector<float>{};
  l->biases = buffer;
}

void ALayerAdaptor::borrow_bias_updates(float* buffer)
{
  assert(l != nullptr);
  vbias_updates = std::vector<float>{};
  l->bias_updates = buffer;
}

void ALayerAdaptor::borrow_scales(float* buffer)
{
  assert(l != nullptr);
  vscales = std::vector<float>{};
  l->scales = buffer;
}

void ALayerAdaptor::borrow_scale_updates(float* buffer)
{
  assert(l != nullptr);
  vscale_updates = std::vector<float>{};
  l->scale_updates = buffer;
}

void ALayerAdaptor::borrow_mean(float* buffer)
{
  assert(l != nullptr);
  vmean = std::vector<float>{};
  l->mean = buffer;
}

void ALayerAdaptor::borrow_variance(float* buffer)
{
  assert(l != nullptr);
  vvariance = std::vector<float>{};
  l->variance = buffer;
}

void ALayerAdaptor::borrow_mean_delta(float* buffer)
{
  assert(l != nullptr);
  vmean_delta = std::vector<float>{};
  l->mean_delta = buffer;
}

void ALayerAdaptor::borrow_variance_delta(float* buffer)
{
  assert(l != nullptr);
  vvariance_delta = std::vector<float>{};
  l->variance_delta = buffer;
}

void ALayerAdaptor::borrow_rolling_mean(float* buffer)
{
  assert(l != nullptr);
  vrolling_mean = std::vector<float>{};
  l->rolling_mean = buffer;
}

void ALayerAdaptor::borrow_rolling_variance(float* buffer)
{
  assert(l != nullptr);
  vrolling_variance = std::vector<float>{};
  l->rolling_variance = buffer;
}

void ALayerAdaptor::borrow_labels(int* buffer)
{
  assert(l != nullptr);
  vlabels = std::vector<int>{};
  l->labels = buffer;
}

void ALayerAdaptor::borrow_class_ids(int* buffer)
{
  assert(l != nullptr);
  vclass_ids = std::vector<int>{};
  l->class_ids = buffer;
}

void ALayerAdaptor::own_softmax_tree(std::shared_ptr<tree> nval)
{
  assert(l != nullptr);
  msoftmax_tree = std::move(nval);
  l->softmax_tree = msoftmax_tree.get();
}

void ALayerAdaptor::share_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);

  mshare_layer = nval;

  l->share_layer = layer_of(mshare_layer);
}

void ALayerAdaptor::share_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);

  mshare_layer = std::move(nval);

  l->share_layer = layer_of(mshare_layer);
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
  l->classes_multipliers = data_or_null(vclasses_multipliers);
}

void ALayerAdaptor::classes_multipliers(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vclasses_multipliers = std::move(nval);
  l->classes_multipliers = data_or_null(vclasses_multipliers);
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
  l->embedding_output = data_or_null(vembedding_output);
}

void ALayerAdaptor::embedding_output(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vembedding_output = std::move(nval);
  l->embedding_output = data_or_null(vembedding_output);
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
  l->mask = data_or_null(vmask);
}

void ALayerAdaptor::mask(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vmask = std::move(nval);
  l->mask = data_or_null(vmask);
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
  l->cweights = data_or_null(scweights);
}

void ALayerAdaptor::cweights(std::string&& nval)
{
  assert(l != nullptr);
  scweights = std::move(nval);
  l->cweights = data_or_null(scweights);
}

void ALayerAdaptor::indexes(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vindexes = nval;
  l->indexes = data_or_null(vindexes);
}

void ALayerAdaptor::indexes(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vindexes = std::move(nval);
  l->indexes = data_or_null(vindexes);
}

void ALayerAdaptor::input_layers(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vinput_layers = nval;
  l->input_layers = data_or_null(vinput_layers);
}

void ALayerAdaptor::input_layers(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vinput_layers = std::move(nval);
  l->input_layers = data_or_null(vinput_layers);
}

void ALayerAdaptor::input_sizes(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vinput_sizes = nval;
  l->input_sizes = data_or_null(vinput_sizes);
}

void ALayerAdaptor::input_sizes(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vinput_sizes = std::move(nval);
  l->input_sizes = data_or_null(vinput_sizes);
}

void ALayerAdaptor::layers_output(const vrfloats& nval)
{
  assert(l != nullptr);
  vrlayers_output = nval;
  l->layers_output = data_or_null(vrlayers_output);
}

void ALayerAdaptor::layers_output(vrfloats&& nval)
{
  assert(l != nullptr);
  vrlayers_output = std::move(nval);
  l->layers_output = data_or_null(vrlayers_output);
}

void ALayerAdaptor::layers_delta(const vrfloats& nval)
{
  assert(l != nullptr);
  vrlayers_delta = nval;
  l->layers_delta = data_or_null(vrlayers_delta);
}

void ALayerAdaptor::layers_delta(vrfloats&& nval)
{
  assert(l != nullptr);
  vrlayers_delta = std::move(nval);
  l->layers_delta = data_or_null(vrlayers_delta);
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
  l->map = data_or_null(vmap);
}

void ALayerAdaptor::map(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vmap = std::move(nval);
  l->map = data_or_null(vmap);
}

void ALayerAdaptor::counts(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vcounts = nval;
  l->counts = data_or_null(vcounts);
}

void ALayerAdaptor::counts(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vcounts = std::move(nval);
  l->counts = data_or_null(vcounts);
}

void ALayerAdaptor::sums(const vvfloats& nval)
{
  assert(l != nullptr);
  vsums = nval;

  place_pointers(vrsums, vsums);

  l->sums = data_or_null(vrsums);
}

void ALayerAdaptor::rand(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vrand = nval;
  l->rand = data_or_null(vrand);
}

void ALayerAdaptor::rand(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vrand = std::move(nval);
  l->rand = data_or_null(vrand);
}

void ALayerAdaptor::cost(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcost = nval;
  l->cost = data_or_null(vcost);
}

void ALayerAdaptor::cost(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcost = std::move(nval);
  l->cost = data_or_null(vcost);
}

void ALayerAdaptor::labels(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vlabels = nval;
  l->labels = data_or_null(vlabels);
}

void ALayerAdaptor::labels(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vlabels = std::move(nval);
  l->labels = data_or_null(vlabels);
}

void ALayerAdaptor::class_ids(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vclass_ids = nval;
  l->class_ids = data_or_null(vclass_ids);
}

void ALayerAdaptor::class_ids(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vclass_ids = std::move(nval);
  l->class_ids = data_or_null(vclass_ids);
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
  l->cos_sim = data_or_null(vcos_sim);
}

void ALayerAdaptor::cos_sim(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcos_sim = std::move(nval);
  l->cos_sim = data_or_null(vcos_sim);
}

void ALayerAdaptor::exp_cos_sim(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vexp_cos_sim = nval;
  l->exp_cos_sim = data_or_null(vexp_cos_sim);
}

void ALayerAdaptor::exp_cos_sim(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vexp_cos_sim = std::move(nval);
  l->exp_cos_sim = data_or_null(vexp_cos_sim);
}

void ALayerAdaptor::p_constrastive(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vp_constrastive = nval;
  l->p_constrastive = data_or_null(vp_constrastive);
}

void ALayerAdaptor::p_constrastive(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vp_constrastive = std::move(nval);
  l->p_constrastive = data_or_null(vp_constrastive);
}

void ALayerAdaptor::contrast_p_gpu(const vcontrastive_params& nval)
{
  assert(l != nullptr);
  vcontrast_p_gpu = nval;
  l->contrast_p_gpu = data_or_null(vcontrast_p_gpu);
}

void ALayerAdaptor::contrast_p_gpu(vcontrastive_params&& nval)
{
  assert(l != nullptr);
  vcontrast_p_gpu = std::move(nval);
  l->contrast_p_gpu = data_or_null(vcontrast_p_gpu);
}

void ALayerAdaptor::state(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstate = nval;
  l->state = data_or_null(vstate);
}

void ALayerAdaptor::state(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstate = std::move(nval);
  l->state = data_or_null(vstate);
}

void ALayerAdaptor::prev_state(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vprev_state = nval;
  l->prev_state = data_or_null(vprev_state);
}

void ALayerAdaptor::prev_state(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vprev_state = std::move(nval);
  l->prev_state = data_or_null(vprev_state);
}

void ALayerAdaptor::forgot_state(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vforgot_state = nval;
  l->forgot_state = data_or_null(vforgot_state);
}

void ALayerAdaptor::forgot_state(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vforgot_state = std::move(nval);
  l->forgot_state = data_or_null(vforgot_state);
}

void ALayerAdaptor::forgot_delta(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vforgot_delta = nval;
  l->forgot_delta = data_or_null(vforgot_delta);
}

void ALayerAdaptor::forgot_delta(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vforgot_delta = std::move(nval);
  l->forgot_delta = data_or_null(vforgot_delta);
}

void ALayerAdaptor::state_delta(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstate_delta = nval;
  l->state_delta = data_or_null(vstate_delta);
}

void ALayerAdaptor::state_delta(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstate_delta = std::move(nval);
  l->state_delta = data_or_null(vstate_delta);
}

void ALayerAdaptor::combine_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcombine_cpu = nval;
  l->combine_cpu = data_or_null(vcombine_cpu);
}

void ALayerAdaptor::combine_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcombine_cpu = std::move(nval);
  l->combine_cpu = data_or_null(vcombine_cpu);
}

void ALayerAdaptor::combine_delta_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcombine_delta_cpu = nval;
  l->combine_delta_cpu = data_or_null(vcombine_delta_cpu);
}

void ALayerAdaptor::combine_delta_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcombine_delta_cpu = std::move(nval);
  l->combine_delta_cpu = data_or_null(vcombine_delta_cpu);
}

void ALayerAdaptor::concat(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vconcat = nval;
  l->concat = data_or_null(vconcat);
}

void ALayerAdaptor::concat(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vconcat = std::move(nval);
  l->concat = data_or_null(vconcat);
}

void ALayerAdaptor::concat_delta(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vconcat_delta = nval;
  l->concat_delta = data_or_null(vconcat_delta);
}

void ALayerAdaptor::concat_delta(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vconcat_delta = std::move(nval);
  l->concat_delta = data_or_null(vconcat_delta);
}

void ALayerAdaptor::binary_weights(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbinary_weights = nval;
  l->binary_weights = data_or_null(vbinary_weights);
}

void ALayerAdaptor::binary_weights(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbinary_weights = std::move(nval);
  l->binary_weights = data_or_null(vbinary_weights);
}

void ALayerAdaptor::biases(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbiases = nval;
  l->biases = data_or_null(vbiases);
}

void ALayerAdaptor::biases(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbiases = std::move(nval);
  l->biases = data_or_null(vbiases);
}

void ALayerAdaptor::bias_updates(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_updates = nval;
  l->bias_updates = data_or_null(vbias_updates);
}

void ALayerAdaptor::bias_updates(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_updates = std::move(nval);
  l->bias_updates = data_or_null(vbias_updates);
}

void ALayerAdaptor::scales(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscales = nval;
  l->scales = data_or_null(vscales);
}

void ALayerAdaptor::scales(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscales = std::move(nval);
  l->scales = data_or_null(vscales);
}

void ALayerAdaptor::scale_updates(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_updates = nval;
  l->scale_updates = data_or_null(vscale_updates);
}

void ALayerAdaptor::scale_updates(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_updates = std::move(nval);
  l->scale_updates = data_or_null(vscale_updates);
}

void ALayerAdaptor::weights_ema(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweights_ema = nval;
  l->weights_ema = data_or_null(vweights_ema);
}

void ALayerAdaptor::weights_ema(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweights_ema = std::move(nval);
  l->weights_ema = data_or_null(vweights_ema);
}

void ALayerAdaptor::biases_ema(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbiases_ema = nval;
  l->biases_ema = data_or_null(vbiases_ema);
}

void ALayerAdaptor::biases_ema(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbiases_ema = std::move(nval);
  l->biases_ema = data_or_null(vbiases_ema);
}

void ALayerAdaptor::scales_ema(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscales_ema = nval;
  l->scales_ema = data_or_null(vscales_ema);
}

void ALayerAdaptor::scales_ema(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscales_ema = std::move(nval);
  l->scales_ema = data_or_null(vscales_ema);
}

void ALayerAdaptor::weights(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweights = nval;
  l->weights = data_or_null(vweights);
}

void ALayerAdaptor::weights(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweights = std::move(nval);
  l->weights = data_or_null(vweights);
}

void ALayerAdaptor::weight_updates(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweight_updates = nval;
  l->weight_updates = data_or_null(vweight_updates);
}

void ALayerAdaptor::weight_updates(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweight_updates = std::move(nval);
  l->weight_updates = data_or_null(vweight_updates);
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
  l->align_bit_weights_gpu = data_or_null(salign_bit_weights_gpu);
}

void ALayerAdaptor::align_bit_weights_gpu(std::string&& nval)
{
  assert(l != nullptr);
  salign_bit_weights_gpu = std::move(nval);
  l->align_bit_weights_gpu = data_or_null(salign_bit_weights_gpu);
}

void ALayerAdaptor::mean_arr_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vmean_arr_gpu = nval;
  l->mean_arr_gpu = data_or_null(vmean_arr_gpu);
}

void ALayerAdaptor::mean_arr_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vmean_arr_gpu = std::move(nval);
  l->mean_arr_gpu = data_or_null(vmean_arr_gpu);
}

void ALayerAdaptor::align_workspace_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  valign_workspace_gpu = nval;
  l->align_workspace_gpu = data_or_null(valign_workspace_gpu);
}

void ALayerAdaptor::align_workspace_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  valign_workspace_gpu = std::move(nval);
  l->align_workspace_gpu = data_or_null(valign_workspace_gpu);
}

void ALayerAdaptor::transposed_align_workspace_gpu(
    const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtransposed_align_workspace_gpu = nval;
  l->transposed_align_workspace_gpu =
      data_or_null(vtransposed_align_workspace_gpu);
}

void ALayerAdaptor::transposed_align_workspace_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtransposed_align_workspace_gpu = std::move(nval);
  l->transposed_align_workspace_gpu =
      data_or_null(vtransposed_align_workspace_gpu);
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
  l->align_bit_weights = data_or_null(salign_bit_weights);
}

void ALayerAdaptor::align_bit_weights(std::string&& nval)
{
  assert(l != nullptr);
  salign_bit_weights = std::move(nval);
  l->align_bit_weights = data_or_null(salign_bit_weights);
}

void ALayerAdaptor::mean_arr(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vmean_arr = nval;
  l->mean_arr = data_or_null(vmean_arr);
}

void ALayerAdaptor::mean_arr(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vmean_arr = std::move(nval);
  l->mean_arr = data_or_null(vmean_arr);
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
  l->col_image = data_or_null(vcol_image);
}

void ALayerAdaptor::col_image(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcol_image = std::move(nval);
  l->col_image = data_or_null(vcol_image);
}

void ALayerAdaptor::delta(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdelta = nval;
  l->delta = data_or_null(vdelta);
}

void ALayerAdaptor::delta(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdelta = std::move(nval);
  l->delta = data_or_null(vdelta);
}

void ALayerAdaptor::output(const std::vector<float>& nval)
{
  assert(l != nullptr);
  voutput = nval;
  l->output = data_or_null(voutput);
}

void ALayerAdaptor::output(std::vector<float>&& nval)
{
  assert(l != nullptr);
  voutput = std::move(nval);
  l->output = data_or_null(voutput);
}

void ALayerAdaptor::activation_input(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vactivation_input = nval;
  l->activation_input = data_or_null(vactivation_input);
}

void ALayerAdaptor::activation_input(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vactivation_input = std::move(nval);
  l->activation_input = data_or_null(vactivation_input);
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
  l->loss = data_or_null(vloss);
}

void ALayerAdaptor::loss(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vloss = std::move(nval);
  l->loss = data_or_null(vloss);
}

void ALayerAdaptor::squared(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vsquared = nval;
  l->squared = data_or_null(vsquared);
}

void ALayerAdaptor::squared(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vsquared = std::move(nval);
  l->squared = data_or_null(vsquared);
}

void ALayerAdaptor::norms(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vnorms = nval;
  l->norms = data_or_null(vnorms);
}

void ALayerAdaptor::norms(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vnorms = std::move(nval);
  l->norms = data_or_null(vnorms);
}

void ALayerAdaptor::spatial_mean(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vspatial_mean = nval;
  l->spatial_mean = data_or_null(vspatial_mean);
}

void ALayerAdaptor::spatial_mean(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vspatial_mean = std::move(nval);
  l->spatial_mean = data_or_null(vspatial_mean);
}

void ALayerAdaptor::mean(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vmean = nval;
  l->mean = data_or_null(vmean);
}

void ALayerAdaptor::mean(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vmean = std::move(nval);
  l->mean = data_or_null(vmean);
}

void ALayerAdaptor::variance(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vvariance = nval;
  l->variance = data_or_null(vvariance);
}

void ALayerAdaptor::variance(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vvariance = std::move(nval);
  l->variance = data_or_null(vvariance);
}

void ALayerAdaptor::mean_delta(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vmean_delta = nval;
  l->mean_delta = data_or_null(vmean_delta);
}

void ALayerAdaptor::mean_delta(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vmean_delta = std::move(nval);
  l->mean_delta = data_or_null(vmean_delta);
}

void ALayerAdaptor::variance_delta(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vvariance_delta = nval;
  l->variance_delta = data_or_null(vvariance_delta);
}

void ALayerAdaptor::variance_delta(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vvariance_delta = std::move(nval);
  l->variance_delta = data_or_null(vvariance_delta);
}

void ALayerAdaptor::rolling_mean(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vrolling_mean = nval;
  l->rolling_mean = data_or_null(vrolling_mean);
}

void ALayerAdaptor::rolling_mean(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vrolling_mean = std::move(nval);
  l->rolling_mean = data_or_null(vrolling_mean);
}

void ALayerAdaptor::rolling_variance(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vrolling_variance = nval;
  l->rolling_variance = data_or_null(vrolling_variance);
}

void ALayerAdaptor::rolling_variance(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vrolling_variance = std::move(nval);
  l->rolling_variance = data_or_null(vrolling_variance);
}

void ALayerAdaptor::x(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vx = nval;
  l->x = data_or_null(vx);
}

void ALayerAdaptor::x(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vx = std::move(nval);
  l->x = data_or_null(vx);
}

void ALayerAdaptor::x_norm(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vx_norm = nval;
  l->x_norm = data_or_null(vx_norm);
}

void ALayerAdaptor::x_norm(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vx_norm = std::move(nval);
  l->x_norm = data_or_null(vx_norm);
}

void ALayerAdaptor::m(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vm = nval;
  l->m = data_or_null(vm);
}

void ALayerAdaptor::m(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vm = std::move(nval);
  l->m = data_or_null(vm);
}

void ALayerAdaptor::v(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vv = nval;
  l->v = data_or_null(vv);
}

void ALayerAdaptor::v(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vv = std::move(nval);
  l->v = data_or_null(vv);
}

void ALayerAdaptor::bias_m(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_m = nval;
  l->bias_m = data_or_null(vbias_m);
}

void ALayerAdaptor::bias_m(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_m = std::move(nval);
  l->bias_m = data_or_null(vbias_m);
}

void ALayerAdaptor::bias_v(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_v = nval;
  l->bias_v = data_or_null(vbias_v);
}

void ALayerAdaptor::bias_v(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_v = std::move(nval);
  l->bias_v = data_or_null(vbias_v);
}

void ALayerAdaptor::scale_m(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_m = nval;
  l->scale_m = data_or_null(vscale_m);
}

void ALayerAdaptor::scale_m(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_m = std::move(nval);
  l->scale_m = data_or_null(vscale_m);
}

void ALayerAdaptor::scale_v(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_v = nval;
  l->scale_v = data_or_null(vscale_v);
}

void ALayerAdaptor::scale_v(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_v = std::move(nval);
  l->scale_v = data_or_null(vscale_v);
}

void ALayerAdaptor::z_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vz_cpu = nval;
  l->z_cpu = data_or_null(vz_cpu);
}

void ALayerAdaptor::z_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vz_cpu = std::move(nval);
  l->z_cpu = data_or_null(vz_cpu);
}

void ALayerAdaptor::r_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vr_cpu = nval;
  l->r_cpu = data_or_null(vr_cpu);
}

void ALayerAdaptor::r_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vr_cpu = std::move(nval);
  l->r_cpu = data_or_null(vr_cpu);
}

void ALayerAdaptor::h_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vh_cpu = nval;
  l->h_cpu = data_or_null(vh_cpu);
}

void ALayerAdaptor::h_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vh_cpu = std::move(nval);
  l->h_cpu = data_or_null(vh_cpu);
}

void ALayerAdaptor::stored_h_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstored_h_cpu = nval;
  l->stored_h_cpu = data_or_null(vstored_h_cpu);
}

void ALayerAdaptor::stored_h_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstored_h_cpu = std::move(nval);
  l->stored_h_cpu = data_or_null(vstored_h_cpu);
}

void ALayerAdaptor::prev_state_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vprev_state_cpu = nval;
  l->prev_state_cpu = data_or_null(vprev_state_cpu);
}

void ALayerAdaptor::prev_state_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vprev_state_cpu = std::move(nval);
  l->prev_state_cpu = data_or_null(vprev_state_cpu);
}

void ALayerAdaptor::temp_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtemp_cpu = nval;
  l->temp_cpu = data_or_null(vtemp_cpu);
}

void ALayerAdaptor::temp_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtemp_cpu = std::move(nval);
  l->temp_cpu = data_or_null(vtemp_cpu);
}

void ALayerAdaptor::temp2_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtemp2_cpu = nval;
  l->temp2_cpu = data_or_null(vtemp2_cpu);
}

void ALayerAdaptor::temp2_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtemp2_cpu = std::move(nval);
  l->temp2_cpu = data_or_null(vtemp2_cpu);
}

void ALayerAdaptor::temp3_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtemp3_cpu = nval;
  l->temp3_cpu = data_or_null(vtemp3_cpu);
}

void ALayerAdaptor::temp3_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtemp3_cpu = std::move(nval);
  l->temp3_cpu = data_or_null(vtemp3_cpu);
}

void ALayerAdaptor::dh_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdh_cpu = nval;
  l->dh_cpu = data_or_null(vdh_cpu);
}

void ALayerAdaptor::dh_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdh_cpu = std::move(nval);
  l->dh_cpu = data_or_null(vdh_cpu);
}

void ALayerAdaptor::hh_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vhh_cpu = nval;
  l->hh_cpu = data_or_null(vhh_cpu);
}

void ALayerAdaptor::hh_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vhh_cpu = std::move(nval);
  l->hh_cpu = data_or_null(vhh_cpu);
}

void ALayerAdaptor::prev_cell_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vprev_cell_cpu = nval;
  l->prev_cell_cpu = data_or_null(vprev_cell_cpu);
}

void ALayerAdaptor::prev_cell_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vprev_cell_cpu = std::move(nval);
  l->prev_cell_cpu = data_or_null(vprev_cell_cpu);
}

void ALayerAdaptor::cell_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcell_cpu = nval;
  l->cell_cpu = data_or_null(vcell_cpu);
}

void ALayerAdaptor::cell_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcell_cpu = std::move(nval);
  l->cell_cpu = data_or_null(vcell_cpu);
}

void ALayerAdaptor::f_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vf_cpu = nval;
  l->f_cpu = data_or_null(vf_cpu);
}

void ALayerAdaptor::f_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vf_cpu = std::move(nval);
  l->f_cpu = data_or_null(vf_cpu);
}

void ALayerAdaptor::i_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vi_cpu = nval;
  l->i_cpu = data_or_null(vi_cpu);
}

void ALayerAdaptor::i_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vi_cpu = std::move(nval);
  l->i_cpu = data_or_null(vi_cpu);
}

void ALayerAdaptor::g_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vg_cpu = nval;
  l->g_cpu = data_or_null(vg_cpu);
}

void ALayerAdaptor::g_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vg_cpu = std::move(nval);
  l->g_cpu = data_or_null(vg_cpu);
}

void ALayerAdaptor::o_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vo_cpu = nval;
  l->o_cpu = data_or_null(vo_cpu);
}

void ALayerAdaptor::o_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vo_cpu = std::move(nval);
  l->o_cpu = data_or_null(vo_cpu);
}

void ALayerAdaptor::c_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vc_cpu = nval;
  l->c_cpu = data_or_null(vc_cpu);
}

void ALayerAdaptor::c_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vc_cpu = std::move(nval);
  l->c_cpu = data_or_null(vc_cpu);
}

void ALayerAdaptor::stored_c_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstored_c_cpu = nval;
  l->stored_c_cpu = data_or_null(vstored_c_cpu);
}

void ALayerAdaptor::stored_c_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstored_c_cpu = std::move(nval);
  l->stored_c_cpu = data_or_null(vstored_c_cpu);
}

void ALayerAdaptor::dc_cpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdc_cpu = nval;
  l->dc_cpu = data_or_null(vdc_cpu);
}

void ALayerAdaptor::dc_cpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdc_cpu = std::move(nval);
  l->dc_cpu = data_or_null(vdc_cpu);
}

void ALayerAdaptor::binary_input(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbinary_input = nval;
  l->binary_input = data_or_null(vbinary_input);
}

void ALayerAdaptor::binary_input(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbinary_input = std::move(nval);
  l->binary_input = data_or_null(vbinary_input);
}

void ALayerAdaptor::bin_re_packed_input(const std::vector<uint32_t>& nval)
{
  assert(l != nullptr);
  vbin_re_packed_input = nval;
  l->bin_re_packed_input = data_or_null(vbin_re_packed_input);
}

void ALayerAdaptor::bin_re_packed_input(std::vector<uint32_t>&& nval)
{
  assert(l != nullptr);
  vbin_re_packed_input = std::move(nval);
  l->bin_re_packed_input = data_or_null(vbin_re_packed_input);
}

void ALayerAdaptor::t_bit_input(const std::string& nval)
{
  assert(l != nullptr);
  st_bit_input = nval;
  l->t_bit_input = data_or_null(st_bit_input);
}

void ALayerAdaptor::t_bit_input(std::string&& nval)
{
  assert(l != nullptr);
  st_bit_input = std::move(nval);
  l->t_bit_input = data_or_null(st_bit_input);
}

void ALayerAdaptor::input_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  minput_layer = nval;
  l->input_layer = layer_of(minput_layer);
}

void ALayerAdaptor::input_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  minput_layer = std::move(nval);
  l->input_layer = layer_of(minput_layer);
}

void ALayerAdaptor::self_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mself_layer = nval;
  l->self_layer = layer_of(mself_layer);
}

void ALayerAdaptor::self_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mself_layer = std::move(nval);
  l->self_layer = layer_of(mself_layer);
}

void ALayerAdaptor::output_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  moutput_layer = nval;
  l->output_layer = layer_of(moutput_layer);
}

void ALayerAdaptor::output_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  moutput_layer = std::move(nval);
  l->output_layer = layer_of(moutput_layer);
}

void ALayerAdaptor::reset_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mreset_layer = nval;
  l->reset_layer = layer_of(mreset_layer);
}

void ALayerAdaptor::reset_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mreset_layer = std::move(nval);
  l->reset_layer = layer_of(mreset_layer);
}

void ALayerAdaptor::update_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mupdate_layer = nval;
  l->update_layer = layer_of(mupdate_layer);
}

void ALayerAdaptor::update_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mupdate_layer = std::move(nval);
  l->update_layer = layer_of(mupdate_layer);
}

void ALayerAdaptor::state_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mstate_layer = nval;
  l->state_layer = layer_of(mstate_layer);
}

void ALayerAdaptor::state_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mstate_layer = std::move(nval);
  l->state_layer = layer_of(mstate_layer);
}

void ALayerAdaptor::input_gate_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  minput_gate_layer = nval;
  l->input_gate_layer = layer_of(minput_gate_layer);
}

void ALayerAdaptor::input_gate_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  minput_gate_layer = std::move(nval);
  l->input_gate_layer = layer_of(minput_gate_layer);
}

void ALayerAdaptor::state_gate_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mstate_gate_layer = nval;
  l->state_gate_layer = layer_of(mstate_gate_layer);
}

void ALayerAdaptor::state_gate_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mstate_gate_layer = std::move(nval);
  l->state_gate_layer = layer_of(mstate_gate_layer);
}

void ALayerAdaptor::input_save_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  minput_save_layer = nval;
  l->input_save_layer = layer_of(minput_save_layer);
}

void ALayerAdaptor::input_save_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  minput_save_layer = std::move(nval);
  l->input_save_layer = layer_of(minput_save_layer);
}

void ALayerAdaptor::state_save_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mstate_save_layer = nval;
  l->state_save_layer = layer_of(mstate_save_layer);
}

void ALayerAdaptor::state_save_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mstate_save_layer = std::move(nval);
  l->state_save_layer = layer_of(mstate_save_layer);
}

void ALayerAdaptor::input_state_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  minput_state_layer = nval;
  l->input_state_layer = layer_of(minput_state_layer);
}

void ALayerAdaptor::input_state_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  minput_state_layer = std::move(nval);
  l->input_state_layer = layer_of(minput_state_layer);
}

void ALayerAdaptor::state_state_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mstate_state_layer = nval;
  l->state_state_layer = layer_of(mstate_state_layer);
}

void ALayerAdaptor::state_state_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mstate_state_layer = std::move(nval);
  l->state_state_layer = layer_of(mstate_state_layer);
}

void ALayerAdaptor::input_z_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  minput_z_layer = nval;
  l->input_z_layer = layer_of(minput_z_layer);
}

void ALayerAdaptor::input_z_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  minput_z_layer = std::move(nval);
  l->input_z_layer = layer_of(minput_z_layer);
}

void ALayerAdaptor::state_z_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mstate_z_layer = nval;
  l->state_z_layer = layer_of(mstate_z_layer);
}

void ALayerAdaptor::state_z_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mstate_z_layer = std::move(nval);
  l->state_z_layer = layer_of(mstate_z_layer);
}

void ALayerAdaptor::input_r_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  minput_r_layer = nval;
  l->input_r_layer = layer_of(minput_r_layer);
}

void ALayerAdaptor::input_r_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  minput_r_layer = std::move(nval);
  l->input_r_layer = layer_of(minput_r_layer);
}

void ALayerAdaptor::state_r_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mstate_r_layer = nval;
  l->state_r_layer = layer_of(mstate_r_layer);
}

void ALayerAdaptor::state_r_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mstate_r_layer = std::move(nval);
  l->state_r_layer = layer_of(mstate_r_layer);
}

void ALayerAdaptor::input_h_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  minput_h_layer = nval;
  l->input_h_layer = layer_of(minput_h_layer);
}

void ALayerAdaptor::input_h_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  minput_h_layer = std::move(nval);
  l->input_h_layer = layer_of(minput_h_layer);
}

void ALayerAdaptor::state_h_layer(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mstate_h_layer = nval;
  l->state_h_layer = layer_of(mstate_h_layer);
}

void ALayerAdaptor::state_h_layer(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mstate_h_layer = std::move(nval);
  l->state_h_layer = layer_of(mstate_h_layer);
}

void ALayerAdaptor::wz(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mwz = nval;
  l->wz = layer_of(mwz);
}

void ALayerAdaptor::wz(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mwz = std::move(nval);
  l->wz = layer_of(mwz);
}

void ALayerAdaptor::uz(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  muz = nval;
  l->uz = layer_of(muz);
}

void ALayerAdaptor::uz(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  muz = std::move(nval);
  l->uz = layer_of(muz);
}

void ALayerAdaptor::wr(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mwr = nval;
  l->wr = layer_of(mwr);
}

void ALayerAdaptor::wr(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mwr = std::move(nval);
  l->wr = layer_of(mwr);
}

void ALayerAdaptor::ur(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mur = nval;
  l->ur = layer_of(mur);
}

void ALayerAdaptor::ur(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mur = std::move(nval);
  l->ur = layer_of(mur);
}

void ALayerAdaptor::wh(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mwh = nval;
  l->wh = layer_of(mwh);
}

void ALayerAdaptor::wh(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mwh = std::move(nval);
  l->wh = layer_of(mwh);
}

void ALayerAdaptor::uh(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  muh = nval;
  l->uh = layer_of(muh);
}

void ALayerAdaptor::uh(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  muh = std::move(nval);
  l->uh = layer_of(muh);
}

void ALayerAdaptor::uo(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  muo = nval;
  l->uo = layer_of(muo);
}

void ALayerAdaptor::uo(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  muo = std::move(nval);
  l->uo = layer_of(muo);
}

void ALayerAdaptor::wo(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mwo = nval;
  l->wo = layer_of(mwo);
}

void ALayerAdaptor::wo(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mwo = std::move(nval);
  l->wo = layer_of(mwo);
}

void ALayerAdaptor::vo(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mvo = nval;
  l->vo = layer_of(mvo);
}

void ALayerAdaptor::vo(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mvo = std::move(nval);
  l->vo = layer_of(mvo);
}

void ALayerAdaptor::uf(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  muf = nval;
  l->uf = layer_of(muf);
}

void ALayerAdaptor::uf(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  muf = std::move(nval);
  l->uf = layer_of(muf);
}

void ALayerAdaptor::wf(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mwf = nval;
  l->wf = layer_of(mwf);
}

void ALayerAdaptor::wf(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mwf = std::move(nval);
  l->wf = layer_of(mwf);
}

void ALayerAdaptor::vf(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mvf = nval;
  l->vf = layer_of(mvf);
}

void ALayerAdaptor::vf(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mvf = std::move(nval);
  l->vf = layer_of(mvf);
}

void ALayerAdaptor::ui(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mui = nval;
  l->ui = layer_of(mui);
}

void ALayerAdaptor::ui(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mui = std::move(nval);
  l->ui = layer_of(mui);
}

void ALayerAdaptor::wi(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mwi = nval;
  l->wi = layer_of(mwi);
}

void ALayerAdaptor::wi(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mwi = std::move(nval);
  l->wi = layer_of(mwi);
}

void ALayerAdaptor::vi(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mvi = nval;
  l->vi = layer_of(mvi);
}

void ALayerAdaptor::vi(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mvi = std::move(nval);
  l->vi = layer_of(mvi);
}

void ALayerAdaptor::ug(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mug = nval;
  l->ug = layer_of(mug);
}

void ALayerAdaptor::ug(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mug = std::move(nval);
  l->ug = layer_of(mug);
}

void ALayerAdaptor::wg(const ALayerAdaptorPtr& nval)
{
  assert(l != nullptr);
  mwg = nval;
  l->wg = layer_of(mwg);
}

void ALayerAdaptor::wg(ALayerAdaptorPtr&& nval)
{
  assert(l != nullptr);
  mwg = std::move(nval);
  l->wg = layer_of(mwg);
}

void ALayerAdaptor::softmax_tree(tree* nval)
{
  assert(l != nullptr);

  if (msoftmax_tree.get() != nval) {
    msoftmax_tree.reset();
  }

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

// #ifdef GPU
void ALayerAdaptor::indexes_gpu(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vindexes_gpu = nval;
  l->indexes_gpu = data_or_null(vindexes_gpu);
}

void ALayerAdaptor::indexes_gpu(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vindexes_gpu = std::move(nval);
  l->indexes_gpu = data_or_null(vindexes_gpu);
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
  l->z_gpu = data_or_null(vz_gpu);
}

void ALayerAdaptor::z_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vz_gpu = std::move(nval);
  l->z_gpu = data_or_null(vz_gpu);
}

void ALayerAdaptor::r_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vr_gpu = nval;
  l->r_gpu = data_or_null(vr_gpu);
}

void ALayerAdaptor::r_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vr_gpu = std::move(nval);
  l->r_gpu = data_or_null(vr_gpu);
}

void ALayerAdaptor::h_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vh_gpu = nval;
  l->h_gpu = data_or_null(vh_gpu);
}

void ALayerAdaptor::h_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vh_gpu = std::move(nval);
  l->h_gpu = data_or_null(vh_gpu);
}

void ALayerAdaptor::stored_h_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstored_h_gpu = nval;
  l->stored_h_gpu = data_or_null(vstored_h_gpu);
}

void ALayerAdaptor::stored_h_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstored_h_gpu = std::move(nval);
  l->stored_h_gpu = data_or_null(vstored_h_gpu);
}

void ALayerAdaptor::bottelneck_hi_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbottelneck_hi_gpu = nval;
  l->bottelneck_hi_gpu = data_or_null(vbottelneck_hi_gpu);
}

void ALayerAdaptor::bottelneck_hi_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbottelneck_hi_gpu = std::move(nval);
  l->bottelneck_hi_gpu = data_or_null(vbottelneck_hi_gpu);
}

void ALayerAdaptor::bottelneck_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbottelneck_delta_gpu = nval;
  l->bottelneck_delta_gpu = data_or_null(vbottelneck_delta_gpu);
}

void ALayerAdaptor::bottelneck_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbottelneck_delta_gpu = std::move(nval);
  l->bottelneck_delta_gpu = data_or_null(vbottelneck_delta_gpu);
}

void ALayerAdaptor::temp_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtemp_gpu = nval;
  l->temp_gpu = data_or_null(vtemp_gpu);
}

void ALayerAdaptor::temp_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtemp_gpu = std::move(nval);
  l->temp_gpu = data_or_null(vtemp_gpu);
}

void ALayerAdaptor::temp2_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtemp2_gpu = nval;
  l->temp2_gpu = data_or_null(vtemp2_gpu);
}

void ALayerAdaptor::temp2_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtemp2_gpu = std::move(nval);
  l->temp2_gpu = data_or_null(vtemp2_gpu);
}

void ALayerAdaptor::temp3_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vtemp3_gpu = nval;
  l->temp3_gpu = data_or_null(vtemp3_gpu);
}

void ALayerAdaptor::temp3_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vtemp3_gpu = std::move(nval);
  l->temp3_gpu = data_or_null(vtemp3_gpu);
}

void ALayerAdaptor::dh_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdh_gpu = nval;
  l->dh_gpu = data_or_null(vdh_gpu);
}

void ALayerAdaptor::dh_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdh_gpu = std::move(nval);
  l->dh_gpu = data_or_null(vdh_gpu);
}

void ALayerAdaptor::hh_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vhh_gpu = nval;
  l->hh_gpu = data_or_null(vhh_gpu);
}

void ALayerAdaptor::hh_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vhh_gpu = std::move(nval);
  l->hh_gpu = data_or_null(vhh_gpu);
}

void ALayerAdaptor::prev_cell_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vprev_cell_gpu = nval;
  l->prev_cell_gpu = data_or_null(vprev_cell_gpu);
}

void ALayerAdaptor::prev_cell_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vprev_cell_gpu = std::move(nval);
  l->prev_cell_gpu = data_or_null(vprev_cell_gpu);
}

void ALayerAdaptor::prev_state_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vprev_state_gpu = nval;
  l->prev_state_gpu = data_or_null(vprev_state_gpu);
}

void ALayerAdaptor::prev_state_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vprev_state_gpu = std::move(nval);
  l->prev_state_gpu = data_or_null(vprev_state_gpu);
}

void ALayerAdaptor::last_prev_state_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vlast_prev_state_gpu = nval;
  l->last_prev_state_gpu = data_or_null(vlast_prev_state_gpu);
}

void ALayerAdaptor::last_prev_state_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vlast_prev_state_gpu = std::move(nval);
  l->last_prev_state_gpu = data_or_null(vlast_prev_state_gpu);
}

void ALayerAdaptor::last_prev_cell_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vlast_prev_cell_gpu = nval;
  l->last_prev_cell_gpu = data_or_null(vlast_prev_cell_gpu);
}

void ALayerAdaptor::last_prev_cell_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vlast_prev_cell_gpu = std::move(nval);
  l->last_prev_cell_gpu = data_or_null(vlast_prev_cell_gpu);
}

void ALayerAdaptor::cell_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcell_gpu = nval;
  l->cell_gpu = data_or_null(vcell_gpu);
}

void ALayerAdaptor::cell_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcell_gpu = std::move(nval);
  l->cell_gpu = data_or_null(vcell_gpu);
}

void ALayerAdaptor::f_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vf_gpu = nval;
  l->f_gpu = data_or_null(vf_gpu);
}

void ALayerAdaptor::f_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vf_gpu = std::move(nval);
  l->f_gpu = data_or_null(vf_gpu);
}

void ALayerAdaptor::i_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vi_gpu = nval;
  l->i_gpu = data_or_null(vi_gpu);
}

void ALayerAdaptor::i_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vi_gpu = std::move(nval);
  l->i_gpu = data_or_null(vi_gpu);
}

void ALayerAdaptor::g_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vg_gpu = nval;
  l->g_gpu = data_or_null(vg_gpu);
}

void ALayerAdaptor::g_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vg_gpu = std::move(nval);
  l->g_gpu = data_or_null(vg_gpu);
}

void ALayerAdaptor::o_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vo_gpu = nval;
  l->o_gpu = data_or_null(vo_gpu);
}

void ALayerAdaptor::o_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vo_gpu = std::move(nval);
  l->o_gpu = data_or_null(vo_gpu);
}

void ALayerAdaptor::c_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vc_gpu = nval;
  l->c_gpu = data_or_null(vc_gpu);
}

void ALayerAdaptor::c_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vc_gpu = std::move(nval);
  l->c_gpu = data_or_null(vc_gpu);
}

void ALayerAdaptor::stored_c_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstored_c_gpu = nval;
  l->stored_c_gpu = data_or_null(vstored_c_gpu);
}

void ALayerAdaptor::stored_c_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstored_c_gpu = std::move(nval);
  l->stored_c_gpu = data_or_null(vstored_c_gpu);
}

void ALayerAdaptor::dc_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdc_gpu = nval;
  l->dc_gpu = data_or_null(vdc_gpu);
}

void ALayerAdaptor::dc_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdc_gpu = std::move(nval);
  l->dc_gpu = data_or_null(vdc_gpu);
}

// adam
void ALayerAdaptor::m_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vm_gpu = nval;
  l->m_gpu = data_or_null(vm_gpu);
}

void ALayerAdaptor::m_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vm_gpu = std::move(nval);
  l->m_gpu = data_or_null(vm_gpu);
}

void ALayerAdaptor::v_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vv_gpu = nval;
  l->v_gpu = data_or_null(vv_gpu);
}

void ALayerAdaptor::v_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vv_gpu = std::move(nval);
  l->v_gpu = data_or_null(vv_gpu);
}

void ALayerAdaptor::bias_m_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_m_gpu = nval;
  l->bias_m_gpu = data_or_null(vbias_m_gpu);
}

void ALayerAdaptor::bias_m_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_m_gpu = std::move(nval);
  l->bias_m_gpu = data_or_null(vbias_m_gpu);
}

void ALayerAdaptor::scale_m_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_m_gpu = nval;
  l->scale_m_gpu = data_or_null(vscale_m_gpu);
}

void ALayerAdaptor::scale_m_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_m_gpu = std::move(nval);
  l->scale_m_gpu = data_or_null(vscale_m_gpu);
}

void ALayerAdaptor::bias_v_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_v_gpu = nval;
  l->bias_v_gpu = data_or_null(vbias_v_gpu);
}

void ALayerAdaptor::bias_v_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_v_gpu = std::move(nval);
  l->bias_v_gpu = data_or_null(vbias_v_gpu);
}

void ALayerAdaptor::scale_v_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_v_gpu = nval;
  l->scale_v_gpu = data_or_null(vscale_v_gpu);
}

void ALayerAdaptor::scale_v_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_v_gpu = std::move(nval);
  l->scale_v_gpu = data_or_null(vscale_v_gpu);
}

void ALayerAdaptor::combine_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcombine_gpu = nval;
  l->combine_gpu = data_or_null(vcombine_gpu);
}

void ALayerAdaptor::combine_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcombine_gpu = std::move(nval);
  l->combine_gpu = data_or_null(vcombine_gpu);
}

void ALayerAdaptor::combine_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcombine_delta_gpu = nval;
  l->combine_delta_gpu = data_or_null(vcombine_delta_gpu);
}

void ALayerAdaptor::combine_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcombine_delta_gpu = std::move(nval);
  l->combine_delta_gpu = data_or_null(vcombine_delta_gpu);
}

void ALayerAdaptor::forgot_state_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vforgot_state_gpu = nval;
  l->forgot_state_gpu = data_or_null(vforgot_state_gpu);
}

void ALayerAdaptor::forgot_state_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vforgot_state_gpu = std::move(nval);
  l->forgot_state_gpu = data_or_null(vforgot_state_gpu);
}

void ALayerAdaptor::forgot_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vforgot_delta_gpu = nval;
  l->forgot_delta_gpu = data_or_null(vforgot_delta_gpu);
}

void ALayerAdaptor::forgot_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vforgot_delta_gpu = std::move(nval);
  l->forgot_delta_gpu = data_or_null(vforgot_delta_gpu);
}

void ALayerAdaptor::state_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstate_gpu = nval;
  l->state_gpu = data_or_null(vstate_gpu);
}

void ALayerAdaptor::state_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstate_gpu = std::move(nval);
  l->state_gpu = data_or_null(vstate_gpu);
}

void ALayerAdaptor::state_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vstate_delta_gpu = nval;
  l->state_delta_gpu = data_or_null(vstate_delta_gpu);
}

void ALayerAdaptor::state_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vstate_delta_gpu = std::move(nval);
  l->state_delta_gpu = data_or_null(vstate_delta_gpu);
}

void ALayerAdaptor::gate_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vgate_gpu = nval;
  l->gate_gpu = data_or_null(vgate_gpu);
}

void ALayerAdaptor::gate_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vgate_gpu = std::move(nval);
  l->gate_gpu = data_or_null(vgate_gpu);
}

void ALayerAdaptor::gate_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vgate_delta_gpu = nval;
  l->gate_delta_gpu = data_or_null(vgate_delta_gpu);
}

void ALayerAdaptor::gate_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vgate_delta_gpu = std::move(nval);
  l->gate_delta_gpu = data_or_null(vgate_delta_gpu);
}

void ALayerAdaptor::save_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vsave_gpu = nval;
  l->save_gpu = data_or_null(vsave_gpu);
}

void ALayerAdaptor::save_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vsave_gpu = std::move(nval);
  l->save_gpu = data_or_null(vsave_gpu);
}

void ALayerAdaptor::save_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vsave_delta_gpu = nval;
  l->save_delta_gpu = data_or_null(vsave_delta_gpu);
}

void ALayerAdaptor::save_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vsave_delta_gpu = std::move(nval);
  l->save_delta_gpu = data_or_null(vsave_delta_gpu);
}

void ALayerAdaptor::concat_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vconcat_gpu = nval;
  l->concat_gpu = data_or_null(vconcat_gpu);
}

void ALayerAdaptor::concat_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vconcat_gpu = std::move(nval);
  l->concat_gpu = data_or_null(vconcat_gpu);
}

void ALayerAdaptor::concat_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vconcat_delta_gpu = nval;
  l->concat_delta_gpu = data_or_null(vconcat_delta_gpu);
}

void ALayerAdaptor::concat_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vconcat_delta_gpu = std::move(nval);
  l->concat_delta_gpu = data_or_null(vconcat_delta_gpu);
}

void ALayerAdaptor::binary_input_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbinary_input_gpu = nval;
  l->binary_input_gpu = data_or_null(vbinary_input_gpu);
}

void ALayerAdaptor::binary_input_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbinary_input_gpu = std::move(nval);
  l->binary_input_gpu = data_or_null(vbinary_input_gpu);
}

void ALayerAdaptor::binary_weights_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbinary_weights_gpu = nval;
  l->binary_weights_gpu = data_or_null(vbinary_weights_gpu);
}

void ALayerAdaptor::binary_weights_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbinary_weights_gpu = std::move(nval);
  l->binary_weights_gpu = data_or_null(vbinary_weights_gpu);
}

void ALayerAdaptor::bin_conv_shortcut_in_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbin_conv_shortcut_in_gpu = nval;
  l->bin_conv_shortcut_in_gpu = data_or_null(vbin_conv_shortcut_in_gpu);
}

void ALayerAdaptor::bin_conv_shortcut_in_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbin_conv_shortcut_in_gpu = std::move(nval);
  l->bin_conv_shortcut_in_gpu = data_or_null(vbin_conv_shortcut_in_gpu);
}

void ALayerAdaptor::bin_conv_shortcut_out_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbin_conv_shortcut_out_gpu = nval;
  l->bin_conv_shortcut_out_gpu = data_or_null(vbin_conv_shortcut_out_gpu);
}

void ALayerAdaptor::bin_conv_shortcut_out_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbin_conv_shortcut_out_gpu = std::move(nval);
  l->bin_conv_shortcut_out_gpu = data_or_null(vbin_conv_shortcut_out_gpu);
}

void ALayerAdaptor::mean_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vmean_gpu = nval;
  l->mean_gpu = data_or_null(vmean_gpu);
}

void ALayerAdaptor::mean_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vmean_gpu = std::move(nval);
  l->mean_gpu = data_or_null(vmean_gpu);
}

void ALayerAdaptor::variance_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vvariance_gpu = nval;
  l->variance_gpu = data_or_null(vvariance_gpu);
}

void ALayerAdaptor::variance_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vvariance_gpu = std::move(nval);
  l->variance_gpu = data_or_null(vvariance_gpu);
}

void ALayerAdaptor::m_cbn_avg_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vm_cbn_avg_gpu = nval;
  l->m_cbn_avg_gpu = data_or_null(vm_cbn_avg_gpu);
}

void ALayerAdaptor::m_cbn_avg_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vm_cbn_avg_gpu = std::move(nval);
  l->m_cbn_avg_gpu = data_or_null(vm_cbn_avg_gpu);
}

void ALayerAdaptor::v_cbn_avg_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vv_cbn_avg_gpu = nval;
  l->v_cbn_avg_gpu = data_or_null(vv_cbn_avg_gpu);
}

void ALayerAdaptor::v_cbn_avg_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vv_cbn_avg_gpu = std::move(nval);
  l->v_cbn_avg_gpu = data_or_null(vv_cbn_avg_gpu);
}

void ALayerAdaptor::rolling_mean_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vrolling_mean_gpu = nval;
  l->rolling_mean_gpu = data_or_null(vrolling_mean_gpu);
}

void ALayerAdaptor::rolling_mean_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vrolling_mean_gpu = std::move(nval);
  l->rolling_mean_gpu = data_or_null(vrolling_mean_gpu);
}

void ALayerAdaptor::rolling_variance_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vrolling_variance_gpu = nval;
  l->rolling_variance_gpu = data_or_null(vrolling_variance_gpu);
}

void ALayerAdaptor::rolling_variance_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vrolling_variance_gpu = std::move(nval);
  l->rolling_variance_gpu = data_or_null(vrolling_variance_gpu);
}

void ALayerAdaptor::variance_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vvariance_delta_gpu = nval;
  l->variance_delta_gpu = data_or_null(vvariance_delta_gpu);
}

void ALayerAdaptor::variance_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vvariance_delta_gpu = std::move(nval);
  l->variance_delta_gpu = data_or_null(vvariance_delta_gpu);
}

void ALayerAdaptor::mean_delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vmean_delta_gpu = nval;
  l->mean_delta_gpu = data_or_null(vmean_delta_gpu);
}

void ALayerAdaptor::mean_delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vmean_delta_gpu = std::move(nval);
  l->mean_delta_gpu = data_or_null(vmean_delta_gpu);
}

void ALayerAdaptor::col_image_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcol_image_gpu = nval;
  l->col_image_gpu = data_or_null(vcol_image_gpu);
}

void ALayerAdaptor::col_image_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcol_image_gpu = std::move(nval);
  l->col_image_gpu = data_or_null(vcol_image_gpu);
}

void ALayerAdaptor::x_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vx_gpu = nval;
  l->x_gpu = data_or_null(vx_gpu);
}

void ALayerAdaptor::x_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vx_gpu = std::move(nval);
  l->x_gpu = data_or_null(vx_gpu);
}

void ALayerAdaptor::x_norm_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vx_norm_gpu = nval;
  l->x_norm_gpu = data_or_null(vx_norm_gpu);
}

void ALayerAdaptor::x_norm_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vx_norm_gpu = std::move(nval);
  l->x_norm_gpu = data_or_null(vx_norm_gpu);
}

void ALayerAdaptor::weights_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweights_gpu = nval;
  l->weights_gpu = data_or_null(vweights_gpu);
}

void ALayerAdaptor::weights_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweights_gpu = std::move(nval);
  l->weights_gpu = data_or_null(vweights_gpu);
}

void ALayerAdaptor::weight_updates_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweight_updates_gpu = nval;
  l->weight_updates_gpu = data_or_null(vweight_updates_gpu);
}

void ALayerAdaptor::weight_updates_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweight_updates_gpu = std::move(nval);
  l->weight_updates_gpu = data_or_null(vweight_updates_gpu);
}

void ALayerAdaptor::weight_deform_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweight_deform_gpu = nval;
  l->weight_deform_gpu = data_or_null(vweight_deform_gpu);
}

void ALayerAdaptor::weight_deform_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweight_deform_gpu = std::move(nval);
  l->weight_deform_gpu = data_or_null(vweight_deform_gpu);
}

void ALayerAdaptor::weight_change_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweight_change_gpu = nval;
  l->weight_change_gpu = data_or_null(vweight_change_gpu);
}

void ALayerAdaptor::weight_change_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweight_change_gpu = std::move(nval);
  l->weight_change_gpu = data_or_null(vweight_change_gpu);
}

void ALayerAdaptor::weights_gpu16(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweights_gpu16 = nval;
  l->weights_gpu16 = data_or_null(vweights_gpu16);
}

void ALayerAdaptor::weights_gpu16(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweights_gpu16 = std::move(nval);
  l->weights_gpu16 = data_or_null(vweights_gpu16);
}

void ALayerAdaptor::weight_updates_gpu16(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vweight_updates_gpu16 = nval;
  l->weight_updates_gpu16 = data_or_null(vweight_updates_gpu16);
}

void ALayerAdaptor::weight_updates_gpu16(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vweight_updates_gpu16 = std::move(nval);
  l->weight_updates_gpu16 = data_or_null(vweight_updates_gpu16);
}

void ALayerAdaptor::biases_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbiases_gpu = nval;
  l->biases_gpu = data_or_null(vbiases_gpu);
}

void ALayerAdaptor::biases_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbiases_gpu = std::move(nval);
  l->biases_gpu = data_or_null(vbiases_gpu);
}

void ALayerAdaptor::bias_updates_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_updates_gpu = nval;
  l->bias_updates_gpu = data_or_null(vbias_updates_gpu);
}

void ALayerAdaptor::bias_updates_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_updates_gpu = std::move(nval);
  l->bias_updates_gpu = data_or_null(vbias_updates_gpu);
}

void ALayerAdaptor::bias_change_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vbias_change_gpu = nval;
  l->bias_change_gpu = data_or_null(vbias_change_gpu);
}

void ALayerAdaptor::bias_change_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vbias_change_gpu = std::move(nval);
  l->bias_change_gpu = data_or_null(vbias_change_gpu);
}

void ALayerAdaptor::scales_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscales_gpu = nval;
  l->scales_gpu = data_or_null(vscales_gpu);
}

void ALayerAdaptor::scales_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscales_gpu = std::move(nval);
  l->scales_gpu = data_or_null(vscales_gpu);
}

void ALayerAdaptor::scale_updates_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_updates_gpu = nval;
  l->scale_updates_gpu = data_or_null(vscale_updates_gpu);
}

void ALayerAdaptor::scale_updates_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_updates_gpu = std::move(nval);
  l->scale_updates_gpu = data_or_null(vscale_updates_gpu);
}

void ALayerAdaptor::scale_change_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vscale_change_gpu = nval;
  l->scale_change_gpu = data_or_null(vscale_change_gpu);
}

void ALayerAdaptor::scale_change_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vscale_change_gpu = std::move(nval);
  l->scale_change_gpu = data_or_null(vscale_change_gpu);
}

void ALayerAdaptor::input_antialiasing_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vinput_antialiasing_gpu = nval;
  l->input_antialiasing_gpu = data_or_null(vinput_antialiasing_gpu);
}

void ALayerAdaptor::input_antialiasing_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vinput_antialiasing_gpu = std::move(nval);
  l->input_antialiasing_gpu = data_or_null(vinput_antialiasing_gpu);
}

void ALayerAdaptor::output_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  voutput_gpu = nval;
  l->output_gpu = data_or_null(voutput_gpu);
}

void ALayerAdaptor::output_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  voutput_gpu = std::move(nval);
  l->output_gpu = data_or_null(voutput_gpu);
}

void ALayerAdaptor::output_avg_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  voutput_avg_gpu = nval;
  l->output_avg_gpu = data_or_null(voutput_avg_gpu);
}

void ALayerAdaptor::output_avg_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  voutput_avg_gpu = std::move(nval);
  l->output_avg_gpu = data_or_null(voutput_avg_gpu);
}

void ALayerAdaptor::activation_input_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vactivation_input_gpu = nval;
  l->activation_input_gpu = data_or_null(vactivation_input_gpu);
}

void ALayerAdaptor::activation_input_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vactivation_input_gpu = std::move(nval);
  l->activation_input_gpu = data_or_null(vactivation_input_gpu);
}

void ALayerAdaptor::loss_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vloss_gpu = nval;
  l->loss_gpu = data_or_null(vloss_gpu);
}

void ALayerAdaptor::loss_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vloss_gpu = std::move(nval);
  l->loss_gpu = data_or_null(vloss_gpu);
}

void ALayerAdaptor::delta_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdelta_gpu = nval;
  l->delta_gpu = data_or_null(vdelta_gpu);
}

void ALayerAdaptor::delta_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdelta_gpu = std::move(nval);
  l->delta_gpu = data_or_null(vdelta_gpu);
}

void ALayerAdaptor::cos_sim_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vcos_sim_gpu = nval;
  l->cos_sim_gpu = data_or_null(vcos_sim_gpu);
}

void ALayerAdaptor::cos_sim_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vcos_sim_gpu = std::move(nval);
  l->cos_sim_gpu = data_or_null(vcos_sim_gpu);
}

void ALayerAdaptor::rand_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vrand_gpu = nval;
  l->rand_gpu = data_or_null(vrand_gpu);
}

void ALayerAdaptor::rand_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vrand_gpu = std::move(nval);
  l->rand_gpu = data_or_null(vrand_gpu);
}

void ALayerAdaptor::drop_blocks_scale(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdrop_blocks_scale = nval;
  l->drop_blocks_scale = data_or_null(vdrop_blocks_scale);
}

void ALayerAdaptor::drop_blocks_scale(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdrop_blocks_scale = std::move(nval);
  l->drop_blocks_scale = data_or_null(vdrop_blocks_scale);
}

void ALayerAdaptor::drop_blocks_scale_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vdrop_blocks_scale_gpu = nval;
  l->drop_blocks_scale_gpu = data_or_null(vdrop_blocks_scale_gpu);
}

void ALayerAdaptor::drop_blocks_scale_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vdrop_blocks_scale_gpu = std::move(nval);
  l->drop_blocks_scale_gpu = data_or_null(vdrop_blocks_scale_gpu);
}

void ALayerAdaptor::squared_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vsquared_gpu = nval;
  l->squared_gpu = data_or_null(vsquared_gpu);
}

void ALayerAdaptor::squared_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vsquared_gpu = std::move(nval);
  l->squared_gpu = data_or_null(vsquared_gpu);
}

void ALayerAdaptor::norms_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vnorms_gpu = nval;
  l->norms_gpu = data_or_null(vnorms_gpu);
}

void ALayerAdaptor::norms_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vnorms_gpu = std::move(nval);
  l->norms_gpu = data_or_null(vnorms_gpu);
}

void ALayerAdaptor::gt_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  vgt_gpu = nval;
  l->gt_gpu = data_or_null(vgt_gpu);
}

void ALayerAdaptor::gt_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  vgt_gpu = std::move(nval);
  l->gt_gpu = data_or_null(vgt_gpu);
}

void ALayerAdaptor::a_avg_gpu(const std::vector<float>& nval)
{
  assert(l != nullptr);
  va_avg_gpu = nval;
  l->a_avg_gpu = data_or_null(va_avg_gpu);
}

void ALayerAdaptor::a_avg_gpu(std::vector<float>&& nval)
{
  assert(l != nullptr);
  va_avg_gpu = std::move(nval);
  l->a_avg_gpu = data_or_null(va_avg_gpu);
}

void ALayerAdaptor::input_sizes_gpu(const std::vector<int>& nval)
{
  assert(l != nullptr);
  vinput_sizes_gpu = nval;
  l->input_sizes_gpu = data_or_null(vinput_sizes_gpu);
}

void ALayerAdaptor::input_sizes_gpu(std::vector<int>&& nval)
{
  assert(l != nullptr);
  vinput_sizes_gpu = std::move(nval);
  l->input_sizes_gpu = data_or_null(vinput_sizes_gpu);
}

void ALayerAdaptor::layers_output_gpu(const vrfloats& nval)
{
  assert(l != nullptr);
  vrlayers_output_gpu = nval;
  l->layers_output_gpu = data_or_null(vrlayers_output_gpu);
}

void ALayerAdaptor::layers_output_gpu(vrfloats&& nval)
{
  assert(l != nullptr);
  vrlayers_output_gpu = std::move(nval);
  l->layers_output_gpu = data_or_null(vrlayers_output_gpu);
}

void ALayerAdaptor::layers_delta_gpu(const vrfloats& nval)
{
  assert(l != nullptr);
  vrlayers_delta_gpu = nval;
  l->layers_delta_gpu = data_or_null(vrlayers_delta_gpu);
}

void ALayerAdaptor::layers_delta_gpu(vrfloats&& nval)
{
  assert(l != nullptr);
  vrlayers_delta_gpu = std::move(nval);
  l->layers_delta_gpu = data_or_null(vrlayers_delta_gpu);
}

}  // namespace darknet_adaptor::adaptors
