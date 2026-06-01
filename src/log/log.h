#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_LOGGER_SUBSYSTEM_DECLARATIONS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_LOGGER_SUBSYSTEM_DECLARATIONS_H

#include <sstream>

#include "src/log/severity-macro-consts.h"
#include "src/log/default-logger/DefaultLogger.h"

#ifndef LOG_INIT
/**
 * @brief The logging init macros. Use them in the main function or
 * as by default is used in the app::ApplicationFactory::execute method.
 */
#define LOG_INIT(filepath, logLvl, printMessages) \
  default_logger::DefaultLogger::init(filepath, logLvl, printMessages);
#endif  // LOG_INIT

#ifndef LOG_INIT_PATH
#define LOG_INIT_PATH(filepath) default_logger::DefaultLogger::init(filepath);
#endif  // LOG_INIT_PATH

#ifndef LOG_INIT_DEFAULTS
#define LOG_INIT_DEFAULTS() default_logger::DefaultLogger::init();
#endif  // LOG_INIT_DEFAULTS

#ifndef LOG_BODY
/**
 * @brief The internal logger macro to define the general logging code body.
 */
#define LOG_BODY(LOGLVL, msg)                                    \
  {                                                              \
    std::stringstream logMessageContainer;                       \
    logMessageContainer << msg;                                  \
    default_logger::DefaultLogger::log(LOGLVL, __FILE__, __LINE__, \
                                     logMessageContainer.str()); \
  }
#endif  // LOG_BODY

#ifndef LOGE
/**
 * @brief Perform the error logging.
 *
 * @param msg The logging message which may use the << operator
 * and each of the log elements MUST be converted into the std::string.
 */
#define LOGE(msg) LOG_BODY(default_logger::DefaultLogger::LVL_ERROR, msg)
#endif  // LOGE

#ifndef LOGI
#if MAX_LOG_LEVEL >= MACRO_LOG_LEVEL_INFO
/**
 * @brief Perform the info logging.
 *
 * @param msg The logging message which may use the << operator
 * and each of the log elements MUST be converted into the std::string.
 */
#define LOGI(msg) LOG_BODY(default_logger::DefaultLogger::LVL_INFO, msg)
#else
#define LOGI(msg)
#endif  // MAX_LOG_LEVEL
#endif  // LOGI

#ifndef LOGW
#if MAX_LOG_LEVEL >= MACRO_LOG_LEVEL_WARNING
/**
 * @brief Perform the warning logging.
 *
 * @param msg The logging message which may use the << operator
 * and each of the log elements MUST be converted into the std::string.
 */
#define LOGW(msg) LOG_BODY(default_logger::DefaultLogger::LVL_WARNING, msg)
#else
#define LOGW(msg)
#endif  // MAX_LOG_LEVEL
#endif  // LOGW

#ifndef LOGD
#if MAX_LOG_LEVEL >= MACRO_LOG_LEVEL_DEBUG
/**
 * @brief Perform the debug logging.
 *
 * @param msg The logging message which may use the << operator
 * and each of the log elements MUST be converted into the std::string.
 */
#define LOGD(msg) LOG_BODY(default_logger::DefaultLogger::LVL_DEBUG, msg)
#else
#define LOGD(msg)
#endif  // MAX_LOG_LEVEL
#endif  // LOGD

#ifndef LOGT
#if MAX_LOG_LEVEL >= MACRO_LOG_LEVEL_TRACE
/**
 * @brief Perform the trace logging.
 *
 * @param msg The logging message which may use the << operator
 * and each of the log elements MUST be converted into the std::string.
 */
#define LOGT(msg) LOG_BODY(default_logger::DefaultLogger::LVL_TRACE, msg)
#else
#define LOGT(msg)
#endif  // MAX_LOG_LEVEL
#endif  // LOGT

#ifndef FMTLOGI
/**
 * @brief The internal logger macro to define the general logging code body.
 */
#define FMTLOGI(fmt, ...)                                             \
  {                                                                   \
    default_logger::DefaultLogger::log(                                 \
        default_logger::DefaultLogger::LVL_INFO, __FILE__, __LINE__,    \
        default_logger::DefaultLogger::prepare_buff(fmt, __VA_ARGS__)); \
  }
#endif  // FMTLOGI

#ifndef LOG_BODY_EXT
/**
 * @brief The logger macro to define the extended logging code body.
 */
#define LOG_BODY_EXT(FILE_STR, LINE_INT, LOGLVL, msg)            \
  {                                                              \
    std::stringstream logMessageContainer;                       \
    logMessageContainer << msg;                                  \
    default_logger::DefaultLogger::log((LOGLVL), (FILE_STR),       \
                                     static_cast<int>(LINE_INT), \
                                     logMessageContainer.str()); \
  }
#endif  // LOG_BODY

#ifndef ELOGE
/**
 * @brief Perform the error logging.
 *
 * @param msg The logging message which may use the << operator
 * and each of the log elements MUST be converted into the std::string.
 */
#define ELOGE(FILE_STR, LINE_INT, msg)                                         \
  LOG_BODY_EXT((FILE_STR), (LINE_INT), default_logger::DefaultLogger::LVL_ERROR, \
               msg)
#endif  // ELOGE

#ifndef ELOGI
#if MAX_LOG_LEVEL >= MACRO_LOG_LEVEL_INFO
/**
 * @brief Perform the info logging.
 *
 * @param msg The logging message which may use the << operator
 * and each of the log elements MUST be converted into the std::string.
 */
#define ELOGI(FILE_STR, LINE_INT, msg)                                        \
  LOG_BODY_EXT((FILE_STR), (LINE_INT), default_logger::DefaultLogger::LVL_INFO, \
               msg)
#else
#define ELOGI(FILE_STR, LINE_INT, msg)
#endif  // MAX_LOG_LEVEL
#endif  // ELOGI

#ifndef ELOGW
#if MAX_LOG_LEVEL >= MACRO_LOG_LEVEL_WARNING
/**
 * @brief Perform the warning logging.
 *
 * @param msg The logging message which may use the << operator
 * and each of the log elements MUST be converted into the std::string.
 */
#define ELOGW(FILE_STR, LINE_INT, msg) \
  LOG_BODY_EXT((FILE_STR), (LINE_INT), \
               default_logger::DefaultLogger::LVL_WARNING, msg)
#else
#define ELOGW(FILE_STR, LINE_INT, msg)
#endif  // MAX_LOG_LEVEL
#endif  // ELOGW

#ifndef ELOGD
#if MAX_LOG_LEVEL >= MACRO_LOG_LEVEL_DEBUG
/**
 * @brief Perform the debug logging.
 *
 * @param msg The logging message which may use the << operator
 * and each of the log elements MUST be converted into the std::string.
 */
#define ELOGD(FILE_STR, LINE_INT, msg)                                         \
  LOG_BODY_EXT((FILE_STR), (LINE_INT), default_logger::DefaultLogger::LVL_DEBUG, \
               msg)
#else
#define ELOGD(FILE_STR, LINE_INT, msg)
#endif  // MAX_LOG_LEVEL
#endif  // ELOGD

#ifndef ELOGT
#if MAX_LOG_LEVEL >= MACRO_LOG_LEVEL_TRACE
/**
 * @brief Perform the trace logging.
 *
 * @param msg The logging message which may use the << operator
 * and each of the log elements MUST be converted into the std::string.
 */
#define ELOGT(FILE_STR, LINE_INT, msg)                                         \
  LOG_BODY_EXT((FILE_STR), (LINE_INT), default_logger::DefaultLogger::LVL_TRACE, \
               msg)
#else
#define ELOGT(FILE_STR, LINE_INT, msg)
#endif  // MAX_LOG_LEVEL
#endif  // ELOGT

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_LOGGER_SUBSYSTEM_DECLARATIONS_H
