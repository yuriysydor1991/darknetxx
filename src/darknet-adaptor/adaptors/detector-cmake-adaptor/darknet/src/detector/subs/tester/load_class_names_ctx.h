#ifndef DARKNET_LOAD_CLASS_NAMES_CTX_C_FUNCTION_H
#define DARKNET_LOAD_CLASS_NAMES_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns the class names file for the given network: the command line
 * one (ctx->name_list) when given, the names= key value of the network cfg
 * [net] section otherwise.
 */
char* get_class_names_file_ctx(const struct detector_context* ctx,
                               const network* net);

/**
 * @brief Checks that the class names file may be read, logging how to provide
 * one otherwise.
 *
 * @return 1 if the file is readable, 0 otherwise.
 */
int check_class_names_file(const char* names_file);

/**
 * @brief Loads the ctx->net network class names into the ctx->names and
 * ctx->names_size fields.
 *
 * Must be called after the network is parsed and before anything uses the
 * names, so a missing names file stops the detection before any output is
 * written instead of exiting the process in the middle of it.
 *
 * @return 1 on success, 0 otherwise.
 */
int load_class_names_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_LOAD_CLASS_NAMES_CTX_C_FUNCTION_H
