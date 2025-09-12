#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"

#include <cassert>
#include <cstring>
#include <memory>

#include "src/darknet-adaptor/adaptors/IDarknetAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"
#include "src/log/log.h"

namespace darknet_adaptor::adaptors
{

ANetworkAdaptor::ANetworkAdaptor() : net{create_net()}
{
  assert(net != nullptr);
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

networkptr ANetworkAdaptor::get() { return net; }

networkptr ANetworkAdaptor::create_net()
{
  networkptr nnet = std::make_shared<network>();

  std::memset(nnet.get(), 0, sizeof(network));

  return nnet;
}

int ANetworkAdaptor::n()
{
  assert(net != nullptr);
  return net->n;
}

int ANetworkAdaptor::batch()
{
  assert(net != nullptr);
  return net->batch;
}

std::vector<uint64_t>& ANetworkAdaptor::seen()
{
  assert(net != nullptr);
  return vseen;
}

std::vector<float>& ANetworkAdaptor::badlabels_reject_threshold()
{
  assert(net != nullptr);
  return vbadlabels_reject_threshold;
}

std::vector<float>& ANetworkAdaptor::delta_rolling_max()
{
  assert(net != nullptr);
  return vdelta_rolling_max;
}

std::vector<float>& ANetworkAdaptor::delta_rolling_avg()
{
  assert(net != nullptr);
  return vdelta_rolling_avg;
}

std::vector<float>& ANetworkAdaptor::delta_rolling_std()
{
  assert(net != nullptr);
  return vdelta_rolling_std;
}

int ANetworkAdaptor::weights_reject_freq()
{
  assert(net != nullptr);
  return net->weights_reject_freq;
}

int ANetworkAdaptor::equidistant_point()
{
  assert(net != nullptr);
  return net->equidistant_point;
}

float ANetworkAdaptor::badlabels_rejection_percentage()
{
  assert(net != nullptr);
  return net->badlabels_rejection_percentage;
}

float ANetworkAdaptor::num_sigmas_reject_badlabels()
{
  assert(net != nullptr);
  return net->num_sigmas_reject_badlabels;
}

float ANetworkAdaptor::ema_alpha()
{
  assert(net != nullptr);
  return net->ema_alpha;
}

std::vector<int>& ANetworkAdaptor::cur_iteration()
{
  assert(net != nullptr);
  return vcur_iteration;
}

float ANetworkAdaptor::loss_scale()
{
  assert(net != nullptr);
  return net->loss_scale;
}

std::vector<int>& ANetworkAdaptor::t()
{
  assert(net != nullptr);
  return vt;
}

float ANetworkAdaptor::epoch()
{
  assert(net != nullptr);
  return net->epoch;
}

int ANetworkAdaptor::subdivisions()
{
  assert(net != nullptr);
  return net->subdivisions;
}

std::vector<layer>& ANetworkAdaptor::layers()
{
  assert(net != nullptr);
  return vlayers;
}

std::vector<float>& ANetworkAdaptor::output()
{
  assert(net != nullptr);
  return voutput;
}

learning_rate_policy ANetworkAdaptor::policy()
{
  assert(net != nullptr);
  return net->policy;
}

int ANetworkAdaptor::benchmark_layers()
{
  assert(net != nullptr);
  return net->benchmark_layers;
}

std::vector<int>& ANetworkAdaptor::total_bbox()
{
  assert(net != nullptr);
  return vtotal_bbox;
}

std::vector<int>& ANetworkAdaptor::rewritten_bbox()
{
  assert(net != nullptr);
  return vrewritten_bbox;
}

float ANetworkAdaptor::learning_rate()
{
  assert(net != nullptr);
  return net->learning_rate;
}

float ANetworkAdaptor::learning_rate_min()
{
  assert(net != nullptr);
  return net->learning_rate_min;
}

float ANetworkAdaptor::learning_rate_max()
{
  assert(net != nullptr);
  return net->learning_rate_max;
}

int ANetworkAdaptor::batches_per_cycle()
{
  assert(net != nullptr);
  return net->batches_per_cycle;
}

int ANetworkAdaptor::batches_cycle_mult()
{
  assert(net != nullptr);
  return net->batches_cycle_mult;
}

float ANetworkAdaptor::momentum()
{
  assert(net != nullptr);
  return net->momentum;
}

float ANetworkAdaptor::decay()
{
  assert(net != nullptr);
  return net->decay;
}

float ANetworkAdaptor::gamma()
{
  assert(net != nullptr);
  return net->gamma;
}

float ANetworkAdaptor::scale()
{
  assert(net != nullptr);
  return net->scale;
}

float ANetworkAdaptor::power()
{
  assert(net != nullptr);
  return net->power;
}

int ANetworkAdaptor::time_steps()
{
  assert(net != nullptr);
  return net->time_steps;
}

int ANetworkAdaptor::step()
{
  assert(net != nullptr);
  return net->step;
}

int ANetworkAdaptor::max_batches()
{
  assert(net != nullptr);
  return net->max_batches;
}

int ANetworkAdaptor::num_boxes()
{
  assert(net != nullptr);
  return net->num_boxes;
}

int ANetworkAdaptor::train_images_num()
{
  assert(net != nullptr);
  return net->train_images_num;
}

std::vector<float>& ANetworkAdaptor::seq_scales()
{
  assert(net != nullptr);
  return vseq_scales;
}

std::vector<float>& ANetworkAdaptor::scales()
{
  assert(net != nullptr);
  return vscales;
}

std::vector<int>& ANetworkAdaptor::steps()
{
  assert(net != nullptr);
  return vsteps;
}

int ANetworkAdaptor::num_steps()
{
  assert(net != nullptr);
  return net->num_steps;
}

int ANetworkAdaptor::burn_in()
{
  assert(net != nullptr);
  return net->burn_in;
}

int ANetworkAdaptor::cudnn_half()
{
  assert(net != nullptr);
  return net->cudnn_half;
}

int ANetworkAdaptor::adam()
{
  assert(net != nullptr);
  return net->adam;
}

float ANetworkAdaptor::B1()
{
  assert(net != nullptr);
  return net->B1;
}

float ANetworkAdaptor::B2()
{
  assert(net != nullptr);
  return net->B2;
}

float ANetworkAdaptor::eps()
{
  assert(net != nullptr);
  return net->eps;
}

int ANetworkAdaptor::inputs()
{
  assert(net != nullptr);
  return net->inputs;
}

int ANetworkAdaptor::outputs()
{
  assert(net != nullptr);
  return net->outputs;
}

int ANetworkAdaptor::truths()
{
  assert(net != nullptr);
  return net->truths;
}

int ANetworkAdaptor::notruth()
{
  assert(net != nullptr);
  return net->notruth;
}

int ANetworkAdaptor::c()
{
  assert(net != nullptr);
  return net->c;
}

int ANetworkAdaptor::h()
{
  assert(net != nullptr);
  return net->c;
}

int ANetworkAdaptor::w()
{
  assert(net != nullptr);
  return net->c;
}

int ANetworkAdaptor::max_crop()
{
  assert(net != nullptr);
  return net->max_crop;
}

int ANetworkAdaptor::min_crop()
{
  assert(net != nullptr);
  return net->min_crop;
}

float ANetworkAdaptor::max_ratio()
{
  assert(net != nullptr);
  return net->max_ratio;
}

float ANetworkAdaptor::min_ratio()
{
  assert(net != nullptr);
  return net->min_ratio;
}

int ANetworkAdaptor::center()
{
  assert(net != nullptr);
  return net->center;
}

int ANetworkAdaptor::flip()
{
  assert(net != nullptr);
  return net->flip;
}

// horizontal flip 50% probability augmentaiont for classifier
// training (default = 1)
int ANetworkAdaptor::gaussian_noise()
{
  assert(net != nullptr);
  return net->gaussian_noise;
}

int ANetworkAdaptor::blur()
{
  assert(net != nullptr);
  return net->blur;
}

int ANetworkAdaptor::mixup()
{
  assert(net != nullptr);
  return net->mixup;
}

float ANetworkAdaptor::label_smooth_eps()
{
  assert(net != nullptr);
  return net->label_smooth_eps;
}

int ANetworkAdaptor::resize_step()
{
  assert(net != nullptr);
  return net->resize_step;
}

int ANetworkAdaptor::attention()
{
  assert(net != nullptr);
  return net->attention;
}

int ANetworkAdaptor::adversarial()
{
  assert(net != nullptr);
  return net->adversarial;
}

float ANetworkAdaptor::adversarial_lr()
{
  assert(net != nullptr);
  return net->adversarial_lr;
}

float ANetworkAdaptor::max_chart_loss()
{
  assert(net != nullptr);
  return net->max_chart_loss;
}

int ANetworkAdaptor::letter_box()
{
  assert(net != nullptr);
  return net->letter_box;
}

int ANetworkAdaptor::mosaic_bound()
{
  assert(net != nullptr);
  return net->mosaic_bound;
}

int ANetworkAdaptor::contrastive()
{
  assert(net != nullptr);
  return net->contrastive;
}

int ANetworkAdaptor::contrastive_jit_flip()
{
  assert(net != nullptr);
  return net->contrastive_jit_flip;
}

int ANetworkAdaptor::contrastive_color()
{
  assert(net != nullptr);
  return net->contrastive_color;
}

int ANetworkAdaptor::unsupervised()
{
  assert(net != nullptr);
  return net->unsupervised;
}

float ANetworkAdaptor::angle()
{
  assert(net != nullptr);
  return net->angle;
}

float ANetworkAdaptor::aspect()
{
  assert(net != nullptr);
  return net->aspect;
}

float ANetworkAdaptor::exposure()
{
  assert(net != nullptr);
  return net->exposure;
}

float ANetworkAdaptor::saturation()
{
  assert(net != nullptr);
  return net->saturation;
}

float ANetworkAdaptor::hue()
{
  assert(net != nullptr);
  return net->hue;
}

int ANetworkAdaptor::random()
{
  assert(net != nullptr);
  return net->random;
}

int ANetworkAdaptor::track()
{
  assert(net != nullptr);
  return net->track;
}

int ANetworkAdaptor::augment_speed()
{
  assert(net != nullptr);
  return net->augment_speed;
}

int ANetworkAdaptor::sequential_subdivisions()
{
  assert(net != nullptr);
  return net->sequential_subdivisions;
}

int ANetworkAdaptor::init_sequential_subdivisions()
{
  assert(net != nullptr);
  return net->init_sequential_subdivisions;
}

int ANetworkAdaptor::current_subdivision()
{
  assert(net != nullptr);
  return net->current_subdivision;
}

int ANetworkAdaptor::try_fix_nan()
{
  assert(net != nullptr);
  return net->try_fix_nan;
}

int ANetworkAdaptor::gpu_index()
{
  assert(net != nullptr);
  return net->gpu_index;
}

tree* ANetworkAdaptor::hierarchy()
{
  assert(net != nullptr);
  return net->hierarchy;
}

std::vector<float>& ANetworkAdaptor::input()
{
  assert(net != nullptr);
  return vinput;
}

std::vector<float>& ANetworkAdaptor::truth()
{
  assert(net != nullptr);
  return vtruth;
}

std::vector<float>& ANetworkAdaptor::delta()
{
  assert(net != nullptr);
  return vdelta;
}

std::vector<float>& ANetworkAdaptor::workspace()
{
  assert(net != nullptr);
  return vworkspace;
}

int ANetworkAdaptor::train()
{
  assert(net != nullptr);
  return net->train;
}

int ANetworkAdaptor::index()
{
  assert(net != nullptr);
  return net->index;
}

std::vector<float>& ANetworkAdaptor::cost()
{
  assert(net != nullptr);
  return vcost;
}

float ANetworkAdaptor::clip()
{
  assert(net != nullptr);
  return net->clip;
}

//#ifdef GPU
// float *ANetworkAdaptor::input_gpu()
//{
//  assert(net != nullptr);
//  return net->input_gpu;
//}

// float *ANetworkAdaptor::truth_gpu()
//{
//  assert(net != nullptr);
//  return net->truth_gpu;
//}

std::vector<float>& ANetworkAdaptor::delta_gpu()
{
  assert(net != nullptr);
  return vdelta_gpu;
}

std::vector<float>& ANetworkAdaptor::output_gpu()
{
  assert(net != nullptr);
  return voutput_gpu;
}

std::vector<float>& ANetworkAdaptor::input_state_gpu()
{
  assert(net != nullptr);
  return vinput_state_gpu;
}

std::vector<float>& ANetworkAdaptor::input_pinned_cpu()
{
  assert(net != nullptr);
  return vinput_pinned_cpu;
}

int ANetworkAdaptor::input_pinned_cpu_flag()
{
  assert(net != nullptr);
  return net->input_pinned_cpu_flag;
}

ANetworkAdaptor::vvfloats& ANetworkAdaptor::input_gpu()
{
  assert(net != nullptr);
  return vvinput_gpu;
}

ANetworkAdaptor::vvfloats& ANetworkAdaptor::truth_gpu()
{
  assert(net != nullptr);
  return vvtruth_gpu;
}

ANetworkAdaptor::vvfloats& ANetworkAdaptor::input16_gpu()
{
  assert(net != nullptr);
  return vvinput16_gpu;
}

ANetworkAdaptor::vvfloats& ANetworkAdaptor::output16_gpu()
{
  assert(net != nullptr);
  return vvoutput16_gpu;
}

std::vector<size_t>& ANetworkAdaptor::max_input16_size()
{
  assert(net != nullptr);
  return vmax_input16_size;
}

std::vector<size_t>& ANetworkAdaptor::max_output16_size()
{
  assert(net != nullptr);
  return vmax_output16_size;
}

int ANetworkAdaptor::wait_stream()
{
  assert(net != nullptr);
  return net->wait_stream;
}

std::shared_ptr<void> ANetworkAdaptor::cuda_graph()
{
  assert(net != nullptr);
  return mcuda_graph;
}

std::shared_ptr<void> ANetworkAdaptor::cuda_graph_exec()
{
  assert(net != nullptr);
  return mcuda_graph_exec;
}

int ANetworkAdaptor::use_cuda_graph()
{
  assert(net != nullptr);
  return net->use_cuda_graph;
}

std::vector<int>& ANetworkAdaptor::cuda_graph_ready()
{
  assert(net != nullptr);
  return vcuda_graph_ready;
}

std::vector<float>& ANetworkAdaptor::global_delta_gpu()
{
  assert(net != nullptr);
  return vglobal_delta_gpu;
}

std::vector<float>& ANetworkAdaptor::state_delta_gpu()
{
  assert(net != nullptr);
  return vstate_delta_gpu;
}

size_t ANetworkAdaptor::max_delta_gpu_size()
{
  assert(net != nullptr);
  return net->max_delta_gpu_size;
}

//#endif  // GPU
int ANetworkAdaptor::optimized_memory()
{
  assert(net != nullptr);
  return net->optimized_memory;
}

int ANetworkAdaptor::dynamic_minibatch()
{
  assert(net != nullptr);
  return net->dynamic_minibatch;
}

size_t ANetworkAdaptor::workspace_size_limit()
{
  assert(net != nullptr);
  return net->workspace_size_limit;
}

/**
 * @brief Temporary fields for extended network params
 * previously stored in the obj.data file so now
 * only one cfg file is required.
 */
std::string& ANetworkAdaptor::names()
{
  assert(net != nullptr);
  return snames;
}

std::string& ANetworkAdaptor::lastweights()
{
  assert(net != nullptr);
  return slastweights;
}

std::string& ANetworkAdaptor::train_images()
{
  assert(net != nullptr);
  return strain_images;
}

std::string& ANetworkAdaptor::valid_images()
{
  assert(net != nullptr);
  return svalid_images;
}

std::string& ANetworkAdaptor::backup_directory()
{
  assert(net != nullptr);
  return sbackup_directory;
}

int ANetworkAdaptor::saveweights()
{
  assert(net != nullptr);
  return net->saveweights;
}

int ANetworkAdaptor::savelast()
{
  assert(net != nullptr);
  return net->savelast;
}

int ANetworkAdaptor::classes()
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

void ANetworkAdaptor::n(const int& nval)
{
  assert(net != nullptr);
  net->n = nval;
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
  net->seen = vseen.data();
}

void ANetworkAdaptor::badlabels_reject_threshold(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vbadlabels_reject_threshold = nval;
  net->badlabels_reject_threshold = vbadlabels_reject_threshold.data();
}

void ANetworkAdaptor::delta_rolling_max(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vdelta_rolling_max = nval;
  net->delta_rolling_max = vdelta_rolling_max.data();
}

void ANetworkAdaptor::delta_rolling_avg(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vdelta_rolling_avg = nval;
  net->delta_rolling_avg = vdelta_rolling_avg.data();
}

void ANetworkAdaptor::delta_rolling_std(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vdelta_rolling_std = nval;
  net->delta_rolling_std = vdelta_rolling_std.data();
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
  net->cur_iteration = vcur_iteration.data();
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
  net->t = vt.data();
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

void ANetworkAdaptor::layers(const std::vector<layer>& nval)
{
  assert(net != nullptr);
  vlayers = nval;
  net->layers = vlayers.data();
}

void ANetworkAdaptor::output(const std::vector<float>& nval)
{
  assert(net != nullptr);
  voutput = nval;
  net->output = voutput.data();
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
  net->total_bbox = vtotal_bbox.data();
}

void ANetworkAdaptor::rewritten_bbox(const std::vector<int>& nval)
{
  assert(net != nullptr);
  vrewritten_bbox = nval;
  net->rewritten_bbox = vrewritten_bbox.data();
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
  net->seq_scales = vseq_scales.data();
}

void ANetworkAdaptor::scales(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vscales = nval;
  net->scales = vscales.data();
}

void ANetworkAdaptor::steps(const std::vector<int>& nval)
{
  assert(net != nullptr);
  vsteps = nval;
  net->steps = vsteps.data();
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
  net->input = vinput.data();
}

void ANetworkAdaptor::truth(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vtruth = nval;
  net->truth = vtruth.data();
}

void ANetworkAdaptor::delta(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vdelta = nval;
  net->delta = vdelta.data();
}

void ANetworkAdaptor::workspace(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vworkspace = nval;
  net->workspace = vworkspace.data();
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
  net->cost = vcost.data();
}

void ANetworkAdaptor::clip(const float& nval)
{
  assert(net != nullptr);
  net->clip = nval;
}

//#ifdef GPU
// void ANetworkAdaptor::input_gpu(const std::vector<float>& nval)
//{
//  assert(net != nullptr);
//  input_gpu = nval;
//}

// void ANetworkAdaptor::truth_gpu(const std::vector<float>& nval)
//{
//  assert(net != nullptr);
//  truth_gpu = nval;
//}

void ANetworkAdaptor::delta_gpu(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vdelta_gpu = nval;
  net->delta_gpu = vdelta_gpu.data();
}

void ANetworkAdaptor::output_gpu(const std::vector<float>& nval)
{
  assert(net != nullptr);
  voutput_gpu = nval;
  net->output_gpu = voutput_gpu.data();
}

void ANetworkAdaptor::input_state_gpu(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vinput_state_gpu = nval;
  net->input_state_gpu = vinput_state_gpu.data();
}

void ANetworkAdaptor::input_pinned_cpu(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vinput_pinned_cpu = nval;
  net->input_pinned_cpu = vinput_pinned_cpu.data();
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
  net->input_gpu = vrinput_gpu.data();
}

void ANetworkAdaptor::truth_gpu(const vvfloats& nval)
{
  assert(net != nullptr);
  vvtruth_gpu = nval;
  place_pointers(vrtruth_gpu, vvtruth_gpu);
  net->truth_gpu = vrtruth_gpu.data();
}

void ANetworkAdaptor::input16_gpu(const vvfloats& nval)
{
  assert(net != nullptr);
  vvinput16_gpu = nval;
  place_pointers(vrinput16_gpu, vvinput16_gpu);
  net->input16_gpu = vrinput16_gpu.data();
}

void ANetworkAdaptor::output16_gpu(const vvfloats& nval)
{
  assert(net != nullptr);
  vvoutput16_gpu = nval;
  place_pointers(vroutput16_gpu, vvoutput16_gpu);
  net->output16_gpu = vroutput16_gpu.data();
}

void ANetworkAdaptor::max_input16_size(const std::vector<size_t>& nval)
{
  assert(net != nullptr);
  vmax_input16_size = nval;
  net->max_input16_size = vmax_input16_size.data();
}

void ANetworkAdaptor::max_output16_size(const std::vector<size_t>& nval)
{
  assert(net != nullptr);
  vmax_output16_size = nval;
  net->max_output16_size = vmax_output16_size.data();
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
  net->cuda_graph_ready = vcuda_graph_ready.data();
}

void ANetworkAdaptor::global_delta_gpu(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vglobal_delta_gpu = nval;
  net->global_delta_gpu = vglobal_delta_gpu.data();
}

void ANetworkAdaptor::state_delta_gpu(const std::vector<float>& nval)
{
  assert(net != nullptr);
  vstate_delta_gpu = nval;
  net->state_delta_gpu = vstate_delta_gpu.data();
}

void ANetworkAdaptor::max_delta_gpu_size(const size_t& nval)
{
  assert(net != nullptr);
  net->max_delta_gpu_size = nval;
}

//#endif  // GPU
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
  net->names = snames.data();
}

void ANetworkAdaptor::lastweights(const std::string& nval)
{
  assert(net != nullptr);
  slastweights = nval;
  net->lastweights = slastweights.data();
}

void ANetworkAdaptor::train_images(const std::string& nval)
{
  assert(net != nullptr);
  strain_images = nval;
  net->train_images = strain_images.data();
}

void ANetworkAdaptor::valid_images(const std::string& nval)
{
  assert(net != nullptr);
  svalid_images = nval;
  net->valid_images = svalid_images.data();
}

void ANetworkAdaptor::backup_directory(const std::string& nval)
{
  assert(net != nullptr);
  sbackup_directory = nval;
  net->backup_directory = sbackup_directory.data();
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

void ANetworkAdaptor::classes(const int& nval)
{
  assert(net != nullptr);
  net->classes = nval;
}

}  // namespace darknet_adaptor::adaptors
