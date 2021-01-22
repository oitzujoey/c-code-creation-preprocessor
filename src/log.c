
#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "insane.h"

// #define LOG_FLUSH

void log_info(const char *function, const char *fmt, ...) {
	va_list va;
	
	const char *infomessage = COLOR_GREEN"Info: "COLOR_BLUE"(%s)"COLOR_NORMAL" %s\n";
	char *buf = malloc((strlen(infomessage) + strlen(function) + strlen(fmt) - 4 + 1) * sizeof(char));
	sprintf(buf, infomessage, function, fmt);
	
	va_start(va, fmt);
	vprintf(buf, va);
	va_end(va);
	
	free(buf);
	
	#ifdef LOG_FLUSH
	fflush(stdout);
	#endif
}

void log_warning(const char *function, const char *fmt, ...) {
	va_list va;
	
	const char *infomessage = COLOR_MAGENTA"Warning: "COLOR_BLUE"(%s)"COLOR_NORMAL" %s\n";
	char *buf = malloc((strlen(infomessage) + strlen(function) + strlen(fmt) - 4 + 1) * sizeof(char));
	sprintf(buf, infomessage, function, fmt);
	
	va_start(va, fmt);
	vfprintf(stderr, buf, va);
	va_end(va);
	
	free(buf);
	
	#ifdef LOG_FLUSH
	fflush(stderr);
	#endif
}

void log_error(const char *function, const char *fmt, ...) {
	va_list va;
	
	const char *infomessage = COLOR_YELLOW"Error: "COLOR_BLUE"(%s)"COLOR_NORMAL" %s\n";
	char *buf = malloc((strlen(infomessage) + strlen(function) + strlen(fmt) - 4 + 1) * sizeof(char));
	sprintf(buf, infomessage, function, fmt);
	
	va_start(va, fmt);
	vfprintf(stderr, buf, va);
	va_end(va);
	
	free(buf);
	
	#ifdef LOG_FLUSH
	fflush(stderr);
	#endif
}

void log_critical_error(const char *function, const char *fmt, ...) {
	va_list va;
	
	const char *infomessage = COLOR_RED"Critical error: "COLOR_BLUE"(%s)"COLOR_NORMAL" %s\n";
	char *buf = malloc((strlen(infomessage) + strlen(function) + strlen(fmt) - 4 + 1) * sizeof(char));
	sprintf(buf, infomessage, function, fmt);
	
	va_start(va, fmt);
	vfprintf(stderr, buf, va);
	va_end(va);
	
	free(buf);
	
	#ifdef LOG_FLUSH
	fflush(stderr);
	#endif
}

// int l_log_info(lua_State *l) {
// 	int error = ERR_CRITICAL;

// 	const char *function = NULL;
// 	const char *message = NULL;
// 	char *buf = NULL;
	
// 	if (!lua_isstring(l, 1) || !lua_isstring(l, 2)) {
// 		error("Arguments must be strings", "");
// 		error = ERR_GENERIC;
// 		goto cleanup_l;
// 	}
	
// 	function = lua_tostring(l, 1);
// 	message = lua_tostring(l, 2);

// 	const char *infomessage = COLOR_CYAN"Lua "COLOR_GREEN"Info: "COLOR_BLUE"(%s)"COLOR_NORMAL" %s\n";
// 	buf = malloc((strlen(infomessage) + strlen(function) + strlen(message) - 4 + 1) * sizeof(char));
// 	sprintf(buf, infomessage, function, message);
	
// 	printf(buf);
	
// 	error = ERR_OK;
// 	cleanup_l:
	
// 	insane_free(buf);
	
// 	if (error) {
// 		lua_error(l);
// 	}
	
// 	return 0;
// }

// int l_log_warning(lua_State *l) {
// 	const char *function = lua_tostring(l, 1);
// 	const char *message = lua_tostring(l, 2);

// 	const char *infomessage = COLOR_CYAN"Lua "COLOR_GREEN"Warning: "COLOR_BLUE"(%s)"COLOR_NORMAL" %s\n";
// 	char *buf = malloc((strlen(infomessage) + strlen(function) + strlen(message) - 4 + 1) * sizeof(char));
// 	sprintf(buf, infomessage, function, message);
	
// 	fprintf(stderr, buf);
	
// 	free(buf);
	
// 	return 0;
// }

// int l_log_error(lua_State *l) {
// 	const char *function = lua_tostring(l, 1);
// 	const char *message = lua_tostring(l, 2);

// 	const char *infomessage = COLOR_CYAN"Lua "COLOR_GREEN"Error: "COLOR_BLUE"(%s)"COLOR_NORMAL" %s\n";
// 	char *buf = malloc((strlen(infomessage) + strlen(function) + strlen(message) - 4 + 1) * sizeof(char));
// 	sprintf(buf, infomessage, function, message);
	
// 	fprintf(stderr, buf);
	
// 	free(buf);
	
// 	return 0;
// }

// int l_log_critical_error(lua_State *l) {
// 	const char *function = lua_tostring(l, 1);
// 	const char *message = lua_tostring(l, 2);

// 	const char *infomessage = COLOR_CYAN"Lua "COLOR_GREEN"Critical error: "COLOR_BLUE"(%s)"COLOR_NORMAL" %s\n";
// 	char *buf = malloc((strlen(infomessage) + strlen(function) + strlen(message) - 4 + 1) * sizeof(char));
// 	sprintf(buf, infomessage, function, message);
	
// 	fprintf(stderr, buf);
	
// 	free(buf);
	
// 	return 0;
// }
