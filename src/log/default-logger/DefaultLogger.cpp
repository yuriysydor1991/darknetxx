#include "src/log/default-logger/DefaultLogger.h"

#include <array>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>

namespace default_logger
{

void DefaultLogger::log(const unsigned short& loglvl, const std::string& msg)
{
  if (!enabled(loglvl)) {
    return;
  }

  std::lock_guard<std::mutex> alogfile_m_guard{alogfile_m()};

  std::ostringstream finalLog;

  insert_current_timestamp(finalLog);

  finalLog << " " << lvl_repr(loglvl) << " " << std::this_thread::get_id()
           << " " << msg << '\n';

  const std::string finalLogStr = finalLog.str();

  // Every record is flushed right away, so neither a crash, a kill nor a pipe
  // reader loses or delays the latest messages (the training progress ones
  // included).
  if (alogfile().is_open()) {
    alogfile() << finalLogStr << std::flush;
  }

  if (!toPrintMsgs.load()) {
    return;
  }

  if (loglvl <= LVL_WARNING) {
    std::cerr << finalLogStr << std::flush;
  } else {
    std::cout << finalLogStr << std::flush;
  }
}

void DefaultLogger::log(const unsigned short& loglvl,
                        const char* const filePath, const int& fileLine,
                        const std::string& msg)
{
  if (!enabled(loglvl)) {
    return;
  }

  std::filesystem::path fullPath{filePath};

  std::string filename = fullPath.filename().string();

  log(loglvl, filename + ":" + std::to_string(fileLine) + " : " + msg);
}

bool DefaultLogger::enabled(const unsigned short& loglvl)
{
  return loglvl <= lvl.load();
}

bool DefaultLogger::logfile(const std::string& filepath)
{
  if (filepath.empty()) {
    return true;
  }

  std::lock_guard<std::mutex> alogfile_m_guard{alogfile_m()};

  if (alogfile().is_open()) {
    alogfile().close();
  }

  alogfile().open(filepath.c_str(), std::fstream::app);

  if (!alogfile().is_open()) {
    std::cerr << "Fail to open the log file at " << filepath
              << ", logging to the console only" << std::endl;
    return false;
  }

  return true;
}

std::fstream& DefaultLogger::alogfile()
{
  // never destroyed on purpose, see the declaration
  static std::fstream* const stream = new std::fstream{};

  return *stream;
}

std::mutex& DefaultLogger::alogfile_m()
{
  // never destroyed on purpose, see the declaration
  static std::mutex* const mutex = new std::mutex{};

  return *mutex;
}

void DefaultLogger::print(const bool toPrintValue)
{
  toPrintMsgs.store(toPrintValue);
}

void DefaultLogger::level(const unsigned short& nlvl) { lvl.store(nlvl); }

void DefaultLogger::init(const std::string& filepath,
                         const unsigned short& nlvl, const bool toPrintValue)
{
  logfile(filepath);
  level(nlvl);
  print(toPrintValue);
}

inline void DefaultLogger::insert_current_timestamp(std::ostringstream& oss)
{
  using namespace std::chrono;

  const auto now = system_clock::now();

  const time_t now_time_t = system_clock::to_time_t(now);
  std::tm timeHolder = *std::localtime(&now_time_t);

  oss << std::put_time(&timeHolder, defaultLogDateFormat);

#ifdef ENABLE_LOGS_MICROSECONDS_TIME
  static constexpr const char microsecFiller = '0';
  static constexpr const unsigned int microsecWidth = 6U;

  const auto timeSinceEpoch = now.time_since_epoch();
  auto seconds = duration_cast<std::chrono::seconds>(timeSinceEpoch);
  auto microseconds =
      duration_cast<std::chrono::microseconds>(timeSinceEpoch - seconds);

  oss << '.' << std::setfill(microsecFiller) << std::setw(microsecWidth)
      << microseconds.count();
#endif  // ENABLE_LOGS_MICROSECONDS_TIME
}

const std::string& DefaultLogger::lvl_repr(const unsigned short& glvl)
{
  static constexpr const unsigned short maxLvls = 6U;
  // never destroyed, since a thread may log during the exit() of another one
  static const std::array<const std::string, maxLvls>* const reprs =
      new std::array<const std::string, maxLvls>{
          std::string{"UNK"}, std::string{"ERR"}, std::string{"WRN"},
          std::string{"INF"}, std::string{"DBG"}, std::string{"TRA"},
      };

  if (glvl >= maxLvls) {
    return (*reprs)[0U];
  }

  return (*reprs)[glvl];
}

std::string DefaultLogger::get_full_log_path(const std::string& logname)
{
  namespace fs = std::filesystem;

  static const fs::path default_log_path = DEFAULT_LOG_FILE_PATH;

  const fs::path logpath = default_log_path / logname;

  return logpath.string();
}

std::string DefaultLogger::get_default_full_log_path()
{
  return get_full_log_path(default_log_name);
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

}  // namespace default_logger
