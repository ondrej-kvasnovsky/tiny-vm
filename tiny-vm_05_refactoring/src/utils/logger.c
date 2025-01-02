// src/utils/logger.c
#include "logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <time.h>

void print(const char *format, ...) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);

    time_t t = ts.tv_sec;
    struct tm tm;
    localtime_r(&t, &tm);

    printf("[%04d-%02d-%02d %02d:%02d:%02d.%06ld] ",
        tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
        tm.tm_hour, tm.tm_min, tm.tm_sec, ts.tv_nsec / 1000);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    printf("\n");
    fflush(stdout);
}
