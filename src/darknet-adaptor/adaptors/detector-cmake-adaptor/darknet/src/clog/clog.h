#ifndef DARKNET_CLOG_C_FUNCTION_H
#define DARKNET_CLOG_C_FUNCTION_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void clogi(const char* file, const size_t line, const char* fmt, ...);
void cloge(const char* file, const size_t line, const char* fmt, ...);
void clogw(const char* file, const size_t line, const char* fmt, ...);
void clogi(const char* file, const size_t line, const char* fmt, ...);
void clogd(const char* file, const size_t line, const char* fmt, ...);
void clogt(const char* file, const size_t line, const char* fmt, ...);

void clogim(const char* file, const size_t line, const char* msg);
void clogem(const char* file, const size_t line, const char* msg);
void clogwm(const char* file, const size_t line, const char* msg);
void clogim(const char* file, const size_t line, const char* msg);
void clogdm(const char* file, const size_t line, const char* msg);
void clogtm(const char* file, const size_t line, const char* msg);

#ifndef CLOGE
#define CLOGE(fmt, ...) cloge(__FILE__, __LINE__, fmt, __VA_ARGS__)
#endif

#ifndef CLOGW
#define CLOGW(fmt, ...) clogw(__FILE__, __LINE__, fmt, __VA_ARGS__)
#endif

#ifndef CLOGI
#define CLOGI(fmt, ...) clogi(__FILE__, __LINE__, fmt, __VA_ARGS__)
#endif

#ifndef CLOGD
#define CLOGD(fmt, ...) clogd(__FILE__, __LINE__, fmt, __VA_ARGS__)
#endif

#ifndef CLOGT
#define CLOGT(fmt, ...) clogt(__FILE__, __LINE__, fmt, __VA_ARGS__)
#endif

#ifndef CLOGEM
#define CLOGEM(msg) clogem(__FILE__, __LINE__, msg)
#endif

#ifndef CLOGWM
#define CLOGWM(msg) clogwm(__FILE__, __LINE__, msg)
#endif

#ifndef CLOGIM
#define CLOGIM(msg) clogim(__FILE__, __LINE__, msg)
#endif

#ifndef CLOGDM
#define CLOGDM(msg) clogdm(__FILE__, __LINE__, msg)
#endif

#ifndef CLOGTM
#define CLOGTM(msg) clogtm(__FILE__, __LINE__, msg)
#endif

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_CLOG_C_FUNCTION_H
