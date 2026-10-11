#ifndef DARKNET_DRAW_TRAIN_LOSS_CTX_C_FUNCTION_H
#define DARKNET_DRAW_TRAIN_LOSS_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

void draw_train_loss_ctx(struct detector_context* ctx);

/// @brief The iterations between the saves of the training chart: the cfg
/// savechart= ones, every one of the --save-charts-every-iter flag for none,
/// every 100 (the original Darknet cadence) otherwise.
int save_chart_nth_ctx(const struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_DRAW_TRAIN_LOSS_CTX_C_FUNCTION_H
