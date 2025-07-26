#include "src/detector/train_detector_ctx.h"

#include <assert.h>
#include <stdio.h>

#include "src/parser.h"
#include "src/utils.h"

#include "src/detector/calc_map_free_ctx.h"
#include "src/detector/calc_map_init_ctx.h"
#include "src/detector/read_options_ctx.h"
#include "src/detector/load_net_weights_ctx.h"
#include "src/detector/draw_train_loss_ctx.h"
#include "src/detector/traverse_detection_layers_ctx.h"
#include "src/detector/sync_gpu_nets_ctx.h"
#include "src/detector/train_network_ctx.h"

int train_detector_ctx(struct detector_context *ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    printf("Invalid context pointer provided for train_detector_ctx\n");
    return;
  }

  read_options_ctx(ctx);

  calc_map_init_ctx(ctx);

  srand(time(0));

  load_net_weights_ctx(ctx);

  srand(time(0));

  print_net_init_stats_ctx(ctx);

  traverse_detection_layers_ctx(ctx);

  ctx->plist = get_paths(ctx->train_images);
  ctx->train_images_num = ctx->plist->size;
  ctx->paths = (char **)list_to_array(ctx->plist);

  const int init_w = ctx->net.w;
  const int init_h = ctx->net.h;
  const int init_b = ctx->net.batch;

  prepare_load_args_ctx(ctx);

  draw_train_chart_ctx(ctx);
  
  // printf(" imgs = %d \n", imgs);

  pthread_t load_thread = load_data(ctx->args);

  int count = 0;

  // while(i*imgs < N*120){
  while (get_current_iteration(ctx->net) < ctx->net.max_batches) {
    if (ctx->l.random && count++ % 10 == 0) {
      float rand_coef = 1.4;
      if (ctx->l.random != 1.0) rand_coef = ctx->l.random;
      printf("Resizing, random_coef = %.2f \n", rand_coef);
      float random_val = rand_scale(rand_coef);  // *x or /x
      int dim_w =
          roundl(random_val * init_w / ctx->net.resize_step + 1) * ctx->net.resize_step;
      int dim_h =
          roundl(random_val * init_h / ctx->net.resize_step + 1) * ctx->net.resize_step;
      if (random_val < 1 && (dim_w > init_w || dim_h > init_h))
        dim_w = init_w, dim_h = init_h;

      int max_dim_w =
          roundl(rand_coef * init_w / ctx->net.resize_step + 1) * ctx->net.resize_step;
      int max_dim_h =
          roundl(rand_coef * init_h / ctx->net.resize_step + 1) * ctx->net.resize_step;

      // at the beginning (check if enough memory) and at the end (calc rolling
      // mean/variance)
      if (ctx->avg_loss < 0 || get_current_iteration(ctx->net) > ctx->net.max_batches - 100) {
        dim_w = max_dim_w;
        dim_h = max_dim_h;
      }

      if (dim_w < ctx->net.resize_step) dim_w = ctx->net.resize_step;
      if (dim_h < ctx->net.resize_step) dim_h = ctx->net.resize_step;
      int dim_b = (init_b * max_dim_w * max_dim_h) / (dim_w * dim_h);
      int new_dim_b = (int)(dim_b * 0.8);
      if (new_dim_b > init_b) dim_b = new_dim_b;

      ctx->args.w = dim_w;
      ctx->args.h = dim_h;

      int k;
      if (ctx->net.dynamic_minibatch) {
        for (k = 0; k < ctx->ngpus; ++k) {
          (*ctx->nets[k].seen) =
              init_b *ctx-> net.subdivisions *
              get_current_iteration(
                  ctx->net);  // remove this line, when you will save to weights-file
                         // both: seen & cur_iteration
          ctx->nets[k].batch = dim_b;
          int j;
          for (j = 0; j < ctx->nets[k].n; ++j) ctx->nets[k].layers[j].batch = dim_b;
        }
        ctx->net.batch = dim_b;
        ctx->imgs = ctx->net.batch * ctx->net.subdivisions * ctx->ngpus;
        ctx->args.n = ctx->imgs;
        printf("\n %d x %d  (batch = %d) \n", dim_w, dim_h, ctx->net.batch);
      } else
        printf("\n %d x %d \n", dim_w, dim_h);

      pthread_join(load_thread, 0);
      ctx->train = ctx->buffer;
      free_data(ctx->train);
      load_thread = load_data(ctx->args);

      for (k = 0; k < ctx->ngpus; ++k) {
        resize_network(ctx->nets + k, dim_w, dim_h);
      }
      ctx->net = ctx->nets[0];
    }
    ctx->time = what_time_is_it_now();

    pthread_join(load_thread, 0);

    ctx->train = ctx->buffer;
    if (ctx->net.track) {
      ctx->net.sequential_subdivisions = get_current_seq_subdivisions(ctx->net);
      ctx->args.threads = ctx->net.sequential_subdivisions * ctx->ngpus;
      printf(" sequential_subdivisions = %d, sequence = %d \n",
             ctx->net.sequential_subdivisions, get_sequence_value(ctx->net));
    }
    load_thread = load_data(ctx->args);
    // wait_key_cv(500);

    save_truth_image_ctx(ctx);

    const double load_time = (what_time_is_it_now() - ctx->time);
    printf("Loaded: %lf seconds", load_time);
    if (load_time > 0.1 && ctx->avg_loss > 0)
      printf(" - performance bottleneck on CPU or Disk HDD/SSD");
    printf("\n");

    ctx->time = what_time_is_it_now();
    ctx->loss = 0;

    train_network_ctx(ctx);

    if (ctx->avg_loss < 0 || ctx->avg_loss != ctx->avg_loss)
      ctx->avg_loss = ctx->loss;  // if(-inf or nan)
    ctx->avg_loss = ctx->avg_loss * .9 + ctx->loss * .1;

    print_net_stat_ctx(ctx);

    const int iteration = get_current_iteration(ctx->net);

    ctx->draw_precision = 0;
    if (ctx->calc_map &&
        (iteration >= ctx->next_map_calc || iteration == ctx->net.max_batches)) {
      if (ctx->l.random) {
        printf("Resizing to initial size: %d x %d ", init_w, init_h);
        ctx->args.w = init_w;
        ctx->args.h = init_h;
        int k;
        if (ctx->net.dynamic_minibatch) {
          for (k = 0; k < ctx->ngpus; ++k) {
            for (k = 0; k < ctx->ngpus; ++k) {
              ctx->nets[k].batch = init_b;
              int j;
              for (j = 0; j < ctx->nets[k].n; ++j) ctx->nets[k].layers[j].batch = init_b;
            }
          }
          ctx->net.batch = init_b;
          ctx->imgs = init_b * ctx->net.subdivisions * ctx->ngpus;
          ctx->args.n = ctx->imgs;
          printf("\n %d x %d  (batch = %d) \n", init_w, init_h, init_b);
        }
        pthread_join(load_thread, 0);
        free_data(ctx->train);
        ctx->train = ctx->buffer;
        load_thread = load_data(ctx->args);
        for (k = 0; k < ctx->ngpus; ++k) {
          resize_network(ctx->nets + k, init_w, init_h);
        }
        ctx->net = ctx->nets[0];
      }

      copy_weights_net(ctx->net, &ctx->net_map);

      // combine Training and Validation networks
      // network net_combined = combine_train_valid_networks(net, net_map);

      ctx->iter_map = iteration;
      ctx->mean_average_precision =
          validate_detector_map(ctx->datacfg, ctx->cfg, ctx->weights,
                                ctx->thresh, ctx->iou_thresh, 0, ctx->net.letter_box,
                                &ctx->net_map);  // &net_combined);
      printf("\n mean_average_precision (mAP@%0.2f) = %f \n", ctx->iou_thresh,
             ctx->mean_average_precision);
      if (ctx->mean_average_precision >= ctx->best_map) {
        ctx->best_map = ctx->mean_average_precision;
        printf("New best mAP!\n");
        char buff[256];
        sprintf(buff, "%s/%s_best.weights", ctx->backup_directory, ctx->base);
        save_weights(ctx->net, buff);
      }

      ctx->draw_precision = 1;
    }
    ctx->time_remaining = ((ctx->net.max_batches - iteration) / ctx->ngpus) *
                     (what_time_is_it_now() - ctx->time + load_time) / 60 / 60;
    // set initial value, even if resume training from 10000 iteration
    if (ctx->avg_time < 0) ctx->avg_time = ctx->time_remaining;
    else
      ctx->avg_time = ctx->alpha_time * ctx->time_remaining + (1 - ctx->alpha_time) * ctx->avg_time;

    draw_train_loss_ctx(ctx);

    if ((iteration >= (ctx->iter_save + ctx->save_after_iterations) ||
         iteration % ctx->save_after_iterations == 0)) {
      ctx->iter_save = iteration;
      sync_gpu_nets_ctx(ctx);
      char buff[256];
      sprintf(buff, "%s/%s_%d.weights", ctx->backup_directory, ctx->base, iteration);
      save_weights(ctx->net, buff);
    }

    if ((ctx->save_after_iterations > ctx->save_last_weights_after) &&
        (iteration >= (ctx->iter_save_last + ctx->save_last_weights_after) ||
         (iteration % ctx->save_last_weights_after == 0 && iteration > 1))) {
      ctx->iter_save_last = iteration;
      sync_gpu_nets_ctx(ctx);
      char buff[256];
      sprintf(buff, "%s/%s_last.weights", ctx->backup_directory, ctx->base);
      save_weights(ctx->net, buff);

      if (ctx->net.ema_alpha && is_ema_initialized(ctx->net)) {
        sprintf(buff, "%s/%s_ema.weights", ctx->backup_directory, ctx->base);
        save_weights_upto(ctx->net, buff, ctx->net.n, 1);
        printf(" EMA weights are saved to the file: %s \n", buff);
      }
    }
    free_data(ctx->train);
  }

  sync_gpu_nets_ctx(ctx);

  char buff[256];
  sprintf(buff, "%s/%s_final.weights", ctx->backup_directory, ctx->base);
  save_weights(ctx->net, buff);
  printf(
      "If you want to train from the beginning, then use flag in the end of "
      "training command: -clear \n");

  pthread_join(load_thread, 0);

  clear_context(ctx);

  return 1;
}