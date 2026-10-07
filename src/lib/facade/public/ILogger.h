#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_ILOGGER_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_ILOGGER_CLASS_H

#include <memory>
#include <string>

// installed beside this one, so included by the neighbour file name
#include "severity-macro-consts.h"

/**
 * @brief The implementation independent part of the logging subsystem
 * encapsulation namespace.
 */
namespace logger
{

/**
 * @brief The logger the library user implements, or takes from the
 * LOG_REAL_LOGGER of the cpp-app-template derived project, and hands over to
 * the library by the LibraryFacade::init_logger, so the library logs into it.
 *
 * It is the logging contract of every library derived from the
 * cpp-app-template, which is why it carries neither the library namespace nor
 * the library include guard: one application logger serves all of them. The
 * class layout is a binary interface between the application and the
 * libraries, so extend it by appending new methods after the existing ones
 * only.
 */
class ILogger
{
 public:
  using ILoggerPtr = std::shared_ptr<ILogger>;

  inline static constexpr const unsigned short LVL_ERROR = MACRO_LVL_ERROR;
  inline static constexpr const unsigned short LVL_WARNING = MACRO_LVL_WARNING;
  inline static constexpr const unsigned short LVL_INFO = MACRO_LVL_INFO;
  inline static constexpr const unsigned short LVL_DEBUG = MACRO_LVL_DEBUG;
  inline static constexpr const unsigned short LVL_TRACE = MACRO_LVL_TRACE;

  virtual ~ILogger() = default;
  ILogger() = default;

  virtual void log(const unsigned short& loglvl, const std::string& msg) = 0;

  /// @brief The message of the source file and line, the __FILE__ and the
  /// __LINE__ values usually.
  virtual void log(const unsigned short& loglvl, const char* const filePath,
                   const int& fileLine, const std::string& msg) = 0;

  /// @brief Appends the messages to the file, nothing for the empty path.
  virtual void logfile(const std::string& filepath) = 0;

  /// @brief Duplicates the messages into the standard output streams.
  virtual void print(const bool toPrintValue) = 0;

  /// @brief Discards the messages of the levels above the given one.
  virtual void level(const unsigned short& nlvl) = 0;

  virtual const std::string& lvl_repr(const unsigned short& glvl) = 0;

  virtual void init(const std::string& filepath, const unsigned short& nlvl,
                    const bool toPrintValue) = 0;
};

using ILoggerPtr = ILogger::ILoggerPtr;

}  // namespace logger

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_ILOGGER_CLASS_H
