#ifndef DARKNET_MEMORY_MANAGMENT_RELATED_C_FUNCTIONS_H
#define DARKNET_MEMORY_MANAGMENT_RELATED_C_FUNCTIONS_H

#include <stddef.h>

#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

/* The "location" is the file, function, and line as defined by the DARKNET_LOC
 * macro. This is then printed when error() is called to terminate the instance
 * of darknet.
 */
void *xmalloc_location(const size_t size, const char *const filename,
                       const char *const funcname, const int line);
void *xcalloc_location(const size_t nmemb, const size_t size,
                       const char *const filename, const char *const funcname,
                       const int line);
void *xrealloc_location(void *ptr, const size_t size,
                        const char *const filename, const char *const funcname,
                        const int line);

#define xmalloc(s) xmalloc_location(s, DARKNET_LOC)
#define xcalloc(m, s) xcalloc_location(m, s, DARKNET_LOC)
#define xrealloc(p, s) xrealloc_location(p, s, DARKNET_LOC)

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_MEMORY_MANAGMENT_RELATED_C_FUNCTIONS_H
