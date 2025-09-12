#include "src/clog/clog.h"

#include <assert.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

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

WARN_UNUSED_RESULT
static char* prepare_buff(const char* fmt, va_list* args)
{
  assert(args != NULL);

  static const size_t DEF_BUFF_CHUNK_SIZE = 102400;

  size_t chunks = 1;
  size_t buff_size = chunks * DEF_BUFF_CHUNK_SIZE;

  char* buff = xmalloc(buff_size + 1);

  assert(buff != NULL);

  if (buff == NULL) {
    return NULL;
  }

  memset(buff, 0, buff_size + 1);

  while (vsnprintf(buff, buff_size, fmt, *args) >= buff_size) {
    buff_size = ++chunks * DEF_BUFF_CHUNK_SIZE;
    buff = xrealloc(buff, buff_size + 1);

    assert(buff != NULL);

    if (buff == NULL) {
      return NULL;
    }
  }

  return buff;
}

void cloge(const char* file, const size_t line, const char* fmt, ...)
{
  va_list args;

  va_start(args, fmt);

  char* buff = prepare_buff(fmt, &args);

  va_end(args);

  cpploge(file, line, buff);

  free(buff);
}

void clogw(const char* file, const size_t line, const char* fmt, ...)
{
  va_list args;

  va_start(args, fmt);

  char* buff = prepare_buff(fmt, &args);

  va_end(args);

  cpplogw(file, line, buff);

  free(buff);
}

void clogi(const char* file, const size_t line, const char* fmt, ...)
{
  va_list args;

  va_start(args, fmt);

  char* buff = prepare_buff(fmt, &args);

  va_end(args);

  cpplogi(file, line, buff);

  free(buff);
}

void clogd(const char* file, const size_t line, const char* fmt, ...)
{
  va_list args;

  va_start(args, fmt);

  char* buff = prepare_buff(fmt, &args);

  va_end(args);

  cpplogd(file, line, buff);

  free(buff);
}

void clogt(const char* file, const size_t line, const char* fmt, ...)
{
  va_list args;

  va_start(args, fmt);

  char* buff = prepare_buff(fmt, &args);

  va_end(args);

  cpplogt(file, line, buff);

  free(buff);
}

void clogem(const char* file, const size_t line, const char* msg)
{
  cpploge(file, line, msg);
}

void clogwm(const char* file, const size_t line, const char* msg)
{
  cpplogw(file, line, msg);
}

void clogim(const char* file, const size_t line, const char* msg)
{
  cpplogi(file, line, msg);
}

void clogdm(const char* file, const size_t line, const char* msg)
{
  cpplogd(file, line, msg);
}

void clogtm(const char* file, const size_t line, const char* msg)
{
  cpplogt(file, line, msg);
}
