#include "src/detector/train_detector_ctx.h"

#include <assert.h>
#include <stdio.h>

#include "src/parser.h"
#include "src/utils.h"

#include "src/detector/calc_map_free_ctx.h"
#include "src/detector/calc_map_init_ctx.h"
#include "src/detector/read_options_ctx.h"

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
  char *base = basecfg(ctx->cfg);
  printf("%s\n", base);
  float avg_loss = -1;
  float avg_contrastive_acc = 0;
  network *nets = (network *)xcalloc(ctx->ngpus, sizeof(network));

  srand(time(0));
  int seed = rand();
  int k;
  for (k = 0; k < ctx->ngpus; ++k) {
    srand(seed);
#ifdef GPU
    cuda_set_device(gpus[k]);
#endif
    nets[k] = parse_network_cfg(ctx->cfg);
    nets[k].benchmark_layers = ctx->benchmark_layers;
    if (ctx->weights) {
      load_weights(&nets[k], ctx->weights);
    }
    if (ctx->clear) {
      *nets[k].seen = 0;
      *nets[k].cur_iteration = 0;
    }
    nets[k].learning_rate *= ctx->ngpus;
  }
  srand(time(0));
  network net = nets[0];

  const int actual_batch_size = net.batch * net.subdivisions;
  if (actual_batch_size == 1) {
    error(
        "Error: You set incorrect value batch=1 for Training! You should set "
        "batch=64 subdivision=64",
        DARKNET_LOC);
  } else if (actual_batch_size < 8) {
    printf(
        "\n Warning: You set batch=%d lower than 64! It is recommended to set "
        "batch=64 subdivision=64 \n",
        actual_batch_size);
  }

  int save_after_iterations = option_find_int(
      ctx->options, "saveweights",
      (net.max_batches < 10000) ? 1000
                                : 10000);  // configure when to write weights.
                                           // Very useful for smaller datasets!
  int save_last_weights_after = option_find_int(ctx->options, "savelast", 100);
  printf(
      "Weights are saved after: %d iterations. Last weights (*_last.weight) "
      "are stored every %d iterations. \n",
      save_after_iterations, save_last_weights_after);

  int imgs = net.batch * net.subdivisions * ctx->ngpus;
  printf("Learning Rate: %g, Momentum: %g, Decay: %g\n", net.learning_rate,
         net.momentum, net.decay);
  data train, buffer;

  layer l = net.layers[net.n - 1];
  for (k = 0; k < net.n; ++k) {
    layer lk = net.layers[k];
    if (lk.type == YOLO || lk.type == GAUSSIAN_YOLO || lk.type == REGION) {
      l = lk;
      printf(" Detection layer: %d - type = %d \n", k, l.type);
    }
  }

  int classes = l.classes;

  list *plist = get_paths(ctx->train_images);
  int train_images_num = plist->size;
  char **paths = (char **)list_to_array(plist);

  const int init_w = net.w;
  const int init_h = net.h;
  const int init_b = net.batch;
  int iter_save, iter_save_last, iter_map;
  iter_save = get_current_iteration(net);
  iter_save_last = get_current_iteration(net);
  iter_map = get_current_iteration(net);
  float mean_average_precision = -1;
  float best_map = mean_average_precision;

  load_args args = {0};
  args.w = net.w;
  args.h = net.h;
  args.c = net.c;
  args.paths = paths;
  args.n = imgs;
  args.m = plist->size;
  args.classes = classes;
  args.flip = net.flip;
  args.jitter = l.jitter;
  args.resize = l.resize;
  args.num_boxes = l.max_boxes;
  args.truth_size = l.truth_size;
  net.num_boxes = args.num_boxes;
  net.train_images_num = train_images_num;
  args.d = &buffer;
  args.type = DETECTION_DATA;
  args.threads = 64;  // 16 or 64

  args.angle = net.angle;
  args.gaussian_noise = net.gaussian_noise;
  args.blur = net.blur;
  args.mixup = net.mixup;
  args.exposure = net.exposure;
  args.saturation = net.saturation;
  args.hue = net.hue;
  args.letter_box = net.letter_box;
  args.mosaic_bound = net.mosaic_bound;
  args.contrastive = net.contrastive;
  args.contrastive_jit_flip = net.contrastive_jit_flip;
  args.contrastive_color = net.contrastive_color;
  if (ctx->dont_show && ctx->show_imgs) ctx->show_imgs = 2;
  args.show_imgs = ctx->show_imgs;

#ifdef OPENCV
  // int num_threads = get_num_threads();
  // if(num_threads > 2) args.threads = get_num_threads() - 2;
  args.threads = 6 * ctx->ngpus;  // 3 for - Amazon EC2 Tesla V100: p3.2xlarge
                                  // (8 logical cores) - p3.16xlarge
  // args.threads = 12 * ngpus;    // Ryzen 7 2700X (16 logical cores)
  mat_cv *img = NULL;
  float max_img_loss = net.max_chart_loss;
  int number_of_lines = 100;
  int img_size = 1000;
  char windows_name[100];
  sprintf(windows_name, "chart_%s.png", base);
  img = draw_train_chart(windows_name, max_img_loss, net.max_batches,
                         number_of_lines, img_size, ctx->dont_show,
                         ctx->chart_path);
#endif  // OPENCV
  if (net.contrastive && args.threads > net.batch / 2)
    args.threads = net.batch / 2;
  if (net.track) {
    args.track = net.track;
    args.augment_speed = net.augment_speed;
    if (net.sequential_subdivisions)
      args.threads = net.sequential_subdivisions * ctx->ngpus;
    else
      args.threads = net.subdivisions * ctx->ngpus;
    args.mini_batch = net.batch / net.time_steps;
    printf(
        "\n Tracking! batch = %d, subdiv = %d, time_steps = %d, mini_batch = "
        "%d \n",
        net.batch, net.subdivisions, net.time_steps, args.mini_batch);
  }
  // printf(" imgs = %d \n", imgs);

  pthread_t load_thread = load_data(args);

  int count = 0;
  double time_remaining, avg_time = -1, alpha_time = 0.01;

  // while(i*imgs < N*120){
  while (get_current_iteration(net) < net.max_batches) {
    if (l.random && count++ % 10 == 0) {
      float rand_coef = 1.4;
      if (l.random != 1.0) rand_coef = l.random;
      printf("Resizing, random_coef = %.2f \n", rand_coef);
      float random_val = rand_scale(rand_coef);  // *x or /x
      int dim_w =
          roundl(random_val * init_w / net.resize_step + 1) * net.resize_step;
      int dim_h =
          roundl(random_val * init_h / net.resize_step + 1) * net.resize_step;
      if (random_val < 1 && (dim_w > init_w || dim_h > init_h))
        dim_w = init_w, dim_h = init_h;

      int max_dim_w =
          roundl(rand_coef * init_w / net.resize_step + 1) * net.resize_step;
      int max_dim_h =
          roundl(rand_coef * init_h / net.resize_step + 1) * net.resize_step;

      // at the beginning (check if enough memory) and at the end (calc rolling
      // mean/variance)
      if (avg_loss < 0 || get_current_iteration(net) > net.max_batches - 100) {
        dim_w = max_dim_w;
        dim_h = max_dim_h;
      }

      if (dim_w < net.resize_step) dim_w = net.resize_step;
      if (dim_h < net.resize_step) dim_h = net.resize_step;
      int dim_b = (init_b * max_dim_w * max_dim_h) / (dim_w * dim_h);
      int new_dim_b = (int)(dim_b * 0.8);
      if (new_dim_b > init_b) dim_b = new_dim_b;

      args.w = dim_w;
      args.h = dim_h;

      int k;
      if (net.dynamic_minibatch) {
        for (k = 0; k < ctx->ngpus; ++k) {
          (*nets[k].seen) =
              init_b * net.subdivisions *
              get_current_iteration(
                  net);  // remove this line, when you will save to weights-file
                         // both: seen & cur_iteration
          nets[k].batch = dim_b;
          int j;
          for (j = 0; j < nets[k].n; ++j) nets[k].layers[j].batch = dim_b;
        }
        net.batch = dim_b;
        imgs = net.batch * net.subdivisions * ctx->ngpus;
        args.n = imgs;
        printf("\n %d x %d  (batch = %d) \n", dim_w, dim_h, net.batch);
      } else
        printf("\n %d x %d \n", dim_w, dim_h);

      pthread_join(load_thread, 0);
      train = buffer;
      free_data(train);
      load_thread = load_data(args);

      for (k = 0; k < ctx->ngpus; ++k) {
        resize_network(nets + k, dim_w, dim_h);
      }
      net = nets[0];
    }
    double time = what_time_is_it_now();
    pthread_join(load_thread, 0);
    train = buffer;
    if (net.track) {
      net.sequential_subdivisions = get_current_seq_subdivisions(net);
      args.threads = net.sequential_subdivisions * ctx->ngpus;
      printf(" sequential_subdivisions = %d, sequence = %d \n",
             net.sequential_subdivisions, get_sequence_value(net));
    }
    load_thread = load_data(args);
    // wait_key_cv(500);

    /*
    int k;
    for(k = 0; k < l.max_boxes; ++k){
    box b = float_to_box(train.y.vals[10] + 1 + k*5);
    if(!b.x) break;
    printf("loaded: %f %f %f %f\n", b.x, b.y, b.w, b.h);
    }
    image im = float_to_image(448, 448, 3, train.X.vals[10]);
    int k;
    for(k = 0; k < l.max_boxes; ++k){
    box b = float_to_box(train.y.vals[10] + 1 + k*5);
    printf("%d %d %d %d\n", truth.x, truth.y, truth.w, truth.h);
    draw_bbox(im, b, 8, 1,0,0);
    }
    save_image(im, "truth11");
    */

    const double load_time = (what_time_is_it_now() - time);
    printf("Loaded: %lf seconds", load_time);
    if (load_time > 0.1 && avg_loss > 0)
      printf(" - performance bottleneck on CPU or Disk HDD/SSD");
    printf("\n");

    time = what_time_is_it_now();
    float loss = 0;
#ifdef GPU
    if (ngpus == 1) {
      int wait_key = (dont_show) ? 0 : 1;
      loss = train_network_waitkey(net, train, wait_key);
    } else {
      loss = train_networks(nets, ngpus, train, 4);
    }
#else
    loss = train_network(net, train);
#endif
    if (avg_loss < 0 || avg_loss != avg_loss)
      avg_loss = loss;  // if(-inf or nan)
    avg_loss = avg_loss * .9 + loss * .1;

    const int iteration = get_current_iteration(net);
    // i = get_current_batch(net);

    int calc_map_for_each =
        ctx->mAP_epochs * train_images_num /
        (net.batch * net.subdivisions);  // calculate mAP every mAP_epochs
    calc_map_for_each = fmax(calc_map_for_each, 100);
    int next_map_calc = iter_map + calc_map_for_each;
    next_map_calc = fmax(next_map_calc, net.burn_in);
    // next_map_calc = fmax(next_map_calc, 400);
    if (ctx->calc_map) {
      printf("\n (next mAP calculation at %d iterations) ", next_map_calc);
      if (mean_average_precision > 0)
        printf("\n Last accuracy mAP@%0.2f = %2.2f %%, best = %2.2f %% ",
               ctx->iou_thresh, mean_average_precision * 100, best_map * 100);
    }

    printf("\033[H\033[J");
    if (mean_average_precision > 0.0) {
      printf("%d/%d: loss=%0.1f map=%0.2f best=%0.2f hours left=%0.1f\007",
             iteration, net.max_batches, loss, mean_average_precision, best_map,
             avg_time);
    } else {
      printf("%d/%d: loss=%0.1f hours left=%0.1f\007", iteration,
             net.max_batches, loss, avg_time);
    }

    if (net.cudnn_half) {
      if (iteration < net.burn_in * 3)
        fprintf(stderr,
                "\n Tensor Cores are disabled until the first %d iterations "
                "are reached.\n",
                3 * net.burn_in);
      else
        fprintf(stderr, "\n Tensor Cores are used.\n");
      fflush(stderr);
    }
    printf(
        "\n %d: %f, %f avg loss, %f rate, %lf seconds, %d images, %f hours "
        "left\n",
        iteration, loss, avg_loss, get_current_rate(net),
        (what_time_is_it_now() - time), iteration * imgs, avg_time);
    fflush(stdout);

    int draw_precision = 0;
    if (ctx->calc_map &&
        (iteration >= next_map_calc || iteration == net.max_batches)) {
      if (l.random) {
        printf("Resizing to initial size: %d x %d ", init_w, init_h);
        args.w = init_w;
        args.h = init_h;
        int k;
        if (net.dynamic_minibatch) {
          for (k = 0; k < ctx->ngpus; ++k) {
            for (k = 0; k < ctx->ngpus; ++k) {
              nets[k].batch = init_b;
              int j;
              for (j = 0; j < nets[k].n; ++j) nets[k].layers[j].batch = init_b;
            }
          }
          net.batch = init_b;
          imgs = init_b * net.subdivisions * ctx->ngpus;
          args.n = imgs;
          printf("\n %d x %d  (batch = %d) \n", init_w, init_h, init_b);
        }
        pthread_join(load_thread, 0);
        free_data(train);
        train = buffer;
        load_thread = load_data(args);
        for (k = 0; k < ctx->ngpus; ++k) {
          resize_network(nets + k, init_w, init_h);
        }
        net = nets[0];
      }

      copy_weights_net(net, &ctx->net_map);

      // combine Training and Validation networks
      // network net_combined = combine_train_valid_networks(net, net_map);

      iter_map = iteration;
      mean_average_precision =
          validate_detector_map(ctx->datacfg, ctx->cfg, ctx->weights,
                                ctx->thresh, ctx->iou_thresh, 0, net.letter_box,
                                &ctx->net_map);  // &net_combined);
      printf("\n mean_average_precision (mAP@%0.2f) = %f \n", ctx->iou_thresh,
             mean_average_precision);
      if (mean_average_precision >= best_map) {
        best_map = mean_average_precision;
        printf("New best mAP!\n");
        char buff[256];
        sprintf(buff, "%s/%s_best.weights", ctx->backup_directory, base);
        save_weights(net, buff);
      }

      draw_precision = 1;
    }
    time_remaining = ((net.max_batches - iteration) / ctx->ngpus) *
                     (what_time_is_it_now() - time + load_time) / 60 / 60;
    // set initial value, even if resume training from 10000 iteration
    if (avg_time < 0) avg_time = time_remaining;
    else
      avg_time = alpha_time * time_remaining + (1 - alpha_time) * avg_time;
#ifdef OPENCV
    if (net.contrastive) {
      float cur_con_acc = -1;
      for (k = 0; k < net.n; ++k)
        if (net.layers[k].type == CONTRASTIVE)
          cur_con_acc = *net.layers[k].loss;
      if (cur_con_acc >= 0)
        avg_contrastive_acc = avg_contrastive_acc * 0.99 + cur_con_acc * 0.01;
      printf("  avg_contrastive_acc = %f \n", avg_contrastive_acc);
    }
    draw_train_loss(windows_name, img, img_size, avg_loss, max_img_loss,
                    iteration, net.max_batches, mean_average_precision,
                    draw_precision, "mAP%", avg_contrastive_acc / 100,
                    ctx->dont_show, ctx->mjpeg_port, avg_time);
#endif  // OPENCV

    if ((iteration >= (iter_save + save_after_iterations) ||
         iteration % save_after_iterations == 0)) {
      iter_save = iteration;
#ifdef GPU
      if (ngpus != 1) sync_nets(nets, ngpus, 0);
#endif
      char buff[256];
      sprintf(buff, "%s/%s_%d.weights", ctx->backup_directory, base, iteration);
      save_weights(net, buff);
    }

    if ((save_after_iterations > save_last_weights_after) &&
        (iteration >= (iter_save_last + save_last_weights_after) ||
         (iteration % save_last_weights_after == 0 && iteration > 1))) {
      iter_save_last = iteration;
#ifdef GPU
      if (ngpus != 1) sync_nets(nets, ngpus, 0);
#endif
      char buff[256];
      sprintf(buff, "%s/%s_last.weights", ctx->backup_directory, base);
      save_weights(net, buff);

      if (net.ema_alpha && is_ema_initialized(net)) {
        sprintf(buff, "%s/%s_ema.weights", ctx->backup_directory, base);
        save_weights_upto(net, buff, net.n, 1);
        printf(" EMA weights are saved to the file: %s \n", buff);
      }
    }
    free_data(train);
  }
#ifdef GPU
  if (ngpus != 1) sync_nets(nets, ngpus, 0);
#endif
  char buff[256];
  sprintf(buff, "%s/%s_final.weights", ctx->backup_directory, base);
  save_weights(net, buff);
  printf(
      "If you want to train from the beginning, then use flag in the end of "
      "training command: -clear \n");

#ifdef OPENCV
  release_mat(&img);
  destroy_all_windows_cv();
#endif

  // free memory
  pthread_join(load_thread, 0);
  free_data(buffer);

  free_load_threads(&args);

  free(base);
  free(paths);
  free_list_contents(plist);
  free_list(plist);

  free_list_contents_kvp(ctx->options);
  free_list(ctx->options);
  ctx->options = NULL;

  for (k = 0; k < ctx->ngpus; ++k) free_network(nets[k]);
  free(nets);
  // free_network(net);

  calc_map_free_ctx(ctx);

  return 1;
}