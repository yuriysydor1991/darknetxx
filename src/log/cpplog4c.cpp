#include "src/log/cpplog4c.h"

#include "src/log/log.h"

extern "C" {

void cpploge(const char* const file, const size_t line, const char* const buff)
{
  ELOGE(file, line, buff);
}
void cpplogw(const char* const file, const size_t line, const char* const buff)
{
  ELOGW(file, line, buff);
}
void cpplogi(const char* const file, const size_t line, const char* const buff)
{
  ELOGI(file, line, buff);
}
void cpplogd(const char* const file, const size_t line, const char* const buff)
{
  ELOGD(file, line, buff);
}
void cpplogt(const char* const file, const size_t line, const char* const buff)
{
  ELOGT(file, line, buff);
}
}
