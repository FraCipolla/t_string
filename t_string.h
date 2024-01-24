#pragma once

# include <string.h>
# include <stdlib.h>

#define SMALL_CHUNK 128
#define MEDIUM_CHUNK 512
#define BIG_CHUNK 2048

enum STR_ERR {
	NO_ERR = 0,
	OUT_OF_BOUND,
	HEAP_ALLOC_ERROR,
	TRUNCATED,
};

typedef struct s_string {
	size_t size;
	size_t capacity;
	int error;
	char buffer[];
}	t_string;

t_string *string_init(char *str) {
	size_t len = strlen(str);
	size_t to_alloc = len < SMALL_CHUNK ? SMALL_CHUNK : len < MEDIUM_CHUNK ? MEDIUM_CHUNK : len < BIG_CHUNK ? BIG_CHUNK : len + SMALL_CHUNK;
	t_string *s = (t_string *)malloc(sizeof(t_string) + to_alloc + 1);
	if (!s) {
		return &(t_string){.error=HEAP_ALLOC_ERROR};
	}
	s->capacity = to_alloc;
	s->size = len;
	for (size_t i = 0; i < len; i++) {
		s->buffer[i] = str[i];
	}
	s->buffer[len] = 0;
	return s;
}

#define String string_init
typedef t_string * string;

void clear(string s) { memset(s->buffer, 0, s->size); s->size = 0; s->error=NO_ERR; }

t_string *strcpy_s_c(t_string *dest, const char *search)
{
	const size_t len = strlen(search);
	if (len >= dest->capacity) {
		// emit warning
		dest->error = TRUNCATED;
	}
	size_t i = 0;
	while (i < dest->capacity && search[i]) {
		dest->buffer[i] = search[i];
		++i;
	}
	if (i > dest->size) {
		dest->size = i;
		dest->buffer[dest->size] = 0;
	}
	return dest;
}

t_string *strcpy_s_s(t_string *dest, const t_string *search)
{
	if (search->size >= dest->capacity) {
		// emit warning
		dest->error = TRUNCATED;
	}
	int i = 0;
	while (i < dest->capacity && i < search->size) {
		dest->buffer[i] = search->buffer[i];
		++i;
	}
	if (i > dest->size) {
		dest->size = i;
		dest->buffer[dest->size] = 0;
	}
	return dest;
}

t_string *strcat_s_c(t_string *dest, const char *search)
{
	const size_t len = strlen(search);
	if (dest->size + len >= dest->capacity) {
		// emit warning
		dest->error = TRUNCATED;
	}
	int i = 0;
	while (dest->size < dest->capacity && search[i]) {
		dest->buffer[dest->size++] = search[i++];
	}
	dest->buffer[dest->size] = 0;
	return dest;
}

t_string *strcat_s_s(t_string *dest, const t_string *search)
{
	if (dest->size + search->size >= dest->capacity) {
		// emit warning
		dest->error = TRUNCATED;
	}
	int i = 0;
	while (dest->size < dest->capacity && search->buffer[i] && i < search->size) {
		dest->buffer[dest->size++] = search->buffer[i++];
	}
	dest->buffer[dest->size] = 0;
	return dest;
}

size_t length_ptr(t_string *s) { return s->size; }
//size_t length(t_string s) { return s.size; }



#define strlen(a) _Generic((a), t_string * : length_ptr, default : strlen) (a)
#define strcpy(a, b) _Generic((a), string : _Generic(b, string : strcpy_s_s, char * : strcpy_s_c), default : strcpy) (a, b)
#define strcat(a, b) _Generic((a), string : _Generic(b, string : strcat_s_s, char * : strcat_s_c), default : strcat) (a, b)