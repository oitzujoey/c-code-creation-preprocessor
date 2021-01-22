
#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <stdint.h>

typedef struct {
	char *value;
	size_t length;
} string_t;

typedef struct {
	int error;
	string_t *fileText;
	string_t text;
	string_t generator;
	string_t generated;
	string_t checksumText;
	uint32_t checksum;
	uint32_t checksumCalculated;
	int hasChecksum;
	char *pointer;
} fragment_t;

#endif
