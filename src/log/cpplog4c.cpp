#include "src/log/cpplog4c.h"

#include <cstdio>

#include "src/log/log.h"

namespace
{

/**
 * @brief Writes the message straight to the stderr, the fallback of a failed
 * logging (a std::bad_alloc when the memory is out, for example). No exception
 * may leave the functions below, since their C callers can't handle it.
 */
void fallback_log(const char* const buff)
{
  if (buff != nullptr) {
    std::fputs(buff, stderr);
    std::fputc('\n', stderr);
  }
}

}  // namespace

extern "C" {

int cpplog_enabled(const unsigned int level)
{
  return default_logger::DefaultLogger::enabled(
             static_cast<unsigned short>(level))
             ? 1
             : 0;
}

// The file and the line are unused by the levels the MAX_LOG_LEVEL value
// compiles out.
void cpploge([[maybe_unused]] const char* const file,
             [[maybe_unused]] const size_t line, const char* const buff)
{
  try {
    ELOGE(file, line, buff);
  }
  catch (...) {
    fallback_log(buff);
  }
}
void cpplogw([[maybe_unused]] const char* const file,
             [[maybe_unused]] const size_t line, const char* const buff)
{
  try {
    ELOGW(file, line, buff);
  }
  catch (...) {
    fallback_log(buff);
  }
}
void cpplogi([[maybe_unused]] const char* const file,
             [[maybe_unused]] const size_t line, const char* const buff)
{
  try {
    ELOGI(file, line, buff);
  }
  catch (...) {
    fallback_log(buff);
  }
}
void cpplogd([[maybe_unused]] const char* const file,
             [[maybe_unused]] const size_t line, const char* const buff)
{
  try {
    ELOGD(file, line, buff);
  }
  catch (...) {
    fallback_log(buff);
  }
}
void cpplogt([[maybe_unused]] const char* const file,
             [[maybe_unused]] const size_t line, const char* const buff)
{
  try {
    ELOGT(file, line, buff);
  }
  catch (...) {
    fallback_log(buff);
  }
}
}
