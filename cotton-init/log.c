#include <stdio.h>
#include <stdarg.h>

#include <time.h>

#include <math.h>

static int log_level = 3;

struct timespec getUpTime() {
    struct timespec uptime;
    clock_gettime(CLOCK_BOOTTIME, &uptime);
    return uptime;
}

int log_getLevel() {return log_level;}
void log_setLevel(int level) {log_level = level;}

void log_debug(const char* tag, const char* text, ...) {
    if(log_level < 4) return;
    va_list args;
    va_start(args, text);
    struct timespec uptime = getUpTime();
    printf("\e[38;2;180;171;171m[%5li.%06i] D %s: ", uptime.tv_sec, (int)(uptime.tv_nsec / powl(10, (((int)log10l(uptime.tv_nsec) + 1) - 6))), tag);
    vprintf(text, args);
    printf("\e[38;2;255;255;255m\n");
    va_end(args);
}

void log_info(const char* tag, const char* text, ...) {
    if(log_level < 3) return;
    va_list args;
    va_start(args, text);
    struct timespec uptime = getUpTime();
    printf("\e[38;2;0;255;0m[%5li.%06i] I %s: ", uptime.tv_sec, (int)(uptime.tv_nsec / powl(10, (((int)log10l(uptime.tv_nsec) + 1) - 6))), tag);
    vprintf(text, args);
    printf("\e[38;2;255;255;255m\n");
    va_end(args);
}

void log_warn(const char* tag, const char* text, ...) {
    if(log_level < 2) return;
    va_list args;
    va_start(args, text);
    struct timespec uptime = getUpTime();
    printf("\e[38;2;255;255;0m[%5li.%06i] W %s: ",  uptime.tv_sec, (int)(uptime.tv_nsec / powl(10, (((int)log10l(uptime.tv_nsec) + 1) - 6))), tag);
    vprintf(text, args);
    printf("\e[38;2;255;255;255m\n");
    va_end(args);
}

void log_error(const char* tag, const char* text, ...) {
    if(log_level < 1) return;
    va_list args;
    va_start(args, text);
    struct timespec uptime = getUpTime();
    printf("\e[38;2;255;0;0m[%5li.%06i] E %s: ", uptime.tv_sec, (int)(uptime.tv_nsec / powl(10, (((int)log10l(uptime.tv_nsec) + 1) - 6))), tag);
    vprintf(text, args);
    printf("\e[38;2;255;255;255m\n");
    va_end(args);

}