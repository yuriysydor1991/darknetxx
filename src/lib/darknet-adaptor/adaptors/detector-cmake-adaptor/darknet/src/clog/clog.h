#ifndef DARKNET_CLOG_C_FUNCTION_H
#define DARKNET_CLOG_C_FUNCTION_H

#include <stddef.h>

#include "src/lib/facade/public/severity-macro-consts.h"

#ifdef __cplusplus
extern "C" {
#endif

// Lets the compiler check the formats of the printf-style logging calls.
#if defined(__GNUC__) || defined(__clang__)
#define CLOG_PRINTF_FORMAT __attribute__((format(printf, 3, 4)))
#else
#define CLOG_PRINTF_FORMAT
#endif

/// @brief Returns non zero if the messages of the given level reach the log
/// (implemented by the C++ logger bridge).
int cpplog_enabled(const unsigned int level);

void cloge(const char* file, const size_t line, const char* fmt,
           ...) CLOG_PRINTF_FORMAT;
void clogw(const char* file, const size_t line, const char* fmt,
           ...) CLOG_PRINTF_FORMAT;
void clogi(const char* file, const size_t line, const char* fmt,
           ...) CLOG_PRINTF_FORMAT;
void clogd(const char* file, const size_t line, const char* fmt,
           ...) CLOG_PRINTF_FORMAT;
void clogt(const char* file, const size_t line, const char* fmt,
           ...) CLOG_PRINTF_FORMAT;

void clogim(const char* file, const size_t line, const char* msg);
void clogem(const char* file, const size_t line, const char* msg);
void clogwm(const char* file, const size_t line, const char* msg);
void clogdm(const char* file, const size_t line, const char* msg);
void clogtm(const char* file, const size_t line, const char* msg);

// The levels above the MAX_LOG_LEVEL are compiled out: their calls stay type
// checked, but no code is generated for them.
#if MAX_LOG_LEVEL >= MACRO_LVL_WARNING
#define CLOG_WARNING_COMPILED 1
#else
#define CLOG_WARNING_COMPILED 0
#endif

#if MAX_LOG_LEVEL >= MACRO_LVL_INFO
#define CLOG_INFO_COMPILED 1
#else
#define CLOG_INFO_COMPILED 0
#endif

#if MAX_LOG_LEVEL >= MACRO_LVL_DEBUG
#define CLOG_DEBUG_COMPILED 1
#else
#define CLOG_DEBUG_COMPILED 0
#endif

#if MAX_LOG_LEVEL >= MACRO_LVL_TRACE
#define CLOG_TRACE_COMPILED 1
#else
#define CLOG_TRACE_COMPILED 0
#endif

// Checks the level before anything else, so a dropped message costs neither
// its arguments evaluation nor its formatting.
#define CLOG_IF_ENABLED(COMPILED, LEVEL, CALL) \
  do {                                         \
    if ((COMPILED) && cpplog_enabled(LEVEL)) { \
      CALL;                                    \
    }                                          \
  } while (0)

#ifndef CLOGE
#define CLOGE(fmt, ...)               \
  CLOG_IF_ENABLED(1, MACRO_LVL_ERROR, \
                  cloge(__FILE__, __LINE__, fmt, __VA_ARGS__))
#endif

#ifndef CLOGW
#define CLOGW(fmt, ...)                                     \
  CLOG_IF_ENABLED(CLOG_WARNING_COMPILED, MACRO_LVL_WARNING, \
                  clogw(__FILE__, __LINE__, fmt, __VA_ARGS__))
#endif

#ifndef CLOGI
#define CLOGI(fmt, ...)                               \
  CLOG_IF_ENABLED(CLOG_INFO_COMPILED, MACRO_LVL_INFO, \
                  clogi(__FILE__, __LINE__, fmt, __VA_ARGS__))
#endif

#ifndef CLOGD
#define CLOGD(fmt, ...)                                 \
  CLOG_IF_ENABLED(CLOG_DEBUG_COMPILED, MACRO_LVL_DEBUG, \
                  clogd(__FILE__, __LINE__, fmt, __VA_ARGS__))
#endif

#ifndef CLOGT
#define CLOGT(fmt, ...)                                 \
  CLOG_IF_ENABLED(CLOG_TRACE_COMPILED, MACRO_LVL_TRACE, \
                  clogt(__FILE__, __LINE__, fmt, __VA_ARGS__))
#endif

#ifndef CLOGEM
#define CLOGEM(msg) \
  CLOG_IF_ENABLED(1, MACRO_LVL_ERROR, clogem(__FILE__, __LINE__, msg))
#endif

#ifndef CLOGWM
#define CLOGWM(msg)                                         \
  CLOG_IF_ENABLED(CLOG_WARNING_COMPILED, MACRO_LVL_WARNING, \
                  clogwm(__FILE__, __LINE__, msg))
#endif

#ifndef CLOGIM
#define CLOGIM(msg)                                   \
  CLOG_IF_ENABLED(CLOG_INFO_COMPILED, MACRO_LVL_INFO, \
                  clogim(__FILE__, __LINE__, msg))
#endif

#ifndef CLOGDM
#define CLOGDM(msg)                                     \
  CLOG_IF_ENABLED(CLOG_DEBUG_COMPILED, MACRO_LVL_DEBUG, \
                  clogdm(__FILE__, __LINE__, msg))
#endif

#ifndef CLOGTM
#define CLOGTM(msg)                                     \
  CLOG_IF_ENABLED(CLOG_TRACE_COMPILED, MACRO_LVL_TRACE, \
                  clogtm(__FILE__, __LINE__, msg))
#endif

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_CLOG_C_FUNCTION_H
