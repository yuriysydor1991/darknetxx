#ifndef DARKNET_NETWORK_STRUCTURE_H
#define DARKNET_NETWORK_STRUCTURE_H

#include "src/enums/enums.h"
#include "src/layers/layer.h"
#include "src/structs/tree.h"

#ifdef __cplusplus
extern "C" {
#endif

struct network;
typedef struct network network;

// network.h
typedef struct network
{
  int n;
  int batch;
  uint64_t *seen;
  float *badlabels_reject_threshold;
  float *delta_rolling_max;
  float *delta_rolling_avg;
  float *delta_rolling_std;
  int weights_reject_freq;
  int equidistant_point;
  float badlabels_rejection_percentage;
  float num_sigmas_reject_badlabels;
  float ema_alpha;
  int *cur_iteration;
  float loss_scale;
  int *t;
  float epoch;
  int subdivisions;
  layer *layers;
  float *output;
  learning_rate_policy policy;
  int benchmark_layers;
  int *total_bbox;
  int *rewritten_bbox;

  float learning_rate;
  float learning_rate_min;
  float learning_rate_max;
  int batches_per_cycle;
  int batches_cycle_mult;
  float momentum;
  float decay;
  float gamma;
  float scale;
  float power;
  int time_steps;
  int step;
  int max_batches;
  int num_boxes;
  int train_images_num;
  float *seq_scales;
  float *scales;
  int *steps;
  int num_steps;
  int burn_in;
  int cudnn_half;

  int adam;
  float B1;
  float B2;
  float eps;

  int inputs;
  int outputs;
  int truths;
  int notruth;
  int h, w, c;
  int max_crop;
  int min_crop;
  float max_ratio;
  float min_ratio;
  int center;
  int flip;  // horizontal flip 50% probability augmentaiont for classifier
             // training (default = 1)
  int gaussian_noise;
  int blur;
  int mixup;
  float label_smooth_eps;
  int resize_step;
  int attention;
  int adversarial;
  float adversarial_lr;
  float max_chart_loss;
  int letter_box;
  int mosaic_bound;
  int contrastive;
  int contrastive_jit_flip;
  int contrastive_color;
  int unsupervised;
  float angle;
  float aspect;
  float exposure;
  float saturation;
  float hue;
  int random;
  int track;
  int augment_speed;
  int sequential_subdivisions;
  int init_sequential_subdivisions;
  int current_subdivision;
  int try_fix_nan;

  int gpu_index;
  tree *hierarchy;

  float *input;
  float *truth;
  float *delta;
  float *workspace;
  int train;
  int index;
  float *cost;
  float clip;

  // #ifdef GPU
  //  float *input_gpu;
  //  float *truth_gpu;
  float *delta_gpu;
  float *output_gpu;

  float *input_state_gpu;
  float *input_pinned_cpu;
  int input_pinned_cpu_flag;

  float **input_gpu;
  float **truth_gpu;
  float **input16_gpu;
  float **output16_gpu;
  size_t *max_input16_size;
  size_t *max_output16_size;
  int wait_stream;

  void *cuda_graph;
  void *cuda_graph_exec;
  int use_cuda_graph;
  int *cuda_graph_ready;

  float *global_delta_gpu;
  float *state_delta_gpu;
  size_t max_delta_gpu_size;
  // #endif  // GPU
  int optimized_memory;
  int dynamic_minibatch;
  size_t workspace_size_limit;

  /**
   * @brief Temporary fields for extended network params
   * previously stored in the obj.data file so now
   * only one cfg file is required.
   */
  char *names;
  char *lastweights;
  char *train_images;
  char *valid_images;
  char *backup_directory;
  int saveweights;
  int savelast;
  int classes;
} network;

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_NETWORK_STRUCTURE_H
