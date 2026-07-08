/**
 * @file log.c
 * @author wongsx
 * @brief
 * @version 0.1
 * @date 2025-09-22
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "log.h"

#include <stdio.h>
#include <stdarg.h>

#define __log_color_set(clr) printf(clr)
#define __log_color_reset() printf("\033[0m")

#define BLACK "\033[1;30m"
#define RED "\033[1;31m"
#define GREEN "\033[1;32m"
#define YELLOW "\033[1;33m"
#define BLUE "\033[1;34m"
#define PURPLE "\033[1;35m"
#define CYAN "\033[1;36m"
#define WHITE "\033[1;37m"

char putchar(char c)
{
    USB_SendData(&c, 1);
    return c;
}

static void log_vprint(const char *color,
                       const char *level,
                       const char *file,
                       int line,
                       const char *fmt,
                       va_list args)
{
    __log_color_set(color);
    printf("[%s] %s:%d ", level, file, line);
    vprintf(fmt, args);
    __log_color_reset();
    printf("\n");
}

void log_error(const char *file, int line, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_vprint(RED, "ERROR", file, line, fmt, args);
    va_end(args);
}

void log_warn(const char *file, int line, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_vprint(YELLOW, "WARN", file, line, fmt, args);
    va_end(args);
}

void log_info(const char *file, int line, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_vprint(GREEN, "INFO", file, line, fmt, args);
    va_end(args);
}

void log_debug(const char *file, int line, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    log_vprint(CYAN, "DEBUG", file, line, fmt, args);
    va_end(args);
}

void log_null(const char *fmt, ...) {}
