#ifndef DARKNET_DETECTOR_ARGS_PARSER_C_FUNCTION_H
#define DARKNET_DETECTOR_ARGS_PARSER_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

int detector_args_parser(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_DETECTOR_ARGS_PARSER_C_FUNCTION_H