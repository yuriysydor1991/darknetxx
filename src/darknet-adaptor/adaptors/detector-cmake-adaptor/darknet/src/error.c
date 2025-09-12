#include "error.h"

#include <errno.h>
#include <execinfo.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "src/clog/clog.h"
#include "string_tools.h"

void log_backtrace()
{
#ifndef WIN32
  void *buffer[PATH_MAX];
  int count = backtrace(buffer, sizeof(buffer));
  char **symbols = backtrace_symbols(buffer, count);

  CLOGE("backtrace (%d entries)", count);

  for (int idx = 0; idx < count; idx++) {
    CLOGE("%d/%d: %s", idx + 1, count, symbols[idx]);
  }

  free(symbols);
#endif
}

void malloc_error(const size_t size, const char *const filename,
                  const char *const funcname, const int line)
{
  CLOGE("Failed to malloc %s", size_to_IEC_string(size));
  error("Malloc error - possibly out of CPU RAM", filename, funcname, line);
}

void calloc_error(const size_t size, const char *const filename,
                  const char *const funcname, const int line)
{
  CLOGE("Failed to calloc %s", size_to_IEC_string(size));
  error("Calloc error - possibly out of CPU RAM", filename, funcname, line);
}

void realloc_error(const size_t size, const char *const filename,
                   const char *const funcname, const int line)
{
  CLOGE("Failed to realloc %s", size_to_IEC_string(size));
  error("Realloc error - possibly out of CPU RAM", filename, funcname, line);
}

void error(const char *const msg, const char *const filename,
           const char *const funcname, const int line)
{
  const int en = errno;
  CLOGE("Darknet error location: %s, %s(), line #%d - %s", filename, funcname,
        line, strerror(en));

  log_backtrace();

  exit(EXIT_FAILURE);
}
