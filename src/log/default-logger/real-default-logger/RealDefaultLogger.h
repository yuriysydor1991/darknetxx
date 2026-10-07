#ifndef DARKNETXX_PROJECT_REAL_DEFAULT_LOGGER_CLASS_H
#define DARKNETXX_PROJECT_REAL_DEFAULT_LOGGER_CLASS_H

#include <atomic>
#include <fstream>
#include <mutex>
#include <sstream>
#include <string>

#include "project-global-decls.h"
#include "src/lib/facade/public/ILogger.h"

namespace default_logger
{

/**
 * @brief The logger of the project: the timestamped messages of the level and
 * the thread, flushed one by one into the log file and duplicated into the
 * standard error (the warnings and the errors) or the standard output.
 */
class RealDefaultLogger : public logger::ILogger
{
 public:
  inline static constexpr const char* const defaultLogDateFormat =
      "%Y-%m-%d %H:%M:%S";

  inline static const std::string default_log_name =
      project_decls::PROJECT_NAME + "-" + project_decls::PROJECT_BUILD_VERSION +
      ".log";

  ~RealDefaultLogger() override = default;
  RealDefaultLogger() = default;

  void log(const unsigned short& loglvl, const std::string& msg) override;
  void log(const unsigned short& loglvl, const char* const filePath,
           const int& fileLine, const std::string& msg) override;
  void logfile(const std::string& filepath) override;
  void print(const bool toPrintValue) override;
  void level(const unsigned short& nlvl) override;
  const std::string& lvl_repr(const unsigned short& glvl) override;
  void init(const std::string& filepath, const unsigned short& nlvl,
            const bool toPrintValue) override;

  /// @brief Opens the log file, false when unable to, the console logging
  /// going on then.
  bool open_logfile(const std::string& filepath);

  static std::string get_full_log_path(const std::string& logname);
  static std::string get_default_full_log_path();

 private:
  static void insert_current_timestamp(std::ostringstream& oss);

  std::atomic_bool toPrintMsgs{true};
  std::atomic<unsigned short> lvl{MAX_LOG_LEVEL};
  std::fstream alogfile;
  std::mutex alogfile_m;
};

}  // namespace default_logger

#endif  // DARKNETXX_PROJECT_REAL_DEFAULT_LOGGER_CLASS_H
