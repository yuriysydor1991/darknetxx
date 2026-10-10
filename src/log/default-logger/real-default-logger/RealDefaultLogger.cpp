#include "src/log/default-logger/real-default-logger/RealDefaultLogger.h"

#include <array>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <locale>
#include <thread>

namespace default_logger
{

void RealDefaultLogger::log(const unsigned short& loglvl,
                            const std::string& msg)
{
  if (loglvl > lvl.load()) {
    return;
  }

  // the timestamp is formatted by the not thread safe std::localtime
  std::lock_guard<std::mutex> alogfile_m_guard{alogfile_m};

  std::ostringstream finalLog;

  // The layout of the line is the logger's own, so it takes no digit groups
  // of the application global locale (the user's one the GTKmm installs),
  // which turn the thread id into 140,482,768,250,560. The message itself is
  // formatted by the LOG_BODY stream of the application locale.
  finalLog.imbue(std::locale::classic());

  insert_current_timestamp(finalLog);

  finalLog << " " << lvl_repr(loglvl) << " " << std::this_thread::get_id()
           << " " << msg << '\n';

  const std::string finalLogStr = finalLog.str();

  // every message is flushed right away, so neither a crash, a kill nor a
  // pipe reader loses or delays the latest ones
  if (alogfile.is_open()) {
    alogfile << finalLogStr << std::flush;
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

void RealDefaultLogger::log(const unsigned short& loglvl,
                            const char* const filePath, const int& fileLine,
                            const std::string& msg)
{
  if (loglvl > lvl.load()) {
    return;
  }

  const std::string filename =
      std::filesystem::path{filePath}.filename().string();

  log(loglvl, filename + ":" + std::to_string(fileLine) + " : " + msg);
}

void RealDefaultLogger::logfile(const std::string& filepath)
{
  open_logfile(filepath);
}

bool RealDefaultLogger::open_logfile(const std::string& filepath)
{
  if (filepath.empty()) {
    return true;
  }

  std::lock_guard<std::mutex> alogfile_m_guard{alogfile_m};

  if (alogfile.is_open()) {
    alogfile.close();
  }

  alogfile.open(filepath.c_str(), std::fstream::app);

  if (!alogfile.is_open()) {
    std::cerr << "Fail to open the log file at " << filepath
              << ", logging to the console only" << std::endl;
    return false;
  }

  return true;
}

void RealDefaultLogger::print(const bool toPrintValue)
{
  toPrintMsgs.store(toPrintValue);
}

void RealDefaultLogger::level(const unsigned short& nlvl) { lvl.store(nlvl); }

void RealDefaultLogger::init(const std::string& filepath,
                             const unsigned short& nlvl,
                             const bool toPrintValue)
{
  logfile(filepath);
  level(nlvl);
  print(toPrintValue);
}

const std::string& RealDefaultLogger::lvl_repr(const unsigned short& glvl)
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

void RealDefaultLogger::insert_current_timestamp(std::ostringstream& oss)
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

std::string RealDefaultLogger::get_full_log_path(const std::string& logname)
{
  static const std::filesystem::path default_log_path = DEFAULT_LOG_FILE_PATH;

  return (default_log_path / logname).string();
}

std::string RealDefaultLogger::get_default_full_log_path()
{
  return get_full_log_path(default_log_name);
}

}  // namespace default_logger
