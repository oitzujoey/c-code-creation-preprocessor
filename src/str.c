
#include "str.h"
#include <string.h>
#include "insane.h"

string_t str_stringify(char *string) {
	return (string_t) {.value = string, .length = strlen(string)};
}

// Only needs to be called when YOU or ANOTHER function allocates memory.
void str_free(string_t *string) {
	insane_free(string->value);
	string->length = 0;
}
