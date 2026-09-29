#ifndef DARKNETXX_PROJECT_LOGGER_SUBSYSTEM_INTERFACE_4_C_CODE_DECLARATIONS_H
#define DARKNETXX_PROJECT_LOGGER_SUBSYSTEM_INTERFACE_4_C_CODE_DECLARATIONS_H

#include <cstddef>

extern "C" {

/// @brief Returns non zero if the messages of the given level reach the log.
int cpplog_enabled(const unsigned int level);

void cpploge(const char* const file, const size_t line, const char* const buff);
void cpplogw(const char* const file, const size_t line, const char* const buff);
void cpplogi(const char* const file, const size_t line, const char* const buff);
void cpplogd(const char* const file, const size_t line, const char* const buff);
void cpplogt(const char* const file, const size_t line, const char* const buff);
}

#endif  // DARKNETXX_PROJECT_LOGGER_SUBSYSTEM_INTERFACE_4_C_CODE_DECLARATIONS_H
