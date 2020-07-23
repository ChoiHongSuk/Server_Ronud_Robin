#include <stdio.h>
#include <string.h>

#include <mem_string.h>

char* memstr(char* buffer, char* string, unsigned int size)
{
	char* ptr = NULL;
	unsigned int string_size = 0;
	int idx = 0;

	if ((buffer == NULL) || (size < 0) || (string == NULL)) {
		return NULL;
	}

	string_size = strlen(string);
	if (size < string_size) {
		return NULL;
	}

	ptr = buffer;

	for (idx = 0; idx < size; idx++) {
		if (memcmp(ptr, string, string_size) == 0) {
			return ptr;
		}
		ptr++;
	}
	return NULL;
}

char* memstrr(char* buffer, char* string, unsigned int size)
{
	char* ptr = NULL;
	unsigned int string_size = 0, buffer_size = 0;
	int idx = 0;

	if ((buffer == NULL) || (size < 0) || (string == NULL)) {
		return NULL;
	}

	buffer_size = strlen(buffer);
	string_size = strlen(string);
	if (size < string_size) {
		return NULL;
	}
	if (buffer_size < string_size) {
		return NULL;
	}
	ptr = buffer;
	ptr = ptr + buffer_size - 1;

	for (idx = 0; idx < size; idx++) {
		if (memcmp(ptr, string, string_size) == 0) {
			return ptr;
		}
		ptr--;
	}
	return NULL;
}
