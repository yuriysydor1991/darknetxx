#include "src/detector/prepare_load_args_ctx.h"

#include <assert.h>
#include <string.h>

void prepare_load_args_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);
  
  if (ctx == NULL) {
    return;
  }

  memset(&ctx->args, 0, sizeof(load_args));

  ctx->args.w = ctx->net.w;
  ctx->args.h = ctx->net.h;
  ctx->args.c = ctx->net.c;
  ctx->args.paths = ctx->paths;
  ctx->args.n = ctx->imgs;
  ctx->args.m = ctx->plist->size;
  ctx->args.classes = ctx->classes;
  ctx->args.flip = ctx->net.flip;
  ctx->args.jitter = ctx->l.jitter;
  ctx->args.resize = ctx->l.resize;
  ctx->args.num_boxes = ctx->l.max_boxes;
  ctx->args.truth_size = ctx->l.truth_size;
  ctx->net.num_boxes = ctx->args.num_boxes;
  ctx->net.train_images_num = ctx->train_images_num;
  ctx->args.d = &ctx->buffer;
  ctx->args.type = DETECTION_DATA;
  ctx->args.threads = 64;  // 16 or 64

  ctx->args.angle = ctx->net.angle;
  ctx->args.gaussian_noise = ctx->net.gaussian_noise;
  ctx->args.blur = ctx->net.blur;
  ctx->args.mixup = ctx->net.mixup;
  ctx->args.exposure = ctx->net.exposure;
  ctx->args.saturation = ctx->net.saturation;
  ctx->args.hue = ctx->net.hue;
  ctx->args.letter_box = ctx->net.letter_box;
  ctx->args.mosaic_bound = ctx->net.mosaic_bound;
  ctx->args.contrastive = ctx->net.contrastive;
  ctx->args.contrastive_jit_flip = ctx->net.contrastive_jit_flip;
  ctx->args.contrastive_color = ctx->net.contrastive_color;

  if (ctx->dont_show && ctx->show_imgs) ctx->show_imgs = 2;

  ctx->args.show_imgs = ctx->show_imgs;
}