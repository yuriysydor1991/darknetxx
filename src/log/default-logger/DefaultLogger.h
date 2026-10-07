#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_DEFAULT_LOGGER_IMPLEMENTATION_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_DEFAULT_LOGGER_IMPLEMENTATION_CLASS_H

#include <atomic>
#include <string>

#include "src/lib/facade/public/ILogger.h"
#include "src/lib/facade/public/severity-macro-consts.h"
#include "src/log/default-logger/real-default-logger/RealDefaultLogger.h"

/**
 * @brief The default logger quick implementation encapsulation namespace.
 */
namespace default_logger
{

/**
 * @brief The static proxy the logging macros call, which forwards to the
 * logger::ILogger it holds: the own RealDefaultLogger or the adopted one, the
 * logger of the library user, for example.
 */
class DefaultLogger
{
 public:
  using RealLoggerPtr = logger::ILoggerPtr;

  inline static constexpr const unsigned short LVL_ERROR =
      logger::ILogger::LVL_ERROR;
  inline static constexpr const unsigned short LVL_WARNING =
      logger::ILogger::LVL_WARNING;
  inline static constexpr const unsigned short LVL_INFO =
      logger::ILogger::LVL_INFO;
  inline static constexpr const unsigned short LVL_DEBUG =
      logger::ILogger::LVL_DEBUG;
  inline static constexpr const unsigned short LVL_TRACE =
      logger::ILogger::LVL_TRACE;

  inline static constexpr const char* const defaultLogDateFormat =
      RealDefaultLogger::defaultLogDateFormat;

  inline static const std::string& default_log_name =
      RealDefaultLogger::default_log_name;

  virtual ~DefaultLogger() = default;
  DefaultLogger() = default;

  static void log(const unsigned short& loglvl, const std::string& msg);
  static void log(const unsigned short& loglvl, const char* const filePath,
                  const int& fileLine, const std::string& msg);

  /// @brief Tells the messages of the level are logged, so the dropped ones
  /// are not prepared at all.
  static bool enabled(const unsigned short& loglvl);

  static void logfile(const std::string& filepath);
  static void print(const bool toPrintValue);
  static void level(const unsigned short& nlvl);
  static const std::string& lvl_repr(const unsigned short& glvl);

  static void init(const std::string& filepath =
                       RealDefaultLogger::get_default_full_log_path(),
                   const unsigned short& nlvl = MAX_LOG_LEVEL,
                   const bool toPrintValue = true);

  /**
   * @brief Formats a printf-style message into a std::string. Used by the
   * FMTLOGI macro and the C logging bridge (cpplog4c) to forward the original
   * Darknet printf-style log calls.
   */
  static std::string prepare_buff(const char* fmt, ...)
#if defined(__GNUC__) || defined(__clang__)
      __attribute__((format(printf, 1, 2)))
#endif
      ;

  static RealLoggerPtr real_logger();

  /// @brief Adopts the logger for every following message, nothing for none.
  static void real_logger(const RealLoggerPtr& newRealLogger);

  static bool real_logger_adopted();

 private:
  /// @brief Never destroyed, since a thread may log during the exit() of
  /// another one.
  static RealLoggerPtr& real_logger_holder();

  /// @brief The level of the own logger, the adopted one filters by it's own.
  inline static std::atomic<unsigned short> lvl{MAX_LOG_LEVEL};
  inline static std::atomic_bool adopted{false};
};

}  // namespace default_logger

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_DEFAULT_LOGGER_IMPLEMENTATION_CLASS_H
