#ifndef DARKNET_CXX_PORT_PROJECT_ANETWORK_ADAPTOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ANETWORK_ADAPTOR_CLASS_H

#include <memory>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/IDarknetAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/StructAdaptorHelper.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The original darknet adaptor for a network structure.
 *
 * Like ALayerAdaptor, the adaptor owns the memory of the wrapped darknet
 * network: the buffers are kept in it's own C++ containers and the struct
 * points at them, the darknet C code only computes with the struct.
 * Therefore:
 * - The struct returned by get() is a VIEW of the adaptor's storage. It keeps
 *   the adaptor alive. Darknet passes the network struct by value, so the
 *   copies of it share every buffer and counter (seen, cur_iteration, the
 *   layers, ...), only the scalar fields of a copy are it's own.
 * - The getters return const references: a buffer is changed only through
 *   its setter, which re-points the struct field. The setter of an empty
 *   container sets the field to the NULL.
 * - The structs of the layers are kept in one array, the layers field of the
 *   network, which darknet indexes. Every layer adaptor is bound to it's slot
 *   of the array (see ALayerAdaptor::bind()), so darknet computes with the
 *   very struct the adaptor keeps. The layers are ALayerAdaptor objects kept
 *   alive by the network.
 * - The adaptor has to be owned by a std::shared_ptr (see create()), which
 *   get() relies on.
 */
class ANetworkAdaptor : virtual public IDarknetAdaptor,
                        virtual public StructAdaptorHelper,
                        public std::enable_shared_from_this<ANetworkAdaptor>
{
 public:
  using ANetworkAdaptorPtr = std::shared_ptr<ANetworkAdaptor>;

  virtual ~ANetworkAdaptor() = default;
  ANetworkAdaptor();
  ANetworkAdaptor(const ANetworkAdaptor&) = delete;
  ANetworkAdaptor(ANetworkAdaptor&&) = delete;

  static ANetworkAdaptorPtr create();

  /**
   * @brief Returns the adaptor's network struct as a view.
   *
   * The returned pointer shares the ownership of the adaptor, so the struct
   * and its buffers stay valid for as long as the pointer is held. The
   * buffers belong to the adaptors' containers, the darknet C code computes
   * with them, while the darknet C API functions which (re)allocate them
   * (resize_network(), set_batch_network(), ...) find the adaptor of the view
   * in the NetworksRegistry. Throws std::bad_weak_ptr if the adaptor is not
   * owned by a std::shared_ptr.
   */
  networkptr get();

  virtual bool perform(DarknetContextPtr dctx) override;

  /*
   * The getters.
   */

  int n() const;
  int batch() const;
  const std::vector<uint64_t>& seen() const;
  const std::vector<float>& badlabels_reject_threshold() const;
  const std::vector<float>& delta_rolling_max() const;
  const std::vector<float>& delta_rolling_avg() const;
  const std::vector<float>& delta_rolling_std() const;
  int weights_reject_freq() const;
  int equidistant_point() const;
  float badlabels_rejection_percentage() const;
  float num_sigmas_reject_badlabels() const;
  float ema_alpha() const;
  const std::vector<int>& cur_iteration() const;
  float loss_scale() const;
  const std::vector<int>& t() const;
  float epoch() const;
  int subdivisions() const;
  const std::vector<ALayerAdaptorPtr>& layers() const;
  const std::vector<float>& output() const;
  learning_rate_policy policy() const;
  int benchmark_layers() const;
  const std::vector<int>& total_bbox() const;
  const std::vector<int>& rewritten_bbox() const;

  float learning_rate() const;
  float learning_rate_min() const;
  float learning_rate_max() const;
  int batches_per_cycle() const;
  int batches_cycle_mult() const;
  float momentum() const;
  float decay() const;
  float gamma() const;
  float scale() const;
  float power() const;
  int time_steps() const;
  int step() const;
  int max_batches() const;
  int num_boxes() const;
  int train_images_num() const;
  const std::vector<float>& seq_scales() const;
  const std::vector<float>& scales() const;
  const std::vector<int>& steps() const;
  int num_steps() const;
  int burn_in() const;
  int cudnn_half() const;

  int adam() const;
  float B1() const;
  float B2() const;
  float eps() const;

  int inputs() const;
  int outputs() const;
  int truths() const;
  int notruth() const;
  int max_crop() const;
  int min_crop() const;
  float max_ratio() const;
  float min_ratio() const;
  int center() const;
  int flip() const;  // horizontal flip 50% probability augmentaiont for
                     // classifier training (default = 1)
  int gaussian_noise() const;
  int blur() const;
  int mixup() const;
  float label_smooth_eps() const;
  int resize_step() const;
  int attention() const;
  int adversarial() const;
  float adversarial_lr() const;
  float max_chart_loss() const;
  int letter_box() const;
  int mosaic_bound() const;
  int contrastive() const;
  int contrastive_jit_flip() const;
  int contrastive_color() const;
  int unsupervised() const;
  float angle() const;
  float aspect() const;
  float exposure() const;
  float saturation() const;
  float hue() const;
  int random() const;
  int track() const;
  int augment_speed() const;
  int sequential_subdivisions() const;
  int init_sequential_subdivisions() const;
  int current_subdivision() const;
  int try_fix_nan() const;

  int gpu_index() const;
  tree* hierarchy() const;
  const std::vector<float>& input() const;
  const std::vector<float>& truth() const;
  const std::vector<float>& delta() const;
  const std::vector<float>& workspace() const;
  int train() const;
  int index() const;
  const std::vector<float>& cost() const;
  float clip() const;

  // #ifdef GPU
  //  float *input_gpu();
  //  float *truth_gpu();
  const std::vector<float>& delta_gpu() const;
  const std::vector<float>& output_gpu() const;
  const std::vector<float>& input_state_gpu() const;
  const std::vector<float>& input_pinned_cpu() const;
  int input_pinned_cpu_flag() const;

  const vvfloats& input_gpu() const;
  const vvfloats& truth_gpu() const;
  const vvfloats& input16_gpu() const;
  const vvfloats& output16_gpu() const;

  const std::vector<size_t>& max_input16_size() const;
  const std::vector<size_t>& max_output16_size() const;
  int wait_stream() const;
  std::shared_ptr<void> cuda_graph() const;
  std::shared_ptr<void> cuda_graph_exec() const;
  int use_cuda_graph() const;
  const std::vector<int>& cuda_graph_ready() const;
  const std::vector<float>& global_delta_gpu() const;
  const std::vector<float>& state_delta_gpu() const;
  size_t max_delta_gpu_size() const;
  // #endif  // GPU
  int optimized_memory() const;
  int dynamic_minibatch() const;
  size_t workspace_size_limit() const;

  /**
   * @brief Temporary fields for extended network params
   * previously stored in the obj.data file so now
   * only one cfg file is required.
   */
  const std::string& names() const;
  const std::string& lastweights() const;
  const std::string& train_images() const;
  const std::string& valid_images() const;
  const std::string& backup_directory() const;
  int saveweights() const;
  int savelast() const;
  int savechart() const;
  int classes() const;

  int h() const;
  int w() const;
  int c() const;

  /*
   * Setters.
   */

  void h(const int& nval);
  void w(const int& nval);
  void c(const int& nval);
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
  /**
   * @brief Sets the layers: the network allocates the array of the layer
   * structs (net->layers, net->n), binds every adaptor to it's slot and keeps
   * the adaptors alive. The NULL entries leave their slots zeroed.
   */
  void layers(const std::vector<ALayerAdaptorPtr>& nval);

  /**
   * @brief Replaces the layers with the given count of the zeroed layer
   * slots and no layer adaptors (net->layers, net->n).
   */
  void allocate_layers(int count);

  /**
   * @brief Binds the layer adaptor to the slot of the given index of the
   * allocated layers and keeps it alive.
   *
   * The layers are bound one by one as they are made, so the pointers at the
   * structs of the previously bound layers (the share_layer field) stay
   * valid.
   */
  void bind_layer(int index, const ALayerAdaptorPtr& adaptor);

  /// @brief The adaptor of the layer of the given index, none for the slot
  /// without an adaptor or the index out of range.
  ALayerAdaptorPtr layer_adaptor(int index) const;

  /// @brief Points the output field at the output buffer of a layer, the way
  /// darknet aliases the output of the last not COST layer.
  void borrow_output(float* buffer);

  /**
   * @brief Points the outputs and the output fields at the last not COST
   * layer ones, as the parser linked them (get_network_output_size() and
   * get_network_output()).
   */
  void link_output();

  /**
   * @brief Replaces the workspace with the zeroed one of the given size in
   * bytes (the NULL for none), the way darknet allocated it with the
   * xcalloc(1, bytes).
   */
  void workspace_bytes(size_t bytes);

  /**
   * @brief The resize_network(): resizes every layer to the new w x h
   * network input and reallocates the workspace.
   *
   * Throws NetworkError for a layer darknet is unable to resize, as the
   * original ended the process with an error.
   */
  void resize(int w, int h);

  /**
   * @brief The set_batch_network(): the new batch of the network and it's
   * layers, which keep their buffers, and the recalculated workspace.
   */
  void set_batch(int b);

  /// @brief The recalculate_workspace_size() of the convolutional and the
  /// connected layers, the workspace reallocated for the biggest one.
  void recalculate_workspace_size();

  /**
   * @brief The fuse_conv_batchnorm(): merges the batch normalization of the
   * convolutional layers into their weights and biases, releasing it's
   * buffers, and applies the weights normalization of the shortcut layers.
   */
  void fuse_conv_batchnorm();

  /// @brief The calculate_binary_weights() of the XNOR convolutional layers.
  void calculate_binary_weights();

  /**
   * @brief The copy_weights_net(): points the layers of the net_map network
   * at the layers of the net_train one, the single image batch, single step
   * inference ones. The net_map has to keep the slots of the net_train
   * layers count.
   */
  static void copy_weights_net(const network& net_train, network& net_map);
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

  // #ifdef GPU
  //  void input_gpu(const std::vector<float>& nval);
  //  void truth_gpu(const std::vector<float>& nval);
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
  // #endif  // GPU
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
  void savechart(const int& nval);
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
  std::vector<ALayerAdaptorPtr> mlayers;
  /// @brief The layers array, the layer adaptors are bound to it's slots.
  std::shared_ptr<std::vector<layer>> mslots;
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
