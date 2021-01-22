
#ifndef LOG_H
#define LOG_H

#define COLOR_NORMAL    "\x1B[0m"
#define COLOR_BLACK     "\x1B[30m"
#define COLOR_RED       "\x1B[31m"
#define COLOR_GREEN     "\x1B[32m"
#define COLOR_YELLOW    "\x1B[33m"
#define COLOR_BLUE      "\x1B[34m"
#define COLOR_MAGENTA   "\x1B[35m"
#define COLOR_CYAN      "\x1B[36m"
#define COLOR_WHITE     "\x1B[37m"

#define B_COLOR_BLACK     "\x1B[40m"
#define B_COLOR_RED       "\x1B[41m"
#define B_COLOR_GREEN     "\x1B[42m"
#define B_COLOR_YELLOW    "\x1B[43m"
#define B_COLOR_BLUE      "\x1B[44m"
#define B_COLOR_MAGENTA   "\x1B[45m"
#define B_COLOR_CYAN      "\x1B[46m"
#define B_COLOR_WHITE     "\x1B[47m"

void log_info(const char *function, const char *fmt, ...);
void log_warning(const char *function, const char *fmt, ...);
void log_error(const char *function, const char *fmt, ...);
void log_critical_error(const char *function, const char *fmt, ...);

#define info(fmt, ...) log_info(__func__, fmt, __VA_ARGS__)
#define warning(fmt, ...) log_warning(__func__, fmt, __VA_ARGS__)
#define error(fmt, ...) log_error(__func__, fmt, __VA_ARGS__)
#define critical_error(fmt, ...) log_critical_error(__func__, fmt, __VA_ARGS__)

// int l_log_info(lua_State *l);
// int l_log_warning(lua_State *l);
// int l_log_error(lua_State *l);
// int l_log_critical_error(lua_State *l);

#endif
