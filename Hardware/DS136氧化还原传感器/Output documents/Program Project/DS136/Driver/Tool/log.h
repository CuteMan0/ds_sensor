/**
 * @file log.h
 * @author wongsx
 * @brief
 * @version 0.1
 * @date 2025-09-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __LOG_H
#define __LOG_H

#include "stc32_stc8_usb.h" // putchar内部接口头文件

#define __LOG 1       // 总开关 (0 = 全部关闭)
#define __LOG_LEVEL 3 // 日志等级：0=ERROR, 1=WARN, 2=INFO, 3=DEBUG

#if __LOG

// -------------------- 日志等级裁剪 --------------------
#if __LOG_LEVEL >= 0
#define LOG_ERROR log_error(__FILE__, __LINE__,
#else
#define LOG_ERROR log_null(
#endif

#if __LOG_LEVEL >= 1
#define LOG_WARN  log_warn(__FILE__, __LINE__,
#else
#define LOG_WARN  log_null(
#endif

#if __LOG_LEVEL >= 2
#define LOG_INFO  log_info(__FILE__, __LINE__,
#else
#define LOG_INFO  log_null(
#endif

#if __LOG_LEVEL >= 3
#define LOG_DEBUG log_debug(__FILE__, __LINE__,
#else
#define LOG_DEBUG log_null(
#endif
// -------------------------------------------------------

#else // __LOG = 0
#define LOG_ERROR log_null(
#define LOG_WARN  log_null(
#define LOG_INFO  log_null(
#define LOG_DEBUG log_null(
#endif

// ------------------ 声明 ------------------
void log_error(const char *file, int line, const char *fmt, ...);
void log_warn(const char *file, int line, const char *fmt, ...);
void log_info(const char *file, int line, const char *fmt, ...);
void log_debug(const char *file, int line, const char *fmt, ...);

void log_null(const char *fmt, ...);

char putchar(char c);

#endif // __LOG_H
