#ifndef DARKNET_CXX_PORT_PROJECT_ANETWORK_ADAPTOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ANETWORK_ADAPTOR_CLASS_H

#include <memory>

#include "src/darknet-adaptor/adaptors/IDarknetAdaptor.h"
#include "src/darknet-adaptor/adaptors/StructAdaptorHelper.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The original darknet adaptor for a network structure.
 */
class ANetworkAdaptor : virtual public IDarknetAdaptor,
                        virtual public StructAdaptorHelper
{
 public:
  using ANetworkAdaptorPtr = std::shared_ptr<ANetworkAdaptor>;

  virtual ~ANetworkAdaptor() = default;
  ANetworkAdaptor();
  ANetworkAdaptor(const ANetworkAdaptor&) = delete;
  ANetworkAdaptor(ANetworkAdaptor&&) = delete;

  ANetworkAdaptorPtr create();

  networkptr get();

  virtual bool perform(DarknetContextPtr dctx) override;

  /*
   * The getters.
   */

  int n();
  int batch();
  std::vector<uint64_t>& seen();
  std::vector<float>& badlabels_reject_threshold();
  std::vector<float>& delta_rolling_max();
  std::vector<float>& delta_rolling_avg();
  std::vector<float>& delta_rolling_std();
  int weights_reject_freq();
  int equidistant_point();
  float badlabels_rejection_percentage();
  float num_sigmas_reject_badlabels();
  float ema_alpha();
  std::vector<int>& cur_iteration();
  float loss_scale();
  std::vector<int>& t();
  float epoch();
  int subdivisions();
  std::vector<layer>& layers();
  std::vector<float>& output();
  learning_rate_policy policy();
  int benchmark_layers();
  std::vector<int>& total_bbox();
  std::vector<int>& rewritten_bbox();

  float learning_rate();
  float learning_rate_min();
  float learning_rate_max();
  int batches_per_cycle();
  int batches_cycle_mult();
  float momentum();
  float decay();
  float gamma();
  float scale();
  float power();
  int time_steps();
  int step();
  int max_batches();
  int num_boxes();
  int train_images_num();
  std::vector<float>& seq_scales();
  std::vector<float>& scales();
  std::vector<int>& steps();
  int num_steps();
  int burn_in();
  int cudnn_half();

  int adam();
  float B1();
  float B2();
  float eps();

  int inputs();
  int outputs();
  int truths();
  int notruth();
  int max_crop();
  int min_crop();
  float max_ratio();
  float min_ratio();
  int center();
  int flip();  // horizontal flip 50% probability augmentaiont for classifier
               // training (default = 1)
  int gaussian_noise();
  int blur();
  int mixup();
  float label_smooth_eps();
  int resize_step();
  int attention();
  int adversarial();
  float adversarial_lr();
  float max_chart_loss();
  int letter_box();
  int mosaic_bound();
  int contrastive();
  int contrastive_jit_flip();
  int contrastive_color();
  int unsupervised();
  float angle();
  float aspect();
  float exposure();
  float saturation();
  float hue();
  int random();
  int track();
  int augment_speed();
  int sequential_subdivisions();
  int init_sequential_subdivisions();
  int current_subdivision();
  int try_fix_nan();

  int gpu_index();
  tree* hierarchy();
  std::vector<float>& input();
  std::vector<float>& truth();
  std::vector<float>& delta();
  std::vector<float>& workspace();
  int train();
  int index();
  std::vector<float>& cost();
  float clip();

  //#ifdef GPU
  // float *input_gpu();
  // float *truth_gpu();
  std::vector<float>& delta_gpu();
  std::vector<float>& output_gpu();
  std::vector<float>& input_state_gpu();
  std::vector<float>& input_pinned_cpu();
  int input_pinned_cpu_flag();

  vvfloats& input_gpu();
  vvfloats& truth_gpu();
  vvfloats& input16_gpu();
  vvfloats& output16_gpu();

  std::vector<size_t>& max_input16_size();
  std::vector<size_t>& max_output16_size();
  int wait_stream();
  std::shared_ptr<void> cuda_graph();
  std::shared_ptr<void> cuda_graph_exec();
  int use_cuda_graph();
  std::vector<int>& cuda_graph_ready();
  std::vector<float>& global_delta_gpu();
  std::vector<float>& state_delta_gpu();
  size_t max_delta_gpu_size();
  //#endif  // GPU
  int optimized_memory();
  int dynamic_minibatch();
  size_t workspace_size_limit();

  /**
   * @brief Temporary fields for extended network params
   * previously stored in the obj.data file so now
   * only one cfg file is required.
   */
  std::string& names();
  std::string& lastweights();
  std::string& train_images();
  std::string& valid_images();
  std::string& backup_directory();
  int saveweights();
  int savelast();
  int classes();

  int h();
  int w();
  int c();

  /*
   * Setters.
   */

  void h(const int& nval);
  void w(const int& nval);
  void c(const int& nval);
  void n(const int& nval);
  void batch(const int& nval);
  void seen(const std::vector<uint64_t>& nval);
  void badlabels_reject_threshold(const std::vector<float>& nval);
  void delta_rolling_max(const std::vector<float>& nval);
  void delta_rolling_avg(const std::vector<float>& nval);
  void delta_rolling_std(const std::vector<float>& nval);
  void weights_reject_freq(const int& nval);
  void equidistant_point(const int& nval);
  void badlabels_rejection_percentage(const float& nval);
  void num_sigmas_reject_badlabels(const float& nval);
  void ema_alpha(const float& nval);
  void cur_iteration(const std::vector<int>& nval);
  void loss_scale(const float& nval);
  void t(const std::vector<int>& nval);
  void epoch(const float& nval);
  void subdivisions(const int& nval);
  void layers(const std::vector<layer>& nval);
  void output(const std::vector<float>& nval);
  void policy(const learning_rate_policy& nval);
  void benchmark_layers(const int& nval);
  void total_bbox(const std::vector<int>& nval);
  void rewritten_bbox(const std::vector<int>& nval);

  void learning_rate(const float& nval);
  void learning_rate_min(const float& nval);
  void learning_rate_max(const float& nval);
  void batches_per_cycle(const int& nval);
  void batches_cycle_mult(const int& nval);
  void momentum(const float& nval);
  void decay(const float& nval);
  void gamma(const float& nval);
  void scale(const float& nval);
  void power(const float& nval);
  void time_steps(const int& nval);
  void step(const int& nval);
  void max_batches(const int& nval);
  void num_boxes(const int& nval);
  void train_images_num(const int& nval);
  void seq_scales(const std::vector<float>& nval);
  void scales(const std::vector<float>& nval);
  void steps(const std::vector<int>& nval);
  void num_steps(const int& nval);
  void burn_in(const int& nval);
  void cudnn_half(const int& nval);

  void adam(const int& nval);
  void B1(const float& nval);
  void B2(const float& nval);
  void eps(const float& nval);

  void inputs(const int& nval);
  void outputs(const int& nval);
  void truths(const int& nval);
  void notruth(const int& nval);

  void max_crop(const int& nval);
  void min_crop(const int& nval);
  void max_ratio(const float& nval);
  void min_ratio(const float& nval);
  void center(const int& nval);
  void flip(const int& nval);  // horizontal flip 50% probability augmentaiont
                               // for classifier training (default = 1)
  void gaussian_noise(const int& nval);
  void blur(const int& nval);
  void mixup(const int& nval);
  void label_smooth_eps(const float& nval);
  void resize_step(const int& nval);
  void attention(const int& nval);
  void adversarial(const int& nval);
  void adversarial_lr(const float& nval);
  void max_chart_loss(const float& nval);
  void letter_box(const int& nval);
  void mosaic_bound(const int& nval);
  void contrastive(const int& nval);
  void contrastive_jit_flip(const int& nval);
  void contrastive_color(const int& nval);
  void unsupervised(const int& nval);
  void angle(const float& nval);
  void aspect(const float& nval);
  void exposure(const float& nval);
  void saturation(const float& nval);
  void hue(const float& nval);
  void random(const int& nval);
  void track(const int& nval);
  void augment_speed(const int& nval);
  void sequential_subdivisions(const int& nval);
  void init_sequential_subdivisions(const int& nval);
  void current_subdivision(const int& nval);
  void try_fix_nan(const int& nval);

  void gpu_index(const int& nval);
  void hierarchy(tree* nval);

  void input(const std::vector<float>& nval);
  void truth(const std::vector<float>& nval);
  void delta(const std::vector<float>& nval);
  void workspace(const std::vector<float>& nval);
  void train(const int& nval);
  void index(const int& nval);
  void cost(const std::vector<float>& nval);
  void clip(const float& nval);

  //#ifdef GPU
  // void input_gpu(const std::vector<float>& nval);
  // void truth_gpu(const std::vector<float>& nval);
  void delta_gpu(const std::vector<float>& nval);
  void output_gpu(const std::vector<float>& nval);

  void input_state_gpu(const std::vector<float>& nval);
  void input_pinned_cpu(const std::vector<float>& nval);
  void input_pinned_cpu_flag(const int& nval);

  void input_gpu(const vvfloats& nval);
  void truth_gpu(const vvfloats& nval);
  void input16_gpu(const vvfloats& nval);
  void output16_gpu(const vvfloats& nval);

  void max_input16_size(const std::vector<size_t>& nval);
  void max_output16_size(const std::vector<size_t>& nval);
  void wait_stream(const int& nval);

  void cuda_graph(const std::shared_ptr<void>& nval);
  void cuda_graph_exec(const std::shared_ptr<void>& nval);
  void use_cuda_graph(const int& nval);
  void cuda_graph_ready(const std::vector<int>& nval);

  void global_delta_gpu(const std::vector<float>& nval);
  void state_delta_gpu(const std::vector<float>& nval);
  void max_delta_gpu_size(const size_t& nval);
  //#endif  // GPU
  void optimized_memory(const int& nval);
  void dynamic_minibatch(const int& nval);
  void workspace_size_limit(const size_t& nval);

  void names(const std::string& nval);
  void lastweights(const std::string& nval);
  void train_images(const std::string& nval);
  void valid_images(const std::string& nval);
  void backup_directory(const std::string& nval);
  void saveweights(const int& nval);
  void savelast(const int& nval);
  void classes(const int& nval);

 private:
  static networkptr create_net();

  networkptr net;

  std::vector<uint64_t> vseen;
  std::vector<float> vbadlabels_reject_threshold;
  std::vector<float> vdelta_rolling_max;
  std::vector<float> vdelta_rolling_avg;
  std::vector<float> vdelta_rolling_std;
  std::vector<int> vcur_iteration;
  std::vector<int> vt;
  std::vector<layer> vlayers;
  std::vector<float> voutput;
  std::vector<int> vtotal_bbox;
  std::vector<int> vrewritten_bbox;
  std::vector<float> vseq_scales;
  std::vector<float> vscales;
  std::vector<int> vsteps;
  // std::vector<tree> vhierarchy;
  std::vector<float> vinput;
  std::vector<float> vtruth;
  std::vector<float> vdelta;
  std::vector<float> vworkspace;
  std::vector<float> vcost;
  std::vector<float> vdelta_gpu;
  std::vector<float> voutput_gpu;
  std::vector<float> vinput_state_gpu;
  std::vector<float> vinput_pinned_cpu;
  std::vector<size_t> vmax_input16_size;
  std::vector<size_t> vmax_output16_size;
  std::shared_ptr<void> mcuda_graph;
  std::shared_ptr<void> mcuda_graph_exec;
  std::vector<int> vcuda_graph_ready;
  std::vector<float> vglobal_delta_gpu;
  std::vector<float> vstate_delta_gpu;

  vvfloats vvinput_gpu;
  vvfloats vvtruth_gpu;
  vvfloats vvinput16_gpu;
  vvfloats vvoutput16_gpu;

  vrfloats vrinput_gpu;
  vrfloats vrtruth_gpu;
  vrfloats vrinput16_gpu;
  vrfloats vroutput16_gpu;

  std::string snames;
  std::string slastweights;
  std::string strain_images;
  std::string svalid_images;
  std::string sbackup_directory;
};

using ANetworkAdaptorPtr = ANetworkAdaptor::ANetworkAdaptorPtr;
using networkptr = ANetworkAdaptor::networkptr;

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_ANETWORK_ADAPTOR_CLASS_H
