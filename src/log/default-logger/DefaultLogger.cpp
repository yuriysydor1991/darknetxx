#include "src/log/default-logger/DefaultLogger.h"

#include <cstdarg>
#include <cstdio>
#include <memory>

namespace default_logger
{

void DefaultLogger::log(const unsigned short& loglvl, const std::string& msg)
{
  real_logger()->log(loglvl, msg);
}

void DefaultLogger::log(const unsigned short& loglvl,
                        const char* const filePath, const int& fileLine,
                        const std::string& msg)
{
  real_logger()->log(loglvl, filePath, fileLine, msg);
}

bool DefaultLogger::enabled(const unsigned short& loglvl)
{
  return adopted.load() || loglvl <= lvl.load();
}

void DefaultLogger::logfile(const std::string& filepath)
{
  real_logger()->logfile(filepath);
}

void DefaultLogger::print(const bool toPrintValue)
{
  real_logger()->print(toPrintValue);
}

void DefaultLogger::level(const unsigned short& nlvl)
{
  lvl.store(nlvl);
  real_logger()->level(nlvl);
}

const std::string& DefaultLogger::lvl_repr(const unsigned short& glvl)
{
  return real_logger()->lvl_repr(glvl);
}

void DefaultLogger::init(const std::string& filepath,
                         const unsigned short& nlvl, const bool toPrintValue)
{
  lvl.store(nlvl);
  real_logger()->init(filepath, nlvl, toPrintValue);
}

std::string DefaultLogger::prepare_buff(const char* fmt, ...)
{
  va_list args;
  va_start(args, fmt);

  // the first pass only measures the message, so it works on a copy of the
  // arguments, which the second pass consumes then
  va_list sizing_args;
  va_copy(sizing_args, args);
  const int size = std::vsnprintf(nullptr, 0, fmt, sizing_args);
  va_end(sizing_args);

  if (size < 0) {
    va_end(args);
    return std::string{"Fail to format the message: "} + fmt;
  }

  std::string rt(static_cast<std::string::size_type>(size) + 1U, '\0');

  std::vsnprintf(rt.data(), rt.size(), fmt, args);

  va_end(args);

  // the string holds the message characters only, no terminating zero
  rt.resize(static_cast<std::string::size_type>(size));

  return rt;
}

DefaultLogger::RealLoggerPtr DefaultLogger::real_logger()
{
  return real_logger_holder();
}

void DefaultLogger::real_logger(const RealLoggerPtr& newRealLogger)
{
  if (!newRealLogger) {
    return;
  }

  real_logger_holder() = newRealLogger;
  adopted.store(true);
}

bool DefaultLogger::real_logger_adopted() { return adopted.load(); }

DefaultLogger::RealLoggerPtr& DefaultLogger::real_logger_holder()
{
  static RealLoggerPtr* const realLogger =
      new RealLoggerPtr{std::make_shared<RealDefaultLogger>()};

  return *realLogger;
}

}  // namespace default_logger
