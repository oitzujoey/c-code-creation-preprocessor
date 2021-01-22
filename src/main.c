
#include <stdio.h>
#include <string.h>
#include <lualib.h>
#include <lauxlib.h>
#include <lua.h>
#include "log.h"
#include "str.h"
#include "insane.h"

const char *luaError[] = {
	"LUA_OK",
	"LUA_YIELD",
	"LUA_ERRRUN",
	"LUA_ERRSYNTAX",
	"LUA_ERRMEM",
	"LUA_ERRERR"
};

string_t getFileText(FILE *file) {
	
	char c;
	string_t str = {.value = NULL, .length = 0};
	size_t length = 1;
	
	while (1) {
		c = fgetc(file);
		if (c == EOF)
			break;
		
		str.length++;
		if (str.length >= length) {
			length *= 2;
			str.value = realloc(str.value, length * sizeof(char));
		}
		str.value[str.length - 1] = c;
	}
	str.value[str.length] = '\0';
	
	return str;
}

uint32_t calculateChecksum(fragment_t fragment) {
	// Adler-32
	const uint32_t prime = 65521;
	uint32_t a = 1, b = 0;
	for (int i = 0; i < fragment.generated.length; i++) {
		a += fragment.generated.value[i];
		// Could also replace with modulo.
		if (a >= prime) {
			a -= prime;
		}
		b += a;
		if (b >= prime) {
			b -= prime;
		}
	}
	
	return (b << 16) + a;
}

fragment_t getFragment(string_t *fileText, fragment_t fragment) {
	int error = 1;
	
	const string_t cccp_generator = str_stringify("/*cccp generate\n");
	const string_t endComment = str_stringify("*/");
	const string_t cccp_begin = str_stringify("/*cccp begin*/");
	const string_t cccp_end = str_stringify("/*cccp end ");
	char tempChar;
	
	// fragment_t fragment;
	// memset(&fragment, 0, sizeof(fragment_t));
	
	if (fragment.pointer == NULL) {
		fragment.pointer = fileText->value;
		fragment.fileText = fileText;
	}
	
	// Find next fragment.
	while (1) {
		if (cccp_generator.length > fileText->value + fileText->length - fragment.pointer) {
			error = 1;
			// No more fragments.
			goto cleanup;
		}
		// Kinda slow.
		if (!strncmp(fragment.pointer, cccp_generator.value, cccp_generator.length)) {
			// Found a fragment.
			fragment.text.value = fragment.pointer;
			fragment.pointer += cccp_generator.length;
			break;
		}
		fragment.pointer++;
	}
	
	// Get generator code.
	fragment.generator.value = fragment.pointer;
	
	// Find end of generator code.
	while (1) {
		if (endComment.length > fileText->value + fileText->length - fragment.pointer) {
			error("Bad fragment declaration. Unfinished generator definition.", "");
			error = 1;
			goto cleanup;
		}
		// Kinda slow.
		if (!strncmp(fragment.pointer, endComment.value, endComment.length)) {
			// Found a fragment.
			fragment.generator.length = fragment.pointer - fragment.generator.value;
			fragment.pointer += endComment.length;
			break;
		}
		fragment.pointer++;
	}
	
	// Get generated code.
	while (1) {
		if (cccp_begin.length > fileText->value + fileText->length - fragment.pointer) {
			error("Bad fragment declaration. No begin statement.", "");
			error = 1;
			goto cleanup;
		}
		// Kinda slow.
		if (!strncmp(fragment.pointer, cccp_begin.value, cccp_begin.length)) {
			// Found a fragment.
			fragment.pointer += cccp_begin.length;
			break;
		}
		fragment.pointer++;
	}
	fragment.generated.value = fragment.pointer;
	
	// Get checksum.
	while (1) {
		if (cccp_end.length > fileText->value + fileText->length - fragment.pointer) {
			error("Bad fragment declaration. No end statement.", "");
			error = 1;
			goto cleanup;
		}
		// Kinda slow.
		if (!strncmp(fragment.pointer, cccp_end.value, cccp_end.length)) {
			// Found a fragment.
			fragment.generated.length = fragment.pointer - fragment.generated.value;
			fragment.pointer += cccp_end.length;
			break;
		}
		fragment.pointer++;
	}
	
	fragment.checksumText.value = fragment.pointer;
	
	while (1) {
		if (endComment.length > fileText->value + fileText->length - fragment.pointer) {
			error("Bad fragment declaration. Unfinished generator definition.", "");
			error = 1;
			goto cleanup;
		}
		// Kinda slow.
		if (!strncmp(fragment.pointer, endComment.value, endComment.length)) {
			// Found a fragment.
			fragment.checksumText.length = fragment.pointer - fragment.checksumText.value;
			fragment.pointer += endComment.length;
			break;
		}
		fragment.pointer++;
	}
	
	fragment.text.length = fragment.pointer - fragment.text.value;
	
	if (fragment.checksumText.length == 0) {
		fragment.hasChecksum = 0;
	}
	else if (fragment.checksumText.length > 8) {
		fragment.hasChecksum = 0;
	}
	else {
		// Doesn't have a null, so add one.
		tempChar = fragment.checksumText.value[fragment.checksumText.length];
		fragment.checksumText.value[fragment.checksumText.length] = '\0';
		// Convert the string to a hex number.
		fragment.checksum = strtoul(fragment.checksumText.value, NULL, 16);
		// And replace the null with the original character.
		fragment.checksumText.value[fragment.checksumText.length] = tempChar;
		
		fragment.hasChecksum = 1;
	}
	
	// Calculate checksum.
	fragment.checksumCalculated = calculateChecksum(fragment);
	
	error = 0;
	cleanup:
	
	fragment.error = error;
	return fragment;
}

char hexChar(uint8_t byte) {
	uint8_t tempByte;
	tempByte = (byte & 0x0F) + '0';
	if (tempByte > '9') {
		tempByte = tempByte + 'A' - '9' - 1;
	}
	return tempByte;
}

int executeFragment(fragment_t *fragment) {
	int error = 1;
	
	char tempChar;
	lua_State *luaState = luaL_newstate();
	int top;
	string_t luaString = {.value = NULL, .length = 0};
	char *source;
	char *destination;
	char *newFileText;
	
	luaL_openlibs(luaState);
	
	// for (i = 0; cfuncs[i].name != NULL; i++) {
	//     lua_pushcfunction(*Lua, cfuncs[i].func);
	//     lua_setglobal(*Lua, cfuncs[i].name);
	// }
	
	tempChar = fragment->generator.value[fragment->generator.length - 1];
	fragment->generator.value[fragment->generator.length - 1] = '\0';
	error = luaL_loadstring(luaState, fragment->generator.value);
	fragment->generator.value[fragment->generator.length - 1] = tempChar;
	if (error) {
		error("Could not load string into Lua state. Returned %s", luaError[error]);
		error = 1;
		goto cleanup;
	}
	
	error = lua_pcall(luaState, 0, 1, 0);
	if (error) {
		error("Lua exited with errors. Returned %s", luaError[error]);
		error = 1;
		goto cleanup;
	}
	
	top = lua_gettop(luaState);
	if (top != 1) {
		error("Lua returned %i variables.", top);
		error = 1;
		goto cleanup;
	}
	
	// BE VERY CAREFUL WITH THIS. THIS IS A CONSTANT VALUE.
	luaString = str_stringify((char *) lua_tostring(luaState, top));
	
	// Make room for the generated string.
	if (luaString.length > fragment->generated.length) {
		fragment->fileText->length += luaString.length - fragment->generated.length;
		
		newFileText = realloc(fragment->fileText->value, (fragment->fileText->length + 1) * sizeof(char));
		if (newFileText == NULL) {
			critical_error("Out of memory.", "");
			error = 1;
			goto cleanup;
		}
		fragment->checksumText.value = newFileText + (fragment->checksumText.value - fragment->fileText->value);
		fragment->generated.value = newFileText + (fragment->generated.value - fragment->fileText->value);
		fragment->fileText->value = newFileText;
		
		// Move the file text after the generated area to the end of the string.
		destination = fragment->fileText->value + fragment->fileText->length;
		source = destination - (luaString.length - fragment->generated.length);
		while (source >= fragment->generated.value + fragment->generated.length) {
			*destination = *source;
			--source;
			--destination;
		}
		
		// Move the generated text into the file text.
		destination = fragment->generated.value;
		source = luaString.value;
		for (int i = 0; i < luaString.length; i++) {
			destination[i] = source[i];
		}
	}
	else {
		// Move the generated text into the file text.
		destination = fragment->generated.value;
		source = luaString.value;
		for (int i = 0; i < luaString.length; i++) {
			destination[i] = source[i];
		}
		
		// Shrink the file text.
		source = fragment->generated.value + fragment->generated.length;
		destination = fragment->generated.value + luaString.length;
		while (source <= fragment->fileText->value + fragment->fileText->length) {
			*destination = *source;
			destination++;
			source++;
		}
		fragment->fileText->length = fragment->fileText->length + luaString.length - fragment->generated.length;
		fragment->fileText->value[fragment->fileText->length] = '\0';
	}
	fragment->checksumText.value += luaString.length - fragment->generated.length;
	
	fragment->generated.length = luaString.length;
	
	// Add checksum.
	if (fragment->hasChecksum) {
		fragment->checksumCalculated = calculateChecksum(*fragment);
		
		for (int i = 0; i < 2 * sizeof(uint32_t); i++) {
			fragment->checksumText.value[i] = hexChar((fragment->checksumCalculated) >> (4 * (2 * sizeof(uint32_t) - (i + 1))));
		}
	}
	else if (fragment->checksumText.length < 8) {
		
		fragment->fileText->length += 2 * sizeof(uint32_t) - fragment->checksumText.length;
		
		newFileText = realloc(fragment->fileText->value, (fragment->fileText->length + 1) * sizeof(char));
		if (newFileText == NULL) {
			critical_error("Out of memory.", "");
			error = 1;
			goto cleanup;
		}
		fragment->checksumText.value = newFileText + (fragment->checksumText.value - fragment->fileText->value);
		fragment->generated.value = newFileText + (fragment->generated.value - fragment->fileText->value);
		fragment->fileText->value = newFileText;
		
		fragment->checksumText.length = 2 * sizeof(uint32_t);
		
		// Move the file text after the checksum area to the end of the string.
		destination = fragment->fileText->value + fragment->fileText->length;
		source = destination - fragment->checksumText.length;
		while (source >= fragment->checksumText.value) {
			*destination = *source;
			--source;
			--destination;
		}
		
		fragment->checksumCalculated = calculateChecksum(*fragment);
		
		for (int i = 0; i < 2 * sizeof(uint32_t); i++) {
			fragment->checksumText.value[i] = hexChar((fragment->checksumCalculated) >> (4 * (2 * sizeof(uint32_t) - (i + 1))));
		}
	}
	// length > 8
	else {
		// Move the generated text into the file text.
		destination = fragment->checksumText.value;
		source = fragment->checksumText.value;
		for (int i = 0; i < luaString.length; i++) {
			destination[i] = source[i];
		}
		
		// Shrink the file text.
		source = fragment->checksumText.value + fragment->checksumText.length;
		destination = fragment->checksumText.value + 2 * sizeof(uint32_t);
		while (source <= fragment->fileText->value + fragment->fileText->length) {
			*destination = *source;
			destination++;
			source++;
		}
		fragment->fileText->length = fragment->fileText->length + 2 * sizeof(uint32_t) - fragment->checksumText.length;
		fragment->fileText->value[fragment->fileText->length] = '\0';
		
		fragment->checksumCalculated = calculateChecksum(*fragment);
		
		for (int i = 0; i < 2 * sizeof(uint32_t); i++) {
			fragment->checksumText.value[i] = hexChar((fragment->checksumCalculated) >> (4 * (2 * sizeof(uint32_t) - (i + 1))));
		}
	}
	
	fragment->pointer = fragment->generated.value + luaString.length;
	
	error = 0;
	cleanup:
	
	lua_close(luaState);

	return error;
}

int main(int argc, char *argv[]) {
	int error = 1;
	
	FILE *file;
	string_t fileText = {.value = NULL, .length = 0};
	fragment_t fragment;
	
	if (argc == 1) {
		info("Usage: cccp <file1.c> <file2.c> ...", "");
		error = 0;
		goto cleanup;
	}
	
	for (int fileNumber = 1; fileNumber < argc; fileNumber++) {
	
		info("Processing \"%s\".", argv[fileNumber]);
		
		memset(&fragment, 0, sizeof(fragment_t));
		
		// Open file.
		file = fopen(argv[fileNumber], "r");
		if (file == NULL) {
			critical_error("Could not open file \"%s\".", argv[fileNumber]);
			error = 1;
			goto cleanup;
		}
		
		// Get file text.
		fileText = getFileText(file);
		fclose(file);
		if (fileText.value == NULL) {
			error("Could not read file text.", "");
			error = 1;
			break;
		}
		
		for (int fragmentNumber = 1;; fragmentNumber++) {
			// Get CCCP fragment.
			fragment = getFragment(&fileText, fragment);
			if (fragment.error != 0) {
				// We're done with this file.
				break;
			}
			
			// Check if generated code was edited since last generation.
			if (fragment.hasChecksum && (fragment.checksum != fragment.checksumCalculated)) {
				// Issue a warning and skip this fragment.
				error("Bad checksum in fragment %i of file \"%s\". To override this error, undo your edits to the generated code, or delete the checksum.",
					fragmentNumber, argv[fileNumber]);
				continue;
			}
			
			// Generate the code.
			if (executeFragment(&fragment)) {
				error("Could not generate fragment %i.", fragmentNumber);
				continue;
			}
		}
		
		// Write back to file.
		error = remove(argv[fileNumber]);
		if (error) {
			critical_error("Could not delete file \"%s\".", argv[fileNumber]);
			error = 1;
			goto cleanup;
		}
		file = fopen(argv[fileNumber], "w");
		if (file == NULL) {
			critical_error("Could not create file \"%s\".", argv[fileNumber]);
		}
		fwrite(fileText.value, sizeof(char), fileText.length, file);
		fclose(file);
		str_free(&fileText);
	}
	
	error = 0;
	cleanup:
	
	return error;
}
