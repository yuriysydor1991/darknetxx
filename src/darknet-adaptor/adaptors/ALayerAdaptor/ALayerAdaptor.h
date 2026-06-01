#ifndef DARKNET_CXX_PORT_PROJECT_ALAYERADAPTOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ALAYERADAPTOR_CLASS_H

#include <memory>
#include <vector>

#include "src/darknet-adaptor/adaptors/IDarknetAdaptor.h"
#include "src/darknet-adaptor/adaptors/StructAdaptorHelper.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The original darknet adaptor for a layer structure.
 */
class ALayerAdaptor : virtual public IDarknetAdaptor,
                      virtual public StructAdaptorHelper
{
 public:
  using ALayerAdaptorPtr = std::shared_ptr<ALayerAdaptor>;

  virtual ~ALayerAdaptor() = default;
  ALayerAdaptor();
  ALayerAdaptor(const ALayerAdaptor&) = delete;
  ALayerAdaptor(ALayerAdaptor&&) = delete;

  ALayerAdaptorPtr create();

  virtual bool perform(DarknetContextPtr dctx) override;

  layerptr get();

  LAYER_TYPE type();
  ACTIVATION activation();
  ACTIVATION lstm_activation();
  COST_TYPE cost_type();

  //  void (*forward)(struct layer, struct network_state);
  //  void (*backward)(struct layer, struct network_state);
  //  void (*update)(struct layer, int, float, float, float);
  //  void (*forward_gpu)(struct layer, struct network_state);
  //  void (*backward_gpu)(struct layer, struct network_state);
  //  void (*update_gpu)(struct layer, int, float, float, float, float);

  layerptr share_layer();
  int train();
  int avgpool();
  int batch_normalize();
  int shortcut();
  int batch();
  int dynamic_minibatch();
  int forced();
  int flipped();
  int inputs();
  int outputs();
  float mean_alpha();
  int nweights();
  int nbiases();
  int extra();
  int truths();
  int h();
  int w();
  int c();
  int out_h();
  int out_w();
  int out_c();
  int n();
  int max_boxes();
  int truth_size();
  int groups();
  int group_id();
  int size();
  int side();
  int stride();
  int stride_x();
  int stride_y();
  int dilation();
  int antialiasing();
  int maxpool_depth();
  int maxpool_zero_nonmax();
  int out_channels();
  float reverse();
  int coordconv();
  int flatten();
  int spatial();
  int pad();
  int sqrt();
  int flip();
  int index();
  int scale_wh();
  int binary();
  int xnor();
  int peephole();
  int use_bin_output();
  int keep_delta_gpu();
  int optimized_memory();
  int steps();
  int history_size();
  int bottleneck();
  float time_normalizer();
  int state_constrain();
  int hidden();
  int truth();
  float smooth();
  float dot();
  int deform();
  int grad_centr();
  int sway();
  int rotate();
  int stretch();
  int stretch_sway();
  float angle();
  float jitter();
  float resize();
  float saturation();
  float exposure();
  float shift();
  float ratio();
  float learning_rate_scale();
  float clip();
  int focal_loss();
  std::vector<float>& classes_multipliers();
  float label_smooth_eps();
  int noloss();
  int softmax();
  int classes();
  int detection();
  int embedding_layer_id();
  std::vector<float>& embedding_output();
  int embedding_size();
  float sim_thresh();
  int track_history_size();
  int dets_for_track();
  int dets_for_show();
  float track_ciou_norm();
  int coords();
  int background();
  int rescore();
  int objectness();
  int does_cost();
  int joint();
  int noadjust();
  int reorg();
  int log();
  int tanh();
  std::vector<int>& mask();
  int total();
  float bflops();

  int adam();
  float B1();
  float B2();
  float eps();

  int t();

  float alpha();
  float beta();
  float kappa();

  float coord_scale();
  float object_scale();
  float noobject_scale();
  float mask_scale();
  float class_scale();
  int bias_match();
  float random();
  float ignore_thresh();
  float truth_thresh();
  float iou_thresh();
  float thresh();
  float focus();
  int classfix();
  int absolute();
  int assisted_excitation();

  int onlyforward();
  int stopbackward();
  int train_only_bn();
  int dont_update();
  int burnin_update();
  int dontload();
  int dontsave();
  int dontloadscales();
  int numload();

  float temperature();
  float probability();
  float dropblock_size_rel();
  int dropblock_size_abs();
  int dropblock();
  float scale();

  int receptive_w();
  int receptive_h();
  int receptive_w_scale();
  int receptive_h_scale();

  std::string cweights();
  std::vector<int>& indexes();
  std::vector<int>& input_layers();
  std::vector<int>& input_sizes();
  vvfloats layers_output();
  vvfloats layers_delta();
  WEIGHTS_TYPE_T weights_type();
  WEIGHTS_NORMALIZATION_T weights_normalization();
  std::vector<int>& map();
  std::vector<int>& counts();
  vvfloats sums();
  std::vector<float>& rand();
  std::vector<float>& cost();
  std::vector<int>& labels();
  std::vector<int>& class_ids();
  int contrastive_neg_max();
  std::vector<float>& cos_sim();
  std::vector<float>& exp_cos_sim();
  std::vector<float>& p_constrastive();
  vcontrastive_params contrast_p_gpu();
  std::vector<float>& state();
  std::vector<float>& prev_state();
  std::vector<float>& forgot_state();
  std::vector<float>& forgot_delta();
  std::vector<float>& state_delta();
  std::vector<float>& combine_cpu();
  std::vector<float>& combine_delta_cpu();

  std::vector<float>& concat();
  std::vector<float>& concat_delta();

  std::vector<float>& binary_weights();

  std::vector<float>& biases();
  std::vector<float>& bias_updates();

  std::vector<float>& scales();
  std::vector<float>& scale_updates();

  std::vector<float>& weights_ema();
  std::vector<float>& biases_ema();
  std::vector<float>& scales_ema();

  std::vector<float>& weights();
  std::vector<float>& weight_updates();

  float scale_x_y();
  int objectness_smooth();
  int new_coords();
  int show_details();
  float max_delta();
  float uc_normalizer();
  float iou_normalizer();
  float obj_normalizer();
  float cls_normalizer();
  float delta_normalizer();
  IOU_LOSS iou_loss();
  IOU_LOSS iou_thresh_kind();
  NMS_KIND nms_kind();
  float beta_nms();
  YOLO_POINT yolo_point();

  std::string align_bit_weights_gpu();
  std::vector<float>& mean_arr_gpu();
  std::vector<float>& align_workspace_gpu();
  std::vector<float>& transposed_align_workspace_gpu();
  int align_workspace_size();

  std::string align_bit_weights();
  std::vector<float>& mean_arr();
  int align_bit_weights_size();
  int lda_align();
  int new_lda();
  int bit_align();

  std::vector<float>& col_image();
  std::vector<float>& delta();
  std::vector<float>& output();
  std::vector<float>& activation_input();
  int delta_pinned();
  int output_pinned();
  std::vector<float>& loss();
  std::vector<float>& squared();
  std::vector<float>& norms();

  std::vector<float>& spatial_mean();
  std::vector<float>& mean();
  std::vector<float>& variance();

  std::vector<float>& mean_delta();
  std::vector<float>& variance_delta();

  std::vector<float>& rolling_mean();
  std::vector<float>& rolling_variance();

  std::vector<float>& x();
  std::vector<float>& x_norm();

  std::vector<float>& m();
  std::vector<float>& v();

  std::vector<float>& bias_m();
  std::vector<float>& bias_v();
  std::vector<float>& scale_m();
  std::vector<float>& scale_v();

  std::vector<float>& z_cpu();
  std::vector<float>& r_cpu();
  std::vector<float>& h_cpu();
  std::vector<float>& stored_h_cpu();
  std::vector<float>& prev_state_cpu();

  std::vector<float>& temp_cpu();
  std::vector<float>& temp2_cpu();
  std::vector<float>& temp3_cpu();

  std::vector<float>& dh_cpu();
  std::vector<float>& hh_cpu();
  std::vector<float>& prev_cell_cpu();
  std::vector<float>& cell_cpu();
  std::vector<float>& f_cpu();
  std::vector<float>& i_cpu();
  std::vector<float>& g_cpu();
  std::vector<float>& o_cpu();
  std::vector<float>& c_cpu();
  std::vector<float>& stored_c_cpu();
  std::vector<float>& dc_cpu();

  std::vector<float>& binary_input();
  std::vector<uint32_t> bin_re_packed_input();
  std::string t_bit_input();

  layerptr input_layer();
  layerptr self_layer();
  layerptr output_layer();

  layerptr reset_layer();
  layerptr update_layer();
  layerptr state_layer();

  layerptr input_gate_layer();
  layerptr state_gate_layer();
  layerptr input_save_layer();
  layerptr state_save_layer();
  layerptr input_state_layer();
  layerptr state_state_layer();

  layerptr input_z_layer();
  layerptr state_z_layer();

  layerptr input_r_layer();
  layerptr state_r_layer();

  layerptr input_h_layer();
  layerptr state_h_layer();

  layerptr wz();
  layerptr uz();
  layerptr wr();
  layerptr ur();
  layerptr wh();
  layerptr uh();
  layerptr uo();
  layerptr wo();
  layerptr vo();
  layerptr uf();
  layerptr wf();
  layerptr vf();
  layerptr ui();
  layerptr wi();
  layerptr vi();
  layerptr ug();
  layerptr wg();

  tree* softmax_tree();

  size_t workspace_size();

  // #ifdef GPU
  std::vector<int>& indexes_gpu();

  int stream();
  int wait_stream_id();

  std::vector<float>& z_gpu();
  std::vector<float>& r_gpu();
  std::vector<float>& h_gpu();
  std::vector<float>& stored_h_gpu();
  std::vector<float>& bottelneck_hi_gpu();
  std::vector<float>& bottelneck_delta_gpu();

  std::vector<float>& temp_gpu();
  std::vector<float>& temp2_gpu();
  std::vector<float>& temp3_gpu();

  std::vector<float>& dh_gpu();
  std::vector<float>& hh_gpu();
  std::vector<float>& prev_cell_gpu();
  std::vector<float>& prev_state_gpu();
  std::vector<float>& last_prev_state_gpu();
  std::vector<float>& last_prev_cell_gpu();
  std::vector<float>& cell_gpu();
  std::vector<float>& f_gpu();
  std::vector<float>& i_gpu();
  std::vector<float>& g_gpu();
  std::vector<float>& o_gpu();
  std::vector<float>& c_gpu();
  std::vector<float>& stored_c_gpu();
  std::vector<float>& dc_gpu();

  // adam
  std::vector<float>& m_gpu();
  std::vector<float>& v_gpu();
  std::vector<float>& bias_m_gpu();
  std::vector<float>& scale_m_gpu();
  std::vector<float>& bias_v_gpu();
  std::vector<float>& scale_v_gpu();

  std::vector<float>& combine_gpu();
  std::vector<float>& combine_delta_gpu();

  std::vector<float>& forgot_state_gpu();
  std::vector<float>& forgot_delta_gpu();
  std::vector<float>& state_gpu();
  std::vector<float>& state_delta_gpu();
  std::vector<float>& gate_gpu();
  std::vector<float>& gate_delta_gpu();
  std::vector<float>& save_gpu();
  std::vector<float>& save_delta_gpu();
  std::vector<float>& concat_gpu();
  std::vector<float>& concat_delta_gpu();

  std::vector<float>& binary_input_gpu();
  std::vector<float>& binary_weights_gpu();
  std::vector<float>& bin_conv_shortcut_in_gpu();
  std::vector<float>& bin_conv_shortcut_out_gpu();

  std::vector<float>& mean_gpu();
  std::vector<float>& variance_gpu();
  std::vector<float>& m_cbn_avg_gpu();
  std::vector<float>& v_cbn_avg_gpu();

  std::vector<float>& rolling_mean_gpu();
  std::vector<float>& rolling_variance_gpu();

  std::vector<float>& variance_delta_gpu();
  std::vector<float>& mean_delta_gpu();

  std::vector<float>& col_image_gpu();

  std::vector<float>& x_gpu();
  std::vector<float>& x_norm_gpu();
  std::vector<float>& weights_gpu();
  std::vector<float>& weight_updates_gpu();
  std::vector<float>& weight_deform_gpu();
  std::vector<float>& weight_change_gpu();

  std::vector<float>& weights_gpu16();
  std::vector<float>& weight_updates_gpu16();

  std::vector<float>& biases_gpu();
  std::vector<float>& bias_updates_gpu();
  std::vector<float>& bias_change_gpu();

  std::vector<float>& scales_gpu();
  std::vector<float>& scale_updates_gpu();
  std::vector<float>& scale_change_gpu();

  std::vector<float>& input_antialiasing_gpu();
  std::vector<float>& output_gpu();
  std::vector<float>& output_avg_gpu();
  std::vector<float>& activation_input_gpu();
  std::vector<float>& loss_gpu();
  std::vector<float>& delta_gpu();
  std::vector<float>& cos_sim_gpu();
  std::vector<float>& rand_gpu();
  std::vector<float>& drop_blocks_scale();
  std::vector<float>& drop_blocks_scale_gpu();
  std::vector<float>& squared_gpu();
  std::vector<float>& norms_gpu();

  std::vector<float>& gt_gpu();
  std::vector<float>& a_avg_gpu();

  std::vector<int>& input_sizes_gpu();
  vvfloats layers_output_gpu();
  vvfloats layers_delta_gpu();

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

  void* srcTensorDesc();
  void* dstTensorDesc();
  void* srcTensorDesc16();
  void* dstTensorDesc16();
  void* dsrcTensorDesc();
  void* ddstTensorDesc();
  void* dsrcTensorDesc16();
  void* ddstTensorDesc16();
  void* normTensorDesc();
  void* normDstTensorDesc();
  void* normDstTensorDescF16();
  void* weightDesc();
  void* weightDesc16();
  void* dweightDesc();
  void* dweightDesc16();
  void* convDesc();

  UNUSED_ENUM_TYPE fw_algo();
  UNUSED_ENUM_TYPE fw_algo16();
  UNUSED_ENUM_TYPE bd_algo();
  UNUSED_ENUM_TYPE bd_algo16();
  UNUSED_ENUM_TYPE bf_algo();
  UNUSED_ENUM_TYPE bf_algo16();

  void* poolingDesc();
#endif  // CUDNN

  /**
   * Setters.
   */

  void type(const LAYER_TYPE& nval);
  void activation(const ACTIVATION& nval);
  void lstm_activation(const ACTIVATION& nval);
  void cost_type(const COST_TYPE& nval);

  void share_layer(const layerptr& nval);
  void share_layer(layerptr&& nval);
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
  void layers_output(const vvfloats& nval);
  void layers_delta(const vvfloats& nval);
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

  void input_layer(const layerptr& nval);
  void input_layer(layerptr&& nval);
  void self_layer(const layerptr& nval);
  void self_layer(layerptr&& nval);
  void output_layer(const layerptr& nval);
  void output_layer(layerptr&& nval);

  void reset_layer(const layerptr& nval);
  void reset_layer(layerptr&& nval);
  void update_layer(const layerptr& nval);
  void update_layer(layerptr&& nval);
  void state_layer(const layerptr& nval);
  void state_layer(layerptr&& nval);

  void input_gate_layer(const layerptr& nval);
  void input_gate_layer(layerptr&& nval);
  void state_gate_layer(const layerptr& nval);
  void state_gate_layer(layerptr&& nval);
  void input_save_layer(const layerptr& nval);
  void input_save_layer(layerptr&& nval);
  void state_save_layer(const layerptr& nval);
  void state_save_layer(layerptr&& nval);
  void input_state_layer(const layerptr& nval);
  void input_state_layer(layerptr&& nval);
  void state_state_layer(const layerptr& nval);
  void state_state_layer(layerptr&& nval);

  void input_z_layer(const layerptr& nval);
  void input_z_layer(layerptr&& nval);
  void state_z_layer(const layerptr& nval);
  void state_z_layer(layerptr&& nval);

  void input_r_layer(const layerptr& nval);
  void input_r_layer(layerptr&& nval);
  void state_r_layer(const layerptr& nval);
  void state_r_layer(layerptr&& nval);

  void input_h_layer(const layerptr& nval);
  void input_h_layer(layerptr&& nval);
  void state_h_layer(const layerptr& nval);
  void state_h_layer(layerptr&& nval);

  void wz(const layerptr& nval);
  void wz(layerptr&& nval);
  void uz(const layerptr& nval);
  void uz(layerptr&& nval);
  void wr(const layerptr& nval);
  void wr(layerptr&& nval);
  void ur(const layerptr& nval);
  void ur(layerptr&& nval);
  void wh(const layerptr& nval);
  void wh(layerptr&& nval);
  void uh(const layerptr& nval);
  void uh(layerptr&& nval);
  void uo(const layerptr& nval);
  void uo(layerptr&& nval);
  void wo(const layerptr& nval);
  void wo(layerptr&& nval);
  void vo(const layerptr& nval);
  void vo(layerptr&& nval);
  void uf(const layerptr& nval);
  void uf(layerptr&& nval);
  void wf(const layerptr& nval);
  void wf(layerptr&& nval);
  void vf(const layerptr& nval);
  void vf(layerptr&& nval);
  void ui(const layerptr& nval);
  void ui(layerptr&& nval);
  void wi(const layerptr& nval);
  void wi(layerptr&& nval);
  void vi(const layerptr& nval);
  void vi(layerptr&& nval);
  void ug(const layerptr& nval);
  void ug(layerptr&& nval);
  void wg(const layerptr& nval);
  void wg(layerptr&& nval);

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
  void layers_output_gpu(const vvfloats& nval);
  void layers_delta_gpu(const vvfloats& nval);

 private:
  static layerptr create_layer();

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

  layerptr mshare_layer;

  layerptr minput_layer;
  layerptr mself_layer;
  layerptr moutput_layer;

  layerptr mreset_layer;
  layerptr mupdate_layer;
  layerptr mstate_layer;

  layerptr minput_gate_layer;
  layerptr mstate_gate_layer;
  layerptr minput_save_layer;
  layerptr mstate_save_layer;
  layerptr minput_state_layer;
  layerptr mstate_state_layer;

  layerptr minput_z_layer;
  layerptr mstate_z_layer;

  layerptr minput_r_layer;
  layerptr mstate_r_layer;

  layerptr minput_h_layer;
  layerptr mstate_h_layer;

  layerptr mwz;
  layerptr muz;
  layerptr mwr;
  layerptr mur;
  layerptr mwh;
  layerptr muh;
  layerptr muo;
  layerptr mwo;
  layerptr mvo;
  layerptr muf;
  layerptr mwf;
  layerptr mvf;
  layerptr mui;
  layerptr mwi;
  layerptr mvi;
  layerptr mug;
  layerptr mwg;

  std::string scweights;
  std::string salign_bit_weights_gpu;
  std::string salign_bit_weights;
  std::string st_bit_input;

  vvfloats vlayers_output;
  vvfloats vlayers_delta;
  vvfloats vsums;
  vvfloats vlayers_output_gpu;
  vvfloats vlayers_delta_gpu;

  std::vector<float*> vrlayers_output;
  std::vector<float*> vrlayers_delta;
  std::vector<float*> vrsums;
  std::vector<float*> vrlayers_output_gpu;
  std::vector<float*> vrlayers_delta_gpu;

  vcontrastive_params scontrastive_params;
  vcontrastive_params scontrast_p_gpu;
};

using ALayerAdaptorPtr = ALayerAdaptor::ALayerAdaptorPtr;
using layerptr = ALayerAdaptor::layerptr;

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_ALAYERADAPTOR_CLASS_H
