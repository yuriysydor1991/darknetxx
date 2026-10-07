#include "memory.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "string_tools.h"

void *xmalloc_location(const size_t size, const char *const filename,
                       const char *const funcname, const int line)
{
  void *ptr = malloc(size + 1);

  if (!ptr) {
    malloc_error(size, filename, funcname, line);
  }

  memset(ptr, 0, size + 1);

  return ptr;
}

void *xcalloc_location(const size_t nmemb, const size_t size,
                       const char *const filename, const char *const funcname,
                       const int line)
{
  void *ptr = calloc(nmemb, size);

  if (!ptr) {
    calloc_error(nmemb * size, filename, funcname, line);
  }

  return ptr;
}

void *xrealloc_location(void *ptr, const size_t size,
                        const char *const filename, const char *const funcname,
                        const int line)
{
  ptr = realloc(ptr, size);

  if (!ptr) {
    realloc_error(size, filename, funcname, line);
  }

  return ptr;
}
