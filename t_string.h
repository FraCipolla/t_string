#pragma once

# include <string.h>
# include <stdlib.h>
# include <stdio.h>

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

typedef struct string_container {
	size_t size;
	size_t capacity;
	int error;
}	capacity;

typedef char * iterator;

typedef struct d_string {
	capacity cap;
	iterator begin;
	iterator end;
	char buffer[];
}	dstring;

dstring *init (char *str) {
	dstring *ret = malloc(sizeof(dstring) + SMALL_CHUNK);
	strcpy(ret->buffer, str);
	// ret->it = (iterator){.begin=&ret->buffer[0], .end=&ret->buffer[strlen(str)]};
	ret->begin = &ret->buffer[0];
	ret->end = &ret->buffer[strlen(str)];
	return ret;
}

typedef t_string * string;

string string_init(char *str) {
	size_t len = strlen(str);
	size_t to_alloc = len < SMALL_CHUNK ? SMALL_CHUNK : len < MEDIUM_CHUNK ? MEDIUM_CHUNK : len < BIG_CHUNK ? BIG_CHUNK : len + SMALL_CHUNK;
	string s = (string)malloc(sizeof(t_string) + to_alloc + 1);
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
string cpy_ctor(string str) {
	string s = (string)malloc(sizeof(t_string) + str->capacity + 1);
	if (!s) {
		return &(t_string){.error=HEAP_ALLOC_ERROR};
	}
	s->capacity = str->capacity;
	s->size = str->size;
	for (size_t i = 0; i < str->size; i++) {
		s->buffer[i] = str->buffer[i];
	}
	s->buffer[s->size] = 0;
	return s;
}
string string_init_size(char *str, size_t len) {
	size_t to_alloc = len < SMALL_CHUNK ? SMALL_CHUNK : len < MEDIUM_CHUNK ? MEDIUM_CHUNK : len < BIG_CHUNK ? BIG_CHUNK : len + SMALL_CHUNK;
	string s = (string)malloc(sizeof(t_string) + to_alloc + 1);
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
string emplace(size_t n) {
	string ret = malloc(sizeof(string) + n);
	ret->capacity = n;
	ret->size = 0;
	ret->buffer[0] = 0;
	return ret;
}
void clear(string s) { memset(s->buffer, 0, s->size); s->size = 0; s->error=NO_ERR; s->buffer[0] = 0;}

size_t length_ptr(string s) { return s->size; }
string strcpy_s_c(string dest, const char *search)
{
	const size_t len = strlen(search);
	if (len >= dest->capacity) {
		// #warning not enought storage for strcpy, dest will be truncated
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
string strcpy_s_s(string dest, const string search)
{
	if (search->size >= dest->capacity) {
		// emit warning
		dest->error = TRUNCATED;
	}
	size_t i = 0;
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
string strcat_s_c(string dest, const char *search)
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
string strcat_s_s(string dest, const string search)
{
	if (dest->size + search->size >= dest->capacity) {
		// emit warning
		dest->error = TRUNCATED;
	}
	size_t i = 0;
	while (dest->size < dest->capacity && search->buffer[i] && i < search->size) {
		dest->buffer[dest->size++] = search->buffer[i++];
	}
	dest->buffer[dest->size] = 0;
	return dest;
}
string strchr_s(string s, int c)
{
	for (size_t i = 0; i < s->size; i++) {
		if (s->buffer[i] == c) {
			return string_init_size(&s->buffer[i], s->size - i);
		}
	}
	return NULL;
}
string strrchr_s(string s, int c)
{
	for (size_t i = s->size; i > 0; --i) {
		if (s->buffer[i] == c) {
			return string_init_size(&s->buffer[i], s->size - i);
		}
	}
	return NULL;
}
string strstr_s_s(string haystack, const string needle)
{
	for (size_t i = 0; i < haystack->size; i++) {
		if (haystack->buffer[i] == needle->buffer[0]) {
			for (size_t j = 0; j < needle->size && i < haystack->size; j++, i++) {
				if (haystack->buffer[i] != needle->buffer[j]) {
					break;
				} else if (j == needle->size -1) {
					return string_init_size(&haystack->buffer[i - j], haystack->size - (i - j));
				}
			}
		}
	}
	return NULL;
}
string strstr_s_c(string haystack, const char * needle)
{
	size_t len = strlen(needle);
	for (size_t i = 0; i < haystack->size; i++) {
		if (haystack->buffer[i] == needle[0]) {
			size_t tmp = i;
			for (size_t j = 0; needle[j] && i < haystack->size; j++, i++) {
				if (haystack->buffer[i] != needle[j]) {
					i = tmp;
					break;
				} else if (j == len - 1) {
					return string_init_size(&haystack->buffer[i - j], haystack->size - (i - j));
				}
			}
		}
	}
	return NULL;
}

#define it(a, exp) {iterator a = __it__; exp}
#define val(a, exp) {typeof(*__it__) a = __it__; exp}
#define for_each(a, b, lamb) for (iterator __it__ = a; __it__ != b; __it__++) lamb

#define String(a) _Generic((a), string : cpy_ctor, default : string_init) (a)
#define strlen(a) _Generic((a), string  : length_ptr, default : strlen) (a)
#define strcpy(a, b) _Generic((a), string : _Generic(b, string : strcpy_s_s, char * : strcpy_s_c), default : strcpy) (a, b)
#define strcat(a, b) _Generic((a), string : _Generic(b, string : strcat_s_s, char * : strcat_s_c), default : strcat) (a, b)
#define strchr(a, b) _Generic((a), string : strchr_s, default : strchr) (a, b)
#define strrchr(a, b) _Generic((a), string : strrchr_s, default : strrchr) (a, b)
#define strstr(a, b) _Generic((a), string : _Generic(b, string: strstr_s_s, char * : strstr_s_c), default : strstr) (a, b)