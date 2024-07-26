#ifndef COTTON_INIT_LOG_H
#define COTTON_INIT_LOG_H

int log_getLevel();
void log_setLevel(int level);

void log_debug(const char* tag, const char* text, ...);
void log_info(const char* tag, const char* text, ...);
void log_warn(const char* tag, const char* text, ...);
void log_error(const char* tag, const char* text, ...);

#endif