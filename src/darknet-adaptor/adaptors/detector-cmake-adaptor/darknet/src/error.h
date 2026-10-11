#ifndef DARKNET_ERROR_C_FUNCTIONS_H
#define DARKNET_ERROR_C_FUNCTIONS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void log_backtrace();

void malloc_error(const size_t size, const char *const filename,
                  const char *const funcname, const int line);
void calloc_error(const size_t size, const char *const filename,
                  const char *const funcname, const int line);
void realloc_error(const size_t size, const char *const filename,
                   const char *const funcname, const int line);
void error(const char *const msg, const char *const filename,
           const char *const funcname, const int line);

#define DARKNET_LOC __FILE__, __func__, __LINE__

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_ERROR_C_FUNCTIONS_H
