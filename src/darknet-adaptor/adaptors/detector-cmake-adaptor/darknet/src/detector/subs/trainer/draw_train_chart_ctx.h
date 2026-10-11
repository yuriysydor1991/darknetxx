#ifndef DARKNET_DRAW_TRAIN_CHART_CTX_C_FUNCTION_H
#define DARKNET_DRAW_TRAIN_CHART_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Starts and saves the chart_<base>_<the start date and time>.png
/// chart of the training: the whole record of the resumed training redrawn,
/// the chart_path image continued for none.
void draw_train_chart_ctx(struct detector_context* ctx);

/// @brief Saves the chart of the training, if any.
void save_train_chart_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_DRAW_TRAIN_CHART_CTX_C_FUNCTION_H
