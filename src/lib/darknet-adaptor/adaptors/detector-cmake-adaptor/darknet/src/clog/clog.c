#include "src/clog/clog.h"

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/// @brief log given buffer as INFO severity
extern void cpploge(const char* file, const size_t line, const char* buff);
extern void cpplogw(const char* file, const size_t line, const char* buff);
extern void cpplogi(const char* file, const size_t line, const char* buff);
extern void cpplogd(const char* file, const size_t line, const char* buff);
extern void cpplogt(const char* file, const size_t line, const char* buff);

#if defined(__GNUC__) || defined(__clang__)
#define WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#elif defined(_MSC_VER)
#define WARN_UNUSED_RESULT _Check_return_
#else
#define WARN_UNUSED_RESULT
#endif

// Formats the message into an exactly sized buffer, or returns NULL on a
// failure. Uses the plain malloc and logs nothing itself, since the out of
// memory handling logs through here too (an xmalloc failure would recurse).
WARN_UNUSED_RESULT
static char* prepare_buff(const char* fmt, va_list args)
{
  // the first pass only measures the message, so it works on a copy of the
  // arguments, which the second pass consumes then
  va_list sizing_args;
  va_copy(sizing_args, args);
  const int size = vsnprintf(NULL, 0, fmt, sizing_args);
  va_end(sizing_args);

  if (size < 0) {
    return NULL;
  }

  char* buff = (char*)malloc((size_t)size + 1);

  if (buff != NULL) {
    vsnprintf(buff, (size_t)size + 1, fmt, args);
  }

  return buff;
}

// The body of every printf-style logging function: formats the message only
// for an enabled level and logs the bare format when the formatting fails.
#define CLOG_FORMATTED(LEVEL, BRIDGE)              \
  do {                                             \
    if (!cpplog_enabled(LEVEL)) {                  \
      return;                                      \
    }                                              \
                                                   \
    va_list args;                                  \
    va_start(args, fmt);                           \
    char* buff = prepare_buff(fmt, args);          \
    va_end(args);                                  \
                                                   \
    BRIDGE(file, line, buff != NULL ? buff : fmt); \
                                                   \
    free(buff);                                    \
  } while (0)

void cloge(const char* file, const size_t line, const char* fmt, ...)
{
  CLOG_FORMATTED(MACRO_LVL_ERROR, cpploge);
}

void clogw(const char* file, const size_t line, const char* fmt, ...)
{
  CLOG_FORMATTED(MACRO_LVL_WARNING, cpplogw);
}

void clogi(const char* file, const size_t line, const char* fmt, ...)
{
  CLOG_FORMATTED(MACRO_LVL_INFO, cpplogi);
}

void clogd(const char* file, const size_t line, const char* fmt, ...)
{
  CLOG_FORMATTED(MACRO_LVL_DEBUG, cpplogd);
}

void clogt(const char* file, const size_t line, const char* fmt, ...)
{
  CLOG_FORMATTED(MACRO_LVL_TRACE, cpplogt);
}

void clogem(const char* file, const size_t line, const char* msg)
{
  if (cpplog_enabled(MACRO_LVL_ERROR)) {
    cpploge(file, line, msg);
  }
}

void clogwm(const char* file, const size_t line, const char* msg)
{
  if (cpplog_enabled(MACRO_LVL_WARNING)) {
    cpplogw(file, line, msg);
  }
}

void clogim(const char* file, const size_t line, const char* msg)
{
  if (cpplog_enabled(MACRO_LVL_INFO)) {
    cpplogi(file, line, msg);
  }
}

void clogdm(const char* file, const size_t line, const char* msg)
{
  if (cpplog_enabled(MACRO_LVL_DEBUG)) {
    cpplogd(file, line, msg);
  }
}

void clogtm(const char* file, const size_t line, const char* msg)
{
  if (cpplog_enabled(MACRO_LVL_TRACE)) {
    cpplogt(file, line, msg);
  }
}
