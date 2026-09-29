#ifndef DARKNET_TRAIN_DATA_CTX_C_FUNCTION_H
#define DARKNET_TRAIN_DATA_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The training batches are loaded in the background into the ctx->buffer, so
 * the next batch loads while the current one trains (the way the original
 * Darknet did). A taken batch moves into the ctx->train, which owns it from
 * then on.
 */

/// @brief Tells if the given training iteration takes a fresh batch, see the
/// --data-reload-nth and the --dont-reload-data-every-iteration options.
int train_data_reload_due_ctx(const struct detector_context* ctx,
                              const int iteration);

/// @brief Starts the background load of a batch, unless one runs already.
void start_train_data_load_ctx(struct detector_context* ctx);

/// @brief Makes the batch of the background load (started now if none runs)
/// the ctx->train one and frees the previous one.
void take_train_data_ctx(struct detector_context* ctx);

/// @brief Waits for the background load and frees its batch, for example when
/// the network input changes and the batch doesn't fit it any more.
void drop_train_data_load_ctx(struct detector_context* ctx);

/// @brief Frees the ctx->train batch and the one of the background load.
void free_train_data_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_TRAIN_DATA_CTX_C_FUNCTION_H
