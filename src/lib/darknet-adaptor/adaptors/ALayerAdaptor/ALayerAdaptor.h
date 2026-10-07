#ifndef DARKNET_CXX_PORT_PROJECT_ALAYERADAPTOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ALAYERADAPTOR_CLASS_H

#include <memory>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/IDarknetAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/StructAdaptorHelper.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The original darknet adaptor for a layer structure.
 *
 * The adaptor owns the memory of the wrapped darknet layer: every buffer is
 * kept in the adaptor's own C++ containers and the pointer fields of the
 * struct point at them. The darknet C code only computes with the struct, it
 * never allocates, reallocates nor frees the layer memory, all of that is
 * done here and by the layer classes of the Layers directory. Therefore:
 * - The struct returned by get() is a VIEW of the adaptor's storage. It keeps
 *   the adaptor alive and it is the struct darknet computes with.
 * - The getters return const references: a buffer is changed only through
 *   its setter, which re-points the struct field. The setter of an empty
 *   container sets the field to the NULL, as darknet expects for an absent
 *   buffer.
 * - Some fields borrow a buffer of another layer instead, the way darknet
 *   aliases them (the borrow_*() setters): the previous layer output and
 *   delta of the DROPOUT and EMPTY layers, the output sub-layer ones of the
 *   RNN and CRNN layers, the weights of a layer sharing them with another one,
 *   the YOLO labels of a CONTRASTIVE layer. The getter of a borrowed field
 *   returns the empty own container and the owner of the buffer has to
 *   outlive the borrowing layer, as the layers of one network do.
 * - Sub-layers are child adaptors kept alive by their parent; they must not
 *   form cycles.
 * - The network keeps the structs of it's layers in one array, which darknet
 *   indexes. ANetworkAdaptor moves the layer struct into it's slot of that
 *   array with bind(), so the adaptor and darknet share the one struct.
 * - The adaptor has to be owned by a std::shared_ptr (see create()), which
 *   get() relies on.
 */
class ALayerAdaptor : virtual public IDarknetAdaptor,
                      virtual public StructAdaptorHelper,
                      public std::enable_shared_from_this<ALayerAdaptor>
{
 public:
  using ALayerAdaptorPtr = std::shared_ptr<ALayerAdaptor>;
  using forward_fn = decltype(layer::forward);
  using backward_fn = decltype(layer::backward);
  using update_fn = decltype(layer::update);

  virtual ~ALayerAdaptor() = default;
  ALayerAdaptor();
  ALayerAdaptor(const ALayerAdaptor&) = delete;
  ALayerAdaptor(ALayerAdaptor&&) = delete;

  static ALayerAdaptorPtr create();

  virtual bool perform(DarknetContextPtr dctx) override;

  /**
   * @brief Returns the adaptor's layer struct as a view.
   *
   * The returned pointer shares the ownership of the adaptor, so the struct
   * and its buffers stay valid for as long as the pointer is held. The
   * buffers belong to the adaptor's containers, darknet computes with them
   * but never frees nor reallocates them. Throws std::bad_weak_ptr if the
   * adaptor is not owned by a std::shared_ptr.
   */
  layerptr get();

  /**
   * @brief Moves the layer struct into the given slot and keeps using the
   * slot as the layer struct from now on.
   *
   * ANetworkAdaptor binds every layer to it's slot of the network layers
   * array. The slot owner has to be kept alive by the slot pointer (an
   * aliasing std::shared_ptr of the array). The layers referring to this
   * one's struct (the share_layer field of another layer) have to be
   * relink()ed afterwards.
   */
  void bind(layerptr slot);

  /**
   * @brief Points the share_layer and the sub-layer fields at the current
   * structs of their adaptors, after one of them was bound to a slot.
   */
  void relink();

  /**
   * @brief Tells if the adaptor keeps the buffers of the batch normalization
   * shared with another layer (the share_layer ones).
   */
  bool borrows_batchnorm() const;

  LAYER_TYPE type() const;
  ACTIVATION activation() const;
  ACTIVATION lstm_activation() const;
  COST_TYPE cost_type() const;

  forward_fn forward() const;
  backward_fn backward() const;
  update_fn update() const;
  //  void (*forward_gpu)(struct layer, struct network_state);
  //  void (*backward_gpu)(struct layer, struct network_state);
  //  void (*update_gpu)(struct layer, int, float, float, float, float);

  ALayerAdaptorPtr share_layer() const;
  int train() const;
  int avgpool() const;
  int batch_normalize() const;
  int shortcut() const;
  int batch() const;
  int dynamic_minibatch() const;
  int forced() const;
  int flipped() const;
  int inputs() const;
  int outputs() const;
  float mean_alpha() const;
  int nweights() const;
  int nbiases() const;
  int extra() const;
  int truths() const;
  int h() const;
  int w() const;
  int c() const;
  int out_h() const;
  int out_w() const;
  int out_c() const;
  int n() const;
  int max_boxes() const;
  int truth_size() const;
  int groups() const;
  int group_id() const;
  int size() const;
  int side() const;
  int stride() const;
  int stride_x() const;
  int stride_y() const;
  int dilation() const;
  int antialiasing() const;
  int maxpool_depth() const;
  int maxpool_zero_nonmax() const;
  int out_channels() const;
  float reverse() const;
  int coordconv() const;
  int flatten() const;
  int spatial() const;
  int pad() const;
  int sqrt() const;
  int flip() const;
  int index() const;
  int scale_wh() const;
  int binary() const;
  int xnor() const;
  int peephole() const;
  int use_bin_output() const;
  int keep_delta_gpu() const;
  int optimized_memory() const;
  int steps() const;
  int history_size() const;
  int bottleneck() const;
  float time_normalizer() const;
  int state_constrain() const;
  int hidden() const;
  int truth() const;
  float smooth() const;
  float dot() const;
  int deform() const;
  int grad_centr() const;
  int sway() const;
  int rotate() const;
  int stretch() const;
  int stretch_sway() const;
  float angle() const;
  float jitter() const;
  float resize() const;
  float saturation() const;
  float exposure() const;
  float shift() const;
  float ratio() const;
  float learning_rate_scale() const;
  float clip() const;
  int focal_loss() const;
  const std::vector<float>& classes_multipliers() const;
  float label_smooth_eps() const;
  int noloss() const;
  int softmax() const;
  int classes() const;
  int detection() const;
  int embedding_layer_id() const;
  const std::vector<float>& embedding_output() const;
  int embedding_size() const;
  float sim_thresh() const;
  int track_history_size() const;
  int dets_for_track() const;
  int dets_for_show() const;
  float track_ciou_norm() const;
  int coords() const;
  int background() const;
  int rescore() const;
  int objectness() const;
  int does_cost() const;
  int joint() const;
  int noadjust() const;
  int reorg() const;
  int log() const;
  int tanh() const;
  const std::vector<int>& mask() const;
  int total() const;
  float bflops() const;

  int adam() const;
  float B1() const;
  float B2() const;
  float eps() const;

  int t() const;

  float alpha() const;
  float beta() const;
  float kappa() const;

  float coord_scale() const;
  float object_scale() const;
  float noobject_scale() const;
  float mask_scale() const;
  float class_scale() const;
  int bias_match() const;
  float random() const;
  float ignore_thresh() const;
  float truth_thresh() const;
  float iou_thresh() const;
  float thresh() const;
  float focus() const;
  int classfix() const;
  int absolute() const;
  int assisted_excitation() const;

  int onlyforward() const;
  int stopbackward() const;
  int train_only_bn() const;
  int dont_update() const;
  int burnin_update() const;
  int dontload() const;
  int dontsave() const;
  int dontloadscales() const;
  int numload() const;

  float temperature() const;
  float probability() const;
  float dropblock_size_rel() const;
  int dropblock_size_abs() const;
  int dropblock() const;
  float scale() const;

  int receptive_w() const;
  int receptive_h() const;
  int receptive_w_scale() const;
  int receptive_h_scale() const;

  const std::string& cweights() const;
  const std::vector<int>& indexes() const;
  const std::vector<int>& input_layers() const;
  const std::vector<int>& input_sizes() const;
  const vrfloats& layers_output() const;
  const vrfloats& layers_delta() const;
  WEIGHTS_TYPE_T weights_type() const;
  WEIGHTS_NORMALIZATION_T weights_normalization() const;
  const std::vector<int>& map() const;
  const std::vector<int>& counts() const;
  const vvfloats& sums() const;
  const std::vector<float>& rand() const;
  const std::vector<float>& cost() const;
  const std::vector<int>& labels() const;
  const std::vector<int>& class_ids() const;
  int contrastive_neg_max() const;
  const std::vector<float>& cos_sim() const;
  const std::vector<float>& exp_cos_sim() const;
  const std::vector<float>& p_constrastive() const;
  const vcontrastive_params& contrast_p_gpu() const;
  const std::vector<float>& state() const;
  const std::vector<float>& prev_state() const;
  const std::vector<float>& forgot_state() const;
  const std::vector<float>& forgot_delta() const;
  const std::vector<float>& state_delta() const;
  const std::vector<float>& combine_cpu() const;
  const std::vector<float>& combine_delta_cpu() const;

  const std::vector<float>& concat() const;
  const std::vector<float>& concat_delta() const;

  const std::vector<float>& binary_weights() const;

  const std::vector<float>& biases() const;
  const std::vector<float>& bias_updates() const;

  const std::vector<float>& scales() const;
  const std::vector<float>& scale_updates() const;

  const std::vector<float>& weights_ema() const;
  const std::vector<float>& biases_ema() const;
  const std::vector<float>& scales_ema() const;

  const std::vector<float>& weights() const;
  const std::vector<float>& weight_updates() const;

  float scale_x_y() const;
  int objectness_smooth() const;
  int new_coords() const;
  int show_details() const;
  float max_delta() const;
  float uc_normalizer() const;
  float iou_normalizer() const;
  float obj_normalizer() const;
  float cls_normalizer() const;
  float delta_normalizer() const;
  IOU_LOSS iou_loss() const;
  IOU_LOSS iou_thresh_kind() const;
  NMS_KIND nms_kind() const;
  float beta_nms() const;
  YOLO_POINT yolo_point() const;

  const std::string& align_bit_weights_gpu() const;
  const std::vector<float>& mean_arr_gpu() const;
  const std::vector<float>& align_workspace_gpu() const;
  const std::vector<float>& transposed_align_workspace_gpu() const;
  int align_workspace_size() const;

  const std::string& align_bit_weights() const;
  const std::vector<float>& mean_arr() const;
  int align_bit_weights_size() const;
  int lda_align() const;
  int new_lda() const;
  int bit_align() const;

  const std::vector<float>& col_image() const;
  const std::vector<float>& delta() const;
  const std::vector<float>& output() const;
  const std::vector<float>& activation_input() const;
  int delta_pinned() const;
  int output_pinned() const;
  const std::vector<float>& loss() const;
  const std::vector<float>& squared() const;
  const std::vector<float>& norms() const;

  const std::vector<float>& spatial_mean() const;
  const std::vector<float>& mean() const;
  const std::vector<float>& variance() const;

  const std::vector<float>& mean_delta() const;
  const std::vector<float>& variance_delta() const;

  const std::vector<float>& rolling_mean() const;
  const std::vector<float>& rolling_variance() const;

  const std::vector<float>& x() const;
  const std::vector<float>& x_norm() const;

  const std::vector<float>& m() const;
  const std::vector<float>& v() const;

  const std::vector<float>& bias_m() const;
  const std::vector<float>& bias_v() const;
  const std::vector<float>& scale_m() const;
  const std::vector<float>& scale_v() const;

  const std::vector<float>& z_cpu() const;
  const std::vector<float>& r_cpu() const;
  const std::vector<float>& h_cpu() const;
  const std::vector<float>& stored_h_cpu() const;
  const std::vector<float>& prev_state_cpu() const;

  const std::vector<float>& temp_cpu() const;
  const std::vector<float>& temp2_cpu() const;
  const std::vector<float>& temp3_cpu() const;

  const std::vector<float>& dh_cpu() const;
  const std::vector<float>& hh_cpu() const;
  const std::vector<float>& prev_cell_cpu() const;
  const std::vector<float>& cell_cpu() const;
  const std::vector<float>& f_cpu() const;
  const std::vector<float>& i_cpu() const;
  const std::vector<float>& g_cpu() const;
  const std::vector<float>& o_cpu() const;
  const std::vector<float>& c_cpu() const;
  const std::vector<float>& stored_c_cpu() const;
  const std::vector<float>& dc_cpu() const;

  const std::vector<float>& binary_input() const;
  const std::vector<uint32_t>& bin_re_packed_input() const;
  const std::string& t_bit_input() const;

  ALayerAdaptorPtr input_layer() const;
  ALayerAdaptorPtr self_layer() const;
  ALayerAdaptorPtr output_layer() const;

  ALayerAdaptorPtr reset_layer() const;
  ALayerAdaptorPtr update_layer() const;
  ALayerAdaptorPtr state_layer() const;

  ALayerAdaptorPtr input_gate_layer() const;
  ALayerAdaptorPtr state_gate_layer() const;
  ALayerAdaptorPtr input_save_layer() const;
  ALayerAdaptorPtr state_save_layer() const;
  ALayerAdaptorPtr input_state_layer() const;
  ALayerAdaptorPtr state_state_layer() const;

  ALayerAdaptorPtr input_z_layer() const;
  ALayerAdaptorPtr state_z_layer() const;

  ALayerAdaptorPtr input_r_layer() const;
  ALayerAdaptorPtr state_r_layer() const;

  ALayerAdaptorPtr input_h_layer() const;
  ALayerAdaptorPtr state_h_layer() const;

  ALayerAdaptorPtr wz() const;
  ALayerAdaptorPtr uz() const;
  ALayerAdaptorPtr wr() const;
  ALayerAdaptorPtr ur() const;
  ALayerAdaptorPtr wh() const;
  ALayerAdaptorPtr uh() const;
  ALayerAdaptorPtr uo() const;
  ALayerAdaptorPtr wo() const;
  ALayerAdaptorPtr vo() const;
  ALayerAdaptorPtr uf() const;
  ALayerAdaptorPtr wf() const;
  ALayerAdaptorPtr vf() const;
  ALayerAdaptorPtr ui() const;
  ALayerAdaptorPtr wi() const;
  ALayerAdaptorPtr vi() const;
  ALayerAdaptorPtr ug() const;
  ALayerAdaptorPtr wg() const;

  tree* softmax_tree() const;

  size_t workspace_size() const;

  // #ifdef GPU
  const std::vector<int>& indexes_gpu() const;

  int stream() const;
  int wait_stream_id() const;

  const std::vector<float>& z_gpu() const;
  const std::vector<float>& r_gpu() const;
  const std::vector<float>& h_gpu() const;
  const std::vector<float>& stored_h_gpu() const;
  const std::vector<float>& bottelneck_hi_gpu() const;
  const std::vector<float>& bottelneck_delta_gpu() const;

  const std::vector<float>& temp_gpu() const;
  const std::vector<float>& temp2_gpu() const;
  const std::vector<float>& temp3_gpu() const;

  const std::vector<float>& dh_gpu() const;
  const std::vector<float>& hh_gpu() const;
  const std::vector<float>& prev_cell_gpu() const;
  const std::vector<float>& prev_state_gpu() const;
  const std::vector<float>& last_prev_state_gpu() const;
  const std::vector<float>& last_prev_cell_gpu() const;
  const std::vector<float>& cell_gpu() const;
  const std::vector<float>& f_gpu() const;
  const std::vector<float>& i_gpu() const;
  const std::vector<float>& g_gpu() const;
  const std::vector<float>& o_gpu() const;
  const std::vector<float>& c_gpu() const;
  const std::vector<float>& stored_c_gpu() const;
  const std::vector<float>& dc_gpu() const;

  // adam
  const std::vector<float>& m_gpu() const;
  const std::vector<float>& v_gpu() const;
  const std::vector<float>& bias_m_gpu() const;
  const std::vector<float>& scale_m_gpu() const;
  const std::vector<float>& bias_v_gpu() const;
  const std::vector<float>& scale_v_gpu() const;

  const std::vector<float>& combine_gpu() const;
  const std::vector<float>& combine_delta_gpu() const;

  const std::vector<float>& forgot_state_gpu() const;
  const std::vector<float>& forgot_delta_gpu() const;
  const std::vector<float>& state_gpu() const;
  const std::vector<float>& state_delta_gpu() const;
  const std::vector<float>& gate_gpu() const;
  const std::vector<float>& gate_delta_gpu() const;
  const std::vector<float>& save_gpu() const;
  const std::vector<float>& save_delta_gpu() const;
  const std::vector<float>& concat_gpu() const;
  const std::vector<float>& concat_delta_gpu() const;

  const std::vector<float>& binary_input_gpu() const;
  const std::vector<float>& binary_weights_gpu() const;
  const std::vector<float>& bin_conv_shortcut_in_gpu() const;
  const std::vector<float>& bin_conv_shortcut_out_gpu() const;

  const std::vector<float>& mean_gpu() const;
  const std::vector<float>& variance_gpu() const;
  const std::vector<float>& m_cbn_avg_gpu() const;
  const std::vector<float>& v_cbn_avg_gpu() const;

  const std::vector<float>& rolling_mean_gpu() const;
  const std::vector<float>& rolling_variance_gpu() const;

  const std::vector<float>& variance_delta_gpu() const;
  const std::vector<float>& mean_delta_gpu() const;

  const std::vector<float>& col_image_gpu() const;

  const std::vector<float>& x_gpu() const;
  const std::vector<float>& x_norm_gpu() const;
  const std::vector<float>& weights_gpu() const;
  const std::vector<float>& weight_updates_gpu() const;
  const std::vector<float>& weight_deform_gpu() const;
  const std::vector<float>& weight_change_gpu() const;

  const std::vector<float>& weights_gpu16() const;
  const std::vector<float>& weight_updates_gpu16() const;

  const std::vector<float>& biases_gpu() const;
  const std::vector<float>& bias_updates_gpu() const;
  const std::vector<float>& bias_change_gpu() const;

  const std::vector<float>& scales_gpu() const;
  const std::vector<float>& scale_updates_gpu() const;
  const std::vector<float>& scale_change_gpu() const;

  const std::vector<float>& input_antialiasing_gpu() const;
  const std::vector<float>& output_gpu() const;
  const std::vector<float>& output_avg_gpu() const;
  const std::vector<float>& activation_input_gpu() const;
  const std::vector<float>& loss_gpu() const;
  const std::vector<float>& delta_gpu() const;
  const std::vector<float>& cos_sim_gpu() const;
  const std::vector<float>& rand_gpu() const;
  const std::vector<float>& drop_blocks_scale() const;
  const std::vector<float>& drop_blocks_scale_gpu() const;
  const std::vector<float>& squared_gpu() const;
  const std::vector<float>& norms_gpu() const;

  const std::vector<float>& gt_gpu() const;
  const std::vector<float>& a_avg_gpu() const;

  const std::vector<int>& input_sizes_gpu() const;
  const vrfloats& layers_output_gpu() const;
  const vrfloats& layers_delta_gpu() const;

#ifdef CUDNN
  cudnnTensorDescriptor_t srcTensorDesc, dstTensorDesc;
  cudnnTensorDescriptor_t srcTensorDesc16, dstTensorDesc16;
  cudnnTensorDescriptor_t dsrcTensorDesc, ddstTensorDesc;
  cudnnTensorDescriptor_t dsrcTensorDesc16, ddstTensorDesc16;
  cudnnTensorDescriptor_t normTensorDesc, normDstTensorDesc,
      normDstTensorDescF16;
  cudnnFilterDescriptor_t weightDesc, weightDesc16;
  cudnnFilterDescriptor_t dweightDesc, dweightDesc16;
  cudnnConvolutionDescriptor_t convDesc;
  cudnnConvolutionFwdAlgo_t fw_algo, fw_algo16;
  cudnnConvolutionBwdDataAlgo_t bd_algo, bd_algo16;
  cudnnConvolutionBwdFilterAlgo_t bf_algo, bf_algo16;
  cudnnPoolingDescriptor_t poolingDesc;
#else   // CUDNN

  void* srcTensorDesc() const;
  void* dstTensorDesc() const;
  void* srcTensorDesc16() const;
  void* dstTensorDesc16() const;
  void* dsrcTensorDesc() const;
  void* ddstTensorDesc() const;
  void* dsrcTensorDesc16() const;
  void* ddstTensorDesc16() const;
  void* normTensorDesc() const;
  void* normDstTensorDesc() const;
  void* normDstTensorDescF16() const;
  void* weightDesc() const;
  void* weightDesc16() const;
  void* dweightDesc() const;
  void* dweightDesc16() const;
  void* convDesc() const;

  UNUSED_ENUM_TYPE fw_algo() const;
  UNUSED_ENUM_TYPE fw_algo16() const;
  UNUSED_ENUM_TYPE bd_algo() const;
  UNUSED_ENUM_TYPE bd_algo16() const;
  UNUSED_ENUM_TYPE bf_algo() const;
  UNUSED_ENUM_TYPE bf_algo16() const;

  void* poolingDesc() const;
#endif  // CUDNN

  /**
   * Setters.
   */

  void type(const LAYER_TYPE& nval);
  void activation(const ACTIVATION& nval);
  void lstm_activation(const ACTIVATION& nval);
  void cost_type(const COST_TYPE& nval);

  void forward(forward_fn nval);
  void backward(backward_fn nval);
  void update(update_fn nval);

  /**
   * The borrowed buffers: the field points at the buffer of another layer,
   * which owns it, and the own container of the field is released. See the
   * class description.
   */
  void borrow_output(float* buffer);
  void borrow_delta(float* buffer);
  void borrow_weights(float* buffer);
  void borrow_weight_updates(float* buffer);
  void borrow_biases(float* buffer);
  void borrow_bias_updates(float* buffer);
  void borrow_scales(float* buffer);
  void borrow_scale_updates(float* buffer);
  void borrow_mean(float* buffer);
  void borrow_variance(float* buffer);
  void borrow_mean_delta(float* buffer);
  void borrow_variance_delta(float* buffer);
  void borrow_rolling_mean(float* buffer);
  void borrow_rolling_variance(float* buffer);
  void borrow_labels(int* buffer);
  void borrow_class_ids(int* buffer);

  /**
   * @brief Keeps the given softmax tree alive with the layer and points the
   * softmax_tree field at it, the NULL for none. The softmax_tree(tree*)
   * setter borrows the tree instead.
   */
  void own_softmax_tree(std::shared_ptr<tree> nval);

  // The sub-layer setters (share_layer and input_layer .. wg) keep the child
  // adaptor alive and point the struct field at the child's layer struct.
  void share_layer(const ALayerAdaptorPtr& nval);
  void share_layer(ALayerAdaptorPtr&& nval);
  void train(const int& nval);
  void avgpool(const int& nval);
  void batch_normalize(const int& nval);
  void shortcut(const int& nval);
  void batch(const int& nval);
  void dynamic_minibatch(const int& nval);
  void forced(const int& nval);
  void flipped(const int& nval);
  void inputs(const int& nval);
  void outputs(const int& nval);
  void mean_alpha(const float& nval);
  void nweights(const int& nval);
  void nbiases(const int& nval);
  void extra(const int& nval);
  void truths(const int& nval);
  void h(const int& nval);
  void w(const int& nval);
  void c(const int& nval);
  void out_h(const int& nval);
  void out_w(const int& nval);
  void out_c(const int& nval);
  void n(const int& nval);
  void max_boxes(const int& nval);
  void truth_size(const int& nval);
  void groups(const int& nval);
  void group_id(const int& nval);
  void size(const int& nval);
  void side(const int& nval);
  void stride(const int& nval);
  void stride_x(const int& nval);
  void stride_y(const int& nval);
  void dilation(const int& nval);
  void antialiasing(const int& nval);
  void maxpool_depth(const int& nval);
  void maxpool_zero_nonmax(const int& nval);
  void out_channels(const int& nval);
  void reverse(const float& nval);
  void coordconv(const int& nval);
  void flatten(const int& nval);
  void spatial(const int& nval);
  void pad(const int& nval);
  void sqrt(const int& nval);
  void flip(const int& nval);
  void index(const int& nval);
  void scale_wh(const int& nval);
  void binary(const int& nval);
  void xnor(const int& nval);
  void peephole(const int& nval);
  void use_bin_output(const int& nval);
  void keep_delta_gpu(const int& nval);
  void optimized_memory(const int& nval);
  void steps(const int& nval);
  void history_size(const int& nval);
  void bottleneck(const int& nval);
  void time_normalizer(const float& nval);
  void state_constrain(const int& nval);
  void hidden(const int& nval);
  void truth(const int& nval);
  void smooth(const float& nval);
  void dot(const float& nval);
  void deform(const int& nval);
  void grad_centr(const int& nval);
  void sway(const int& nval);
  void rotate(const int& nval);
  void stretch(const int& nval);
  void stretch_sway(const int& nval);
  void angle(const float& nval);
  void jitter(const float& nval);
  void resize(const float& nval);
  void saturation(const float& nval);
  void exposure(const float& nval);
  void shift(const float& nval);
  void ratio(const float& nval);
  void learning_rate_scale(const float& nval);
  void clip(const float& nval);
  void focal_loss(const int& nval);
  void classes_multipliers(const std::vector<float>& nval);
  void classes_multipliers(std::vector<float>&& nval);
  void label_smooth_eps(const float& nval);
  void noloss(const int& nval);
  void softmax(const int& nval);
  void classes(const int& nval);
  void detection(const int& nval);
  void embedding_layer_id(const int& nval);
  void embedding_output(const std::vector<float>& nval);
  void embedding_output(std::vector<float>&& nval);
  void embedding_size(const int& nval);
  void sim_thresh(const float& nval);
  void track_history_size(const int& nval);
  void dets_for_track(const int& nval);
  void dets_for_show(const int& nval);
  void track_ciou_norm(const float& nval);
  void coords(const int& nval);
  void background(const int& nval);
  void rescore(const int& nval);
  void objectness(const int& nval);
  void does_cost(const int& nval);
  void joint(const int& nval);
  void noadjust(const int& nval);
  void reorg(const int& nval);
  void log(const int& nval);
  void tanh(const int& nval);
  void mask(const std::vector<int>& nval);
  void mask(std::vector<int>&& nval);
  void total(const int& nval);
  void bflops(const float& nval);

  void adam(const int& nval);
  void B1(const float& nval);
  void B2(const float& nval);
  void eps(const float& nval);

  void t(const int& nval);

  void alpha(const float& nval);
  void beta(const float& nval);
  void kappa(const float& nval);

  void coord_scale(const float& nval);
  void object_scale(const float& nval);
  void noobject_scale(const float& nval);
  void mask_scale(const float& nval);
  void class_scale(const float& nval);
  void bias_match(const int& nval);
  void random(const float& nval);
  void ignore_thresh(const float& nval);
  void truth_thresh(const float& nval);
  void iou_thresh(const float& nval);
  void thresh(const float& nval);
  void focus(const float& nval);
  void classfix(const int& nval);
  void absolute(const int& nval);
  void assisted_excitation(const int& nval);

  void onlyforward(const int& nval);
  void stopbackward(const int& nval);
  void train_only_bn(const int& nval);
  void dont_update(const int& nval);
  void burnin_update(const int& nval);
  void dontload(const int& nval);
  void dontsave(const int& nval);
  void dontloadscales(const int& nval);
  void numload(const int& nval);

  void temperature(const float& nval);
  void probability(const float& nval);
  void dropblock_size_rel(const float& nval);
  void dropblock_size_abs(const int& nval);
  void dropblock(const int& nval);
  void scale(const float& nval);

  void receptive_w(const int& nval);
  void receptive_h(const int& nval);
  void receptive_w_scale(const int& nval);
  void receptive_h_scale(const int& nval);

  void cweights(const std::string& nval);
  void cweights(std::string&& nval);
  void indexes(const std::vector<int>& nval);
  void indexes(std::vector<int>&& nval);
  void input_layers(const std::vector<int>& nval);
  void input_layers(std::vector<int>&& nval);
  void input_sizes(const std::vector<int>& nval);
  void input_sizes(std::vector<int>&& nval);
  // layers_output/layers_delta (and their _gpu twins) borrow the output and
  // delta buffers of other layers, as darknet's parse_shortcut() does: only
  // the pointer array is stored, the buffers must outlive the view.
  void layers_output(const vrfloats& nval);
  void layers_output(vrfloats&& nval);
  void layers_delta(const vrfloats& nval);
  void layers_delta(vrfloats&& nval);
  void weights_type(const WEIGHTS_TYPE_T& nval);
  void weights_type(WEIGHTS_TYPE_T&& nval);
  void weights_normalization(const WEIGHTS_NORMALIZATION_T& nval);
  void weights_normalization(WEIGHTS_NORMALIZATION_T&& nval);
  void map(const std::vector<int>& nval);
  void map(std::vector<int>&& nval);
  void counts(const std::vector<int>& nval);
  void counts(std::vector<int>&& nval);
  void sums(const vvfloats& nval);
  void rand(const std::vector<float>& nval);
  void rand(std::vector<float>&& nval);
  void cost(const std::vector<float>& nval);
  void cost(std::vector<float>&& nval);
  void labels(const std::vector<int>& nval);
  void labels(std::vector<int>&& nval);
  void class_ids(const std::vector<int>& nval);
  void class_ids(std::vector<int>&& nval);
  void contrastive_neg_max(const int& nval);
  void cos_sim(const std::vector<float>& nval);
  void cos_sim(std::vector<float>&& nval);
  void exp_cos_sim(const std::vector<float>& nval);
  void exp_cos_sim(std::vector<float>&& nval);
  void p_constrastive(const std::vector<float>& nval);
  void p_constrastive(std::vector<float>&& nval);
  void contrast_p_gpu(const vcontrastive_params& nval);
  void contrast_p_gpu(vcontrastive_params&& nval);
  void state(const std::vector<float>& nval);
  void state(std::vector<float>&& nval);
  void prev_state(const std::vector<float>& nval);
  void prev_state(std::vector<float>&& nval);
  void forgot_state(const std::vector<float>& nval);
  void forgot_state(std::vector<float>&& nval);
  void forgot_delta(const std::vector<float>& nval);
  void forgot_delta(std::vector<float>&& nval);
  void state_delta(const std::vector<float>& nval);
  void state_delta(std::vector<float>&& nval);
  void combine_cpu(const std::vector<float>& nval);
  void combine_cpu(std::vector<float>&& nval);
  void combine_delta_cpu(const std::vector<float>& nval);
  void combine_delta_cpu(std::vector<float>&& nval);

  void concat(const std::vector<float>& nval);
  void concat(std::vector<float>&& nval);
  void concat_delta(const std::vector<float>& nval);
  void concat_delta(std::vector<float>&& nval);

  void binary_weights(const std::vector<float>& nval);
  void binary_weights(std::vector<float>&& nval);

  void biases(const std::vector<float>& nval);
  void biases(std::vector<float>&& nval);
  void bias_updates(const std::vector<float>& nval);
  void bias_updates(std::vector<float>&& nval);

  void scales(const std::vector<float>& nval);
  void scales(std::vector<float>&& nval);
  void scale_updates(const std::vector<float>& nval);
  void scale_updates(std::vector<float>&& nval);

  void weights_ema(const std::vector<float>& nval);
  void weights_ema(std::vector<float>&& nval);
  void biases_ema(const std::vector<float>& nval);
  void biases_ema(std::vector<float>&& nval);
  void scales_ema(const std::vector<float>& nval);
  void scales_ema(std::vector<float>&& nval);

  void weights(const std::vector<float>& nval);
  void weights(std::vector<float>&& nval);
  void weight_updates(const std::vector<float>& nval);
  void weight_updates(std::vector<float>&& nval);

  void scale_x_y(const float& nval);
  void objectness_smooth(const int& nval);
  void new_coords(const int& nval);
  void show_details(const int& nval);
  void max_delta(const float& nval);
  void uc_normalizer(const float& nval);
  void iou_normalizer(const float& nval);
  void obj_normalizer(const float& nval);
  void cls_normalizer(const float& nval);
  void delta_normalizer(const float& nval);
  void iou_loss(const IOU_LOSS& nval);
  void iou_loss(IOU_LOSS&& nval);
  void iou_thresh_kind(const IOU_LOSS& nval);
  void iou_thresh_kind(IOU_LOSS&& nval);
  void nms_kind(const NMS_KIND& nval);
  void nms_kind(NMS_KIND&& nval);
  void beta_nms(const float& nval);
  void yolo_point(const YOLO_POINT& nval);

  void align_bit_weights_gpu(const std::string& nval);
  void align_bit_weights_gpu(std::string&& nval);
  void mean_arr_gpu(const std::vector<float>& nval);
  void mean_arr_gpu(std::vector<float>&& nval);
  void align_workspace_gpu(const std::vector<float>& nval);
  void align_workspace_gpu(std::vector<float>&& nval);
  void transposed_align_workspace_gpu(const std::vector<float>& nval);
  void transposed_align_workspace_gpu(std::vector<float>&& nval);
  void align_workspace_size(const int& nval);

  void align_bit_weights(const std::string& nval);
  void align_bit_weights(std::string&& nval);
  void mean_arr(const std::vector<float>& nval);
  void mean_arr(std::vector<float>&& nval);
  void align_bit_weights_size(const int& nval);
  void lda_align(const int& nval);
  void new_lda(const int& nval);
  void bit_align(const int& nval);

  void col_image(const std::vector<float>& nval);
  void col_image(std::vector<float>&& nval);
  void delta(const std::vector<float>& nval);
  void delta(std::vector<float>&& nval);
  void output(const std::vector<float>& nval);
  void output(std::vector<float>&& nval);
  void activation_input(const std::vector<float>& nval);
  void activation_input(std::vector<float>&& nval);
  void delta_pinned(const int& nval);
  void output_pinned(const int& nval);
  void loss(const std::vector<float>& nval);
  void loss(std::vector<float>&& nval);
  void squared(const std::vector<float>& nval);
  void squared(std::vector<float>&& nval);
  void norms(const std::vector<float>& nval);
  void norms(std::vector<float>&& nval);

  void spatial_mean(const std::vector<float>& nval);
  void spatial_mean(std::vector<float>&& nval);
  void mean(const std::vector<float>& nval);
  void mean(std::vector<float>&& nval);
  void variance(const std::vector<float>& nval);
  void variance(std::vector<float>&& nval);

  void mean_delta(const std::vector<float>& nval);
  void mean_delta(std::vector<float>&& nval);
  void variance_delta(const std::vector<float>& nval);
  void variance_delta(std::vector<float>&& nval);

  void rolling_mean(const std::vector<float>& nval);
  void rolling_mean(std::vector<float>&& nval);
  void rolling_variance(const std::vector<float>& nval);
  void rolling_variance(std::vector<float>&& nval);

  void x(const std::vector<float>& nval);
  void x(std::vector<float>&& nval);
  void x_norm(const std::vector<float>& nval);
  void x_norm(std::vector<float>&& nval);

  void m(const std::vector<float>& nval);
  void m(std::vector<float>&& nval);
  void v(const std::vector<float>& nval);
  void v(std::vector<float>&& nval);

  void bias_m(const std::vector<float>& nval);
  void bias_m(std::vector<float>&& nval);
  void bias_v(const std::vector<float>& nval);
  void bias_v(std::vector<float>&& nval);
  void scale_m(const std::vector<float>& nval);
  void scale_m(std::vector<float>&& nval);
  void scale_v(const std::vector<float>& nval);
  void scale_v(std::vector<float>&& nval);

  void z_cpu(const std::vector<float>& nval);
  void z_cpu(std::vector<float>&& nval);
  void r_cpu(const std::vector<float>& nval);
  void r_cpu(std::vector<float>&& nval);
  void h_cpu(const std::vector<float>& nval);
  void h_cpu(std::vector<float>&& nval);
  void stored_h_cpu(const std::vector<float>& nval);
  void stored_h_cpu(std::vector<float>&& nval);
  void prev_state_cpu(const std::vector<float>& nval);
  void prev_state_cpu(std::vector<float>&& nval);

  void temp_cpu(const std::vector<float>& nval);
  void temp_cpu(std::vector<float>&& nval);
  void temp2_cpu(const std::vector<float>& nval);
  void temp2_cpu(std::vector<float>&& nval);
  void temp3_cpu(const std::vector<float>& nval);
  void temp3_cpu(std::vector<float>&& nval);

  void dh_cpu(const std::vector<float>& nval);
  void dh_cpu(std::vector<float>&& nval);
  void hh_cpu(const std::vector<float>& nval);
  void hh_cpu(std::vector<float>&& nval);
  void prev_cell_cpu(const std::vector<float>& nval);
  void prev_cell_cpu(std::vector<float>&& nval);
  void cell_cpu(const std::vector<float>& nval);
  void cell_cpu(std::vector<float>&& nval);
  void f_cpu(const std::vector<float>& nval);
  void f_cpu(std::vector<float>&& nval);
  void i_cpu(const std::vector<float>& nval);
  void i_cpu(std::vector<float>&& nval);
  void g_cpu(const std::vector<float>& nval);
  void g_cpu(std::vector<float>&& nval);
  void o_cpu(const std::vector<float>& nval);
  void o_cpu(std::vector<float>&& nval);
  void c_cpu(const std::vector<float>& nval);
  void c_cpu(std::vector<float>&& nval);
  void stored_c_cpu(const std::vector<float>& nval);
  void stored_c_cpu(std::vector<float>&& nval);
  void dc_cpu(const std::vector<float>& nval);
  void dc_cpu(std::vector<float>&& nval);

  void binary_input(const std::vector<float>& nval);
  void binary_input(std::vector<float>&& nval);
  void bin_re_packed_input(const std::vector<uint32_t>& nval);
  void bin_re_packed_input(std::vector<uint32_t>&& nval);
  void t_bit_input(const std::string& nval);
  void t_bit_input(std::string&& nval);

  void input_layer(const ALayerAdaptorPtr& nval);
  void input_layer(ALayerAdaptorPtr&& nval);
  void self_layer(const ALayerAdaptorPtr& nval);
  void self_layer(ALayerAdaptorPtr&& nval);
  void output_layer(const ALayerAdaptorPtr& nval);
  void output_layer(ALayerAdaptorPtr&& nval);

  void reset_layer(const ALayerAdaptorPtr& nval);
  void reset_layer(ALayerAdaptorPtr&& nval);
  void update_layer(const ALayerAdaptorPtr& nval);
  void update_layer(ALayerAdaptorPtr&& nval);
  void state_layer(const ALayerAdaptorPtr& nval);
  void state_layer(ALayerAdaptorPtr&& nval);

  void input_gate_layer(const ALayerAdaptorPtr& nval);
  void input_gate_layer(ALayerAdaptorPtr&& nval);
  void state_gate_layer(const ALayerAdaptorPtr& nval);
  void state_gate_layer(ALayerAdaptorPtr&& nval);
  void input_save_layer(const ALayerAdaptorPtr& nval);
  void input_save_layer(ALayerAdaptorPtr&& nval);
  void state_save_layer(const ALayerAdaptorPtr& nval);
  void state_save_layer(ALayerAdaptorPtr&& nval);
  void input_state_layer(const ALayerAdaptorPtr& nval);
  void input_state_layer(ALayerAdaptorPtr&& nval);
  void state_state_layer(const ALayerAdaptorPtr& nval);
  void state_state_layer(ALayerAdaptorPtr&& nval);

  void input_z_layer(const ALayerAdaptorPtr& nval);
  void input_z_layer(ALayerAdaptorPtr&& nval);
  void state_z_layer(const ALayerAdaptorPtr& nval);
  void state_z_layer(ALayerAdaptorPtr&& nval);

  void input_r_layer(const ALayerAdaptorPtr& nval);
  void input_r_layer(ALayerAdaptorPtr&& nval);
  void state_r_layer(const ALayerAdaptorPtr& nval);
  void state_r_layer(ALayerAdaptorPtr&& nval);

  void input_h_layer(const ALayerAdaptorPtr& nval);
  void input_h_layer(ALayerAdaptorPtr&& nval);
  void state_h_layer(const ALayerAdaptorPtr& nval);
  void state_h_layer(ALayerAdaptorPtr&& nval);

  void wz(const ALayerAdaptorPtr& nval);
  void wz(ALayerAdaptorPtr&& nval);
  void uz(const ALayerAdaptorPtr& nval);
  void uz(ALayerAdaptorPtr&& nval);
  void wr(const ALayerAdaptorPtr& nval);
  void wr(ALayerAdaptorPtr&& nval);
  void ur(const ALayerAdaptorPtr& nval);
  void ur(ALayerAdaptorPtr&& nval);
  void wh(const ALayerAdaptorPtr& nval);
  void wh(ALayerAdaptorPtr&& nval);
  void uh(const ALayerAdaptorPtr& nval);
  void uh(ALayerAdaptorPtr&& nval);
  void uo(const ALayerAdaptorPtr& nval);
  void uo(ALayerAdaptorPtr&& nval);
  void wo(const ALayerAdaptorPtr& nval);
  void wo(ALayerAdaptorPtr&& nval);
  void vo(const ALayerAdaptorPtr& nval);
  void vo(ALayerAdaptorPtr&& nval);
  void uf(const ALayerAdaptorPtr& nval);
  void uf(ALayerAdaptorPtr&& nval);
  void wf(const ALayerAdaptorPtr& nval);
  void wf(ALayerAdaptorPtr&& nval);
  void vf(const ALayerAdaptorPtr& nval);
  void vf(ALayerAdaptorPtr&& nval);
  void ui(const ALayerAdaptorPtr& nval);
  void ui(ALayerAdaptorPtr&& nval);
  void wi(const ALayerAdaptorPtr& nval);
  void wi(ALayerAdaptorPtr&& nval);
  void vi(const ALayerAdaptorPtr& nval);
  void vi(ALayerAdaptorPtr&& nval);
  void ug(const ALayerAdaptorPtr& nval);
  void ug(ALayerAdaptorPtr&& nval);
  void wg(const ALayerAdaptorPtr& nval);
  void wg(ALayerAdaptorPtr&& nval);

  void softmax_tree(tree* nval);

  void workspace_size(const size_t& nval);
  void workspace_size(size_t&& nval);

  // #ifdef GPU
  void indexes_gpu(const std::vector<int>& nval);
  void indexes_gpu(std::vector<int>&& nval);

  void stream(const int& nval);
  void wait_stream_id(const int& nval);

  void z_gpu(const std::vector<float>& nval);
  void z_gpu(std::vector<float>&& nval);
  void r_gpu(const std::vector<float>& nval);
  void r_gpu(std::vector<float>&& nval);
  void h_gpu(const std::vector<float>& nval);
  void h_gpu(std::vector<float>&& nval);
  void stored_h_gpu(const std::vector<float>& nval);
  void stored_h_gpu(std::vector<float>&& nval);
  void bottelneck_hi_gpu(const std::vector<float>& nval);
  void bottelneck_hi_gpu(std::vector<float>&& nval);
  void bottelneck_delta_gpu(const std::vector<float>& nval);
  void bottelneck_delta_gpu(std::vector<float>&& nval);

  void temp_gpu(const std::vector<float>& nval);
  void temp_gpu(std::vector<float>&& nval);
  void temp2_gpu(const std::vector<float>& nval);
  void temp2_gpu(std::vector<float>&& nval);
  void temp3_gpu(const std::vector<float>& nval);
  void temp3_gpu(std::vector<float>&& nval);

  void dh_gpu(const std::vector<float>& nval);
  void dh_gpu(std::vector<float>&& nval);
  void hh_gpu(const std::vector<float>& nval);
  void hh_gpu(std::vector<float>&& nval);
  void prev_cell_gpu(const std::vector<float>& nval);
  void prev_cell_gpu(std::vector<float>&& nval);
  void prev_state_gpu(const std::vector<float>& nval);
  void prev_state_gpu(std::vector<float>&& nval);
  void last_prev_state_gpu(const std::vector<float>& nval);
  void last_prev_state_gpu(std::vector<float>&& nval);
  void last_prev_cell_gpu(const std::vector<float>& nval);
  void last_prev_cell_gpu(std::vector<float>&& nval);
  void cell_gpu(const std::vector<float>& nval);
  void cell_gpu(std::vector<float>&& nval);
  void f_gpu(const std::vector<float>& nval);
  void f_gpu(std::vector<float>&& nval);
  void i_gpu(const std::vector<float>& nval);
  void i_gpu(std::vector<float>&& nval);
  void g_gpu(const std::vector<float>& nval);
  void g_gpu(std::vector<float>&& nval);
  void o_gpu(const std::vector<float>& nval);
  void o_gpu(std::vector<float>&& nval);
  void c_gpu(const std::vector<float>& nval);
  void c_gpu(std::vector<float>&& nval);
  void stored_c_gpu(const std::vector<float>& nval);
  void stored_c_gpu(std::vector<float>&& nval);
  void dc_gpu(const std::vector<float>& nval);
  void dc_gpu(std::vector<float>&& nval);

  // adam
  void m_gpu(const std::vector<float>& nval);
  void m_gpu(std::vector<float>&& nval);
  void v_gpu(const std::vector<float>& nval);
  void v_gpu(std::vector<float>&& nval);
  void bias_m_gpu(const std::vector<float>& nval);
  void bias_m_gpu(std::vector<float>&& nval);
  void scale_m_gpu(const std::vector<float>& nval);
  void scale_m_gpu(std::vector<float>&& nval);
  void bias_v_gpu(const std::vector<float>& nval);
  void bias_v_gpu(std::vector<float>&& nval);
  void scale_v_gpu(const std::vector<float>& nval);
  void scale_v_gpu(std::vector<float>&& nval);

  void combine_gpu(const std::vector<float>& nval);
  void combine_gpu(std::vector<float>&& nval);
  void combine_delta_gpu(const std::vector<float>& nval);
  void combine_delta_gpu(std::vector<float>&& nval);

  void forgot_state_gpu(const std::vector<float>& nval);
  void forgot_state_gpu(std::vector<float>&& nval);
  void forgot_delta_gpu(const std::vector<float>& nval);
  void forgot_delta_gpu(std::vector<float>&& nval);
  void state_gpu(const std::vector<float>& nval);
  void state_gpu(std::vector<float>&& nval);
  void state_delta_gpu(const std::vector<float>& nval);
  void state_delta_gpu(std::vector<float>&& nval);
  void gate_gpu(const std::vector<float>& nval);
  void gate_gpu(std::vector<float>&& nval);
  void gate_delta_gpu(const std::vector<float>& nval);
  void gate_delta_gpu(std::vector<float>&& nval);
  void save_gpu(const std::vector<float>& nval);
  void save_gpu(std::vector<float>&& nval);
  void save_delta_gpu(const std::vector<float>& nval);
  void save_delta_gpu(std::vector<float>&& nval);
  void concat_gpu(const std::vector<float>& nval);
  void concat_gpu(std::vector<float>&& nval);
  void concat_delta_gpu(const std::vector<float>& nval);
  void concat_delta_gpu(std::vector<float>&& nval);

  void binary_input_gpu(const std::vector<float>& nval);
  void binary_input_gpu(std::vector<float>&& nval);
  void binary_weights_gpu(const std::vector<float>& nval);
  void binary_weights_gpu(std::vector<float>&& nval);
  void bin_conv_shortcut_in_gpu(const std::vector<float>& nval);
  void bin_conv_shortcut_in_gpu(std::vector<float>&& nval);
  void bin_conv_shortcut_out_gpu(const std::vector<float>& nval);
  void bin_conv_shortcut_out_gpu(std::vector<float>&& nval);

  void mean_gpu(const std::vector<float>& nval);
  void mean_gpu(std::vector<float>&& nval);
  void variance_gpu(const std::vector<float>& nval);
  void variance_gpu(std::vector<float>&& nval);
  void m_cbn_avg_gpu(const std::vector<float>& nval);
  void m_cbn_avg_gpu(std::vector<float>&& nval);
  void v_cbn_avg_gpu(const std::vector<float>& nval);
  void v_cbn_avg_gpu(std::vector<float>&& nval);

  void rolling_mean_gpu(const std::vector<float>& nval);
  void rolling_mean_gpu(std::vector<float>&& nval);
  void rolling_variance_gpu(const std::vector<float>& nval);
  void rolling_variance_gpu(std::vector<float>&& nval);

  void variance_delta_gpu(const std::vector<float>& nval);
  void variance_delta_gpu(std::vector<float>&& nval);
  void mean_delta_gpu(const std::vector<float>& nval);
  void mean_delta_gpu(std::vector<float>&& nval);

  void col_image_gpu(const std::vector<float>& nval);
  void col_image_gpu(std::vector<float>&& nval);

  void x_gpu(const std::vector<float>& nval);
  void x_gpu(std::vector<float>&& nval);
  void x_norm_gpu(const std::vector<float>& nval);
  void x_norm_gpu(std::vector<float>&& nval);
  void weights_gpu(const std::vector<float>& nval);
  void weights_gpu(std::vector<float>&& nval);
  void weight_updates_gpu(const std::vector<float>& nval);
  void weight_updates_gpu(std::vector<float>&& nval);
  void weight_deform_gpu(const std::vector<float>& nval);
  void weight_deform_gpu(std::vector<float>&& nval);
  void weight_change_gpu(const std::vector<float>& nval);
  void weight_change_gpu(std::vector<float>&& nval);

  void weights_gpu16(const std::vector<float>& nval);
  void weights_gpu16(std::vector<float>&& nval);
  void weight_updates_gpu16(const std::vector<float>& nval);
  void weight_updates_gpu16(std::vector<float>&& nval);

  void biases_gpu(const std::vector<float>& nval);
  void biases_gpu(std::vector<float>&& nval);
  void bias_updates_gpu(const std::vector<float>& nval);
  void bias_updates_gpu(std::vector<float>&& nval);
  void bias_change_gpu(const std::vector<float>& nval);
  void bias_change_gpu(std::vector<float>&& nval);

  void scales_gpu(const std::vector<float>& nval);
  void scales_gpu(std::vector<float>&& nval);
  void scale_updates_gpu(const std::vector<float>& nval);
  void scale_updates_gpu(std::vector<float>&& nval);
  void scale_change_gpu(const std::vector<float>& nval);
  void scale_change_gpu(std::vector<float>&& nval);

  void input_antialiasing_gpu(const std::vector<float>& nval);
  void input_antialiasing_gpu(std::vector<float>&& nval);
  void output_gpu(const std::vector<float>& nval);
  void output_gpu(std::vector<float>&& nval);
  void output_avg_gpu(const std::vector<float>& nval);
  void output_avg_gpu(std::vector<float>&& nval);
  void activation_input_gpu(const std::vector<float>& nval);
  void activation_input_gpu(std::vector<float>&& nval);
  void loss_gpu(const std::vector<float>& nval);
  void loss_gpu(std::vector<float>&& nval);
  void delta_gpu(const std::vector<float>& nval);
  void delta_gpu(std::vector<float>&& nval);
  void cos_sim_gpu(const std::vector<float>& nval);
  void cos_sim_gpu(std::vector<float>&& nval);
  void rand_gpu(const std::vector<float>& nval);
  void rand_gpu(std::vector<float>&& nval);
  void drop_blocks_scale(const std::vector<float>& nval);
  void drop_blocks_scale(std::vector<float>&& nval);
  void drop_blocks_scale_gpu(const std::vector<float>& nval);
  void drop_blocks_scale_gpu(std::vector<float>&& nval);
  void squared_gpu(const std::vector<float>& nval);
  void squared_gpu(std::vector<float>&& nval);
  void norms_gpu(const std::vector<float>& nval);
  void norms_gpu(std::vector<float>&& nval);

  void gt_gpu(const std::vector<float>& nval);
  void gt_gpu(std::vector<float>&& nval);
  void a_avg_gpu(const std::vector<float>& nval);
  void a_avg_gpu(std::vector<float>&& nval);

  void input_sizes_gpu(const std::vector<int>& nval);
  void input_sizes_gpu(std::vector<int>&& nval);
  void layers_output_gpu(const vrfloats& nval);
  void layers_output_gpu(vrfloats&& nval);
  void layers_delta_gpu(const vrfloats& nval);
  void layers_delta_gpu(vrfloats&& nval);

 private:
  static layerptr create_layer();
  static layer* layer_of(const ALayerAdaptorPtr& adaptor);

  layerptr l;

  std::vector<float> vclasses_multipliers;
  std::vector<float> vembedding_output;
  std::vector<float> vrand;
  std::vector<float> vcost;
  std::vector<float> vcos_sim;
  std::vector<float> vexp_cos_sim;
  std::vector<float> vp_constrastive;
  std::vector<float> vstate;
  std::vector<float> vprev_state;
  std::vector<float> vforgot_state;
  std::vector<float> vforgot_delta;
  std::vector<float> vstate_delta;
  std::vector<float> vcombine_cpu;
  std::vector<float> vcombine_delta_cpu;
  std::vector<float> vconcat;
  std::vector<float> vconcat_delta;
  std::vector<float> vbinary_weights;
  std::vector<float> vbiases;
  std::vector<float> vbias_updates;
  std::vector<float> vscales;
  std::vector<float> vscale_updates;
  std::vector<float> vweights_ema;
  std::vector<float> vbiases_ema;
  std::vector<float> vscales_ema;
  std::vector<float> vweights;
  std::vector<float> vweight_updates;
  std::vector<float> vmean_arr_gpu;
  std::vector<float> valign_workspace_gpu;
  std::vector<float> vtransposed_align_workspace_gpu;
  std::vector<float> vmean_arr;
  std::vector<float> vcol_image;
  std::vector<float> vdelta;
  std::vector<float> voutput;
  std::vector<float> vactivation_input;
  std::vector<float> vloss;
  std::vector<float> vsquared;
  std::vector<float> vnorms;
  std::vector<float> vspatial_mean;
  std::vector<float> vmean;
  std::vector<float> vvariance;
  std::vector<float> vmean_delta;
  std::vector<float> vvariance_delta;
  std::vector<float> vrolling_mean;
  std::vector<float> vrolling_variance;
  std::vector<float> vx;
  std::vector<float> vx_norm;
  std::vector<float> vm;
  std::vector<float> vv;
  std::vector<float> vbias_m;
  std::vector<float> vbias_v;
  std::vector<float> vscale_m;
  std::vector<float> vscale_v;
  std::vector<float> vz_cpu;
  std::vector<float> vr_cpu;
  std::vector<float> vh_cpu;
  std::vector<float> vstored_h_cpu;
  std::vector<float> vprev_state_cpu;
  std::vector<float> vtemp_cpu;
  std::vector<float> vtemp2_cpu;
  std::vector<float> vtemp3_cpu;
  std::vector<float> vdh_cpu;
  std::vector<float> vhh_cpu;
  std::vector<float> vprev_cell_cpu;
  std::vector<float> vcell_cpu;
  std::vector<float> vf_cpu;
  std::vector<float> vi_cpu;
  std::vector<float> vg_cpu;
  std::vector<float> vo_cpu;
  std::vector<float> vc_cpu;
  std::vector<float> vstored_c_cpu;
  std::vector<float> vdc_cpu;
  std::vector<float> vbinary_input;
  std::vector<float> vz_gpu;
  std::vector<float> vr_gpu;
  std::vector<float> vh_gpu;
  std::vector<float> vstored_h_gpu;
  std::vector<float> vbottelneck_hi_gpu;
  std::vector<float> vbottelneck_delta_gpu;
  std::vector<float> vtemp_gpu;
  std::vector<float> vtemp2_gpu;
  std::vector<float> vtemp3_gpu;
  std::vector<float> vdh_gpu;
  std::vector<float> vhh_gpu;
  std::vector<float> vprev_cell_gpu;
  std::vector<float> vprev_state_gpu;
  std::vector<float> vlast_prev_state_gpu;
  std::vector<float> vlast_prev_cell_gpu;
  std::vector<float> vcell_gpu;
  std::vector<float> vf_gpu;
  std::vector<float> vi_gpu;
  std::vector<float> vg_gpu;
  std::vector<float> vo_gpu;
  std::vector<float> vc_gpu;
  std::vector<float> vstored_c_gpu;
  std::vector<float> vdc_gpu;
  std::vector<float> vm_gpu;
  std::vector<float> vv_gpu;
  std::vector<float> vbias_m_gpu;
  std::vector<float> vscale_m_gpu;
  std::vector<float> vbias_v_gpu;
  std::vector<float> vscale_v_gpu;
  std::vector<float> vcombine_gpu;
  std::vector<float> vcombine_delta_gpu;
  std::vector<float> vforgot_state_gpu;
  std::vector<float> vforgot_delta_gpu;
  std::vector<float> vstate_gpu;
  std::vector<float> vstate_delta_gpu;
  std::vector<float> vgate_gpu;
  std::vector<float> vgate_delta_gpu;
  std::vector<float> vsave_gpu;
  std::vector<float> vsave_delta_gpu;
  std::vector<float> vconcat_gpu;
  std::vector<float> vconcat_delta_gpu;
  std::vector<float> vbinary_input_gpu;
  std::vector<float> vbinary_weights_gpu;
  std::vector<float> vbin_conv_shortcut_in_gpu;
  std::vector<float> vbin_conv_shortcut_out_gpu;
  std::vector<float> vmean_gpu;
  std::vector<float> vvariance_gpu;
  std::vector<float> vm_cbn_avg_gpu;
  std::vector<float> vv_cbn_avg_gpu;
  std::vector<float> vrolling_mean_gpu;
  std::vector<float> vrolling_variance_gpu;
  std::vector<float> vvariance_delta_gpu;
  std::vector<float> vmean_delta_gpu;
  std::vector<float> vcol_image_gpu;
  std::vector<float> vx_gpu;
  std::vector<float> vx_norm_gpu;
  std::vector<float> vweights_gpu;
  std::vector<float> vweight_updates_gpu;
  std::vector<float> vweight_deform_gpu;
  std::vector<float> vweight_change_gpu;
  std::vector<float> vweights_gpu16;
  std::vector<float> vweight_updates_gpu16;
  std::vector<float> vbiases_gpu;
  std::vector<float> vbias_updates_gpu;
  std::vector<float> vbias_change_gpu;
  std::vector<float> vscales_gpu;
  std::vector<float> vscale_updates_gpu;
  std::vector<float> vscale_change_gpu;
  std::vector<float> vinput_antialiasing_gpu;
  std::vector<float> voutput_gpu;
  std::vector<float> voutput_avg_gpu;
  std::vector<float> vactivation_input_gpu;
  std::vector<float> vloss_gpu;
  std::vector<float> vdelta_gpu;
  std::vector<float> vcos_sim_gpu;
  std::vector<float> vrand_gpu;
  std::vector<float> vdrop_blocks_scale;
  std::vector<float> vdrop_blocks_scale_gpu;
  std::vector<float> vsquared_gpu;
  std::vector<float> vnorms_gpu;
  std::vector<float> vgt_gpu;
  std::vector<float> va_avg_gpu;

  std::vector<int> vmask;
  std::vector<int> vindexes;
  std::vector<int> vinput_layers;
  std::vector<int> vinput_sizes;
  std::vector<int> vmap;
  std::vector<int> vcounts;
  std::vector<int> vlabels;
  std::vector<int> vclass_ids;
  std::vector<int> vindexes_gpu;
  std::vector<int> vinput_sizes_gpu;

  std::vector<uint32_t> vbin_re_packed_input;

  ALayerAdaptorPtr mshare_layer;

  std::shared_ptr<tree> msoftmax_tree;

  ALayerAdaptorPtr minput_layer;
  ALayerAdaptorPtr mself_layer;
  ALayerAdaptorPtr moutput_layer;

  ALayerAdaptorPtr mreset_layer;
  ALayerAdaptorPtr mupdate_layer;
  ALayerAdaptorPtr mstate_layer;

  ALayerAdaptorPtr minput_gate_layer;
  ALayerAdaptorPtr mstate_gate_layer;
  ALayerAdaptorPtr minput_save_layer;
  ALayerAdaptorPtr mstate_save_layer;
  ALayerAdaptorPtr minput_state_layer;
  ALayerAdaptorPtr mstate_state_layer;

  ALayerAdaptorPtr minput_z_layer;
  ALayerAdaptorPtr mstate_z_layer;

  ALayerAdaptorPtr minput_r_layer;
  ALayerAdaptorPtr mstate_r_layer;

  ALayerAdaptorPtr minput_h_layer;
  ALayerAdaptorPtr mstate_h_layer;

  ALayerAdaptorPtr mwz;
  ALayerAdaptorPtr muz;
  ALayerAdaptorPtr mwr;
  ALayerAdaptorPtr mur;
  ALayerAdaptorPtr mwh;
  ALayerAdaptorPtr muh;
  ALayerAdaptorPtr muo;
  ALayerAdaptorPtr mwo;
  ALayerAdaptorPtr mvo;
  ALayerAdaptorPtr muf;
  ALayerAdaptorPtr mwf;
  ALayerAdaptorPtr mvf;
  ALayerAdaptorPtr mui;
  ALayerAdaptorPtr mwi;
  ALayerAdaptorPtr mvi;
  ALayerAdaptorPtr mug;
  ALayerAdaptorPtr mwg;

  std::string scweights;
  std::string salign_bit_weights_gpu;
  std::string salign_bit_weights;
  std::string st_bit_input;

  vvfloats vsums;
  vrfloats vrsums;

  vrfloats vrlayers_output;
  vrfloats vrlayers_delta;
  vrfloats vrlayers_output_gpu;
  vrfloats vrlayers_delta_gpu;

  vcontrastive_params vcontrast_p_gpu;
};

using ALayerAdaptorPtr = ALayerAdaptor::ALayerAdaptorPtr;
using layerptr = ALayerAdaptor::layerptr;

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_ALAYERADAPTOR_CLASS_H
