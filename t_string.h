#pragma once

# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include <stdbool.h>
# include <printf.h>
# include <stdarg.h>
# include <unistd.h>

#define SMALL_CHUNK 128
#define MEDIUM_CHUNK 512
#define BIG_CHUNK 2048

enum STR_ERR {
	NO_ERR = 0,
	OUT_OF_BOUND,
	HEAP_ALLOC_ERROR,
	TRUNCATED,
};

typedef void * iterator;

/* default string structure. Begin and End are meant to be used for range loop mechanic*/
typedef struct s_string {
	size_t size;
	size_t capacity;
	char * begin;
	char * end;
	int error;
	char buffer[];
}	t_string;

/* Testing fixed sized array for performance */
typedef struct s_string_128 {
	size_t size;
	size_t capacity;
	iterator begin;
	iterator end;
	int error;
	char buffer[128];
}	string_128;
typedef struct s_string_512 {
	size_t size;
	size_t capacity;
	iterator begin;
	iterator end;
	int error;
	char buffer[512];
}	string_512;
typedef struct s_string_4096 {
	size_t size;
	size_t capacity;
	iterator begin;
	iterator end;
	int error;
	char buffer[4096];
}	string_4096;

/* typedef to string. You can't use regular string without malloc due to Flexible Array Member. If you need stack allocated string, try using fixed size structures */
typedef t_string * string;

/* printf logic to add a custom specifier. %T will print our string. This will generate compiler warnings */
bool g_initialize_printf_spec = false;
static int printf_arginfo_T(const struct printf_info *info, size_t n, int argtypes[n], int size[n])
{
    /* "%T" always takes one argument, a pointer to t_string. */
    if (n > 0) {
        argtypes[0] = PA_POINTER;
    }

    return 1;
} /* printf_arginfo_T */
static int printf_output_T(FILE *stream, const struct printf_info *info, const void *const *args)
{
	string str = *(string *)(args[0]);
	if (!str || str->size == 0) { return 0; }
	fprintf(stream, "%s", str->buffer);
    return str->size;
} /* printf_output_T */

/* String costructor. Initialize a string taking a char * as parameter. This enforce security. Manual initialization is possible,
   although highly discouraged */
string string_init(char *str) {
	// printf spec
	if (g_initialize_printf_spec == false) {
		register_printf_specifier('T', printf_output_T, printf_arginfo_T);
		g_initialize_printf_spec = true;
	}
	size_t len = strlen(str);
	size_t to_alloc = len < SMALL_CHUNK ? SMALL_CHUNK : len < MEDIUM_CHUNK ? MEDIUM_CHUNK : len < BIG_CHUNK ? BIG_CHUNK : len + SMALL_CHUNK;
	string s = (string)malloc(sizeof(t_string) + to_alloc + 1);
	if (!s) {
		return &(t_string){.error=HEAP_ALLOC_ERROR};
	}
	s->capacity = to_alloc;
	s->size = len;
	s->begin = &s->buffer[0];
	s->end = &s->buffer[s->size];
	for (size_t i = 0; i < len; i++) {
		s->buffer[i] = str[i];
	}
	s->buffer[len] = 0;
	return s;
}
/* String copy costructor. It takes a string as parameter, and create a new allocated copy */
string cpy_ctor(string str) {
	// printf spec
	if (g_initialize_printf_spec == false) {
		register_printf_specifier('T', printf_output_T, printf_arginfo_T);
		g_initialize_printf_spec = true;
	}
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
/* support function for to */
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
void reserve(string *s, size_t n) {
	*s = malloc(sizeof(t_string) + n);
	(*s)->begin = &(*s)->buffer[0];
	(*s)->end = (*s)->begin;
	(*s)->capacity = n;
	(*s)->size = 0;
	(*s)->buffer[0] = 0;
}
void clear(string s) { memset(s->buffer, 0, s->size); s->size = 0; s->error=NO_ERR; s->buffer[0] = 0;}

size_t length(string s) { return s->size; }
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

string sstrchr_s(string s, int c)
{
	for (size_t i = 0; i < s->size; i++) {
		if (s->buffer[i] == c) {
			return string_init_size(&s->buffer[i], s->size - i);
		}
	}
	return NULL;
}
string sstrrchr_s(string s, int c)
{
	for (size_t i = s->size; i > 0; --i) {
		if (s->buffer[i] == c) {
			return string_init_size(&s->buffer[i], s->size - i);
		}
	}
	return NULL;
}
string sstrstr_s_s(string haystack, const string needle)
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
string sstrstr_s_c(string haystack, const char * needle)
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

char *strchr_s(string s, int c)
{
	for (size_t i = 0; i < s->size; i++) {
		if (s->buffer[i] == c) {
			return &s->buffer[i];
		}
	}
	return NULL;
}
char *strrchr_s(string s, int c)
{
	for (size_t i = s->size; i > 0; --i) {
		if (s->buffer[i] == c) {
			return &s->buffer[i];
		}
	}
	return NULL;
}
char *strstr_s_s(string haystack, const string needle)
{
	for (size_t i = 0; i < haystack->size; i++) {
		if (haystack->buffer[i] == needle->buffer[0]) {
			for (size_t j = 0; j < needle->size && i < haystack->size; j++, i++) {
				if (haystack->buffer[i] != needle->buffer[j]) {
					break;
				} else if (j == needle->size -1) {
					return &haystack->buffer[i - j];
				}
			}
		}
	}
	return NULL;
}
char *strstr_s_c(string haystack, const char * needle)
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
					return &haystack->buffer[i - j];
				}
			}
		}
	}
	return NULL;
}

#define ENSURE_SPACE(a, n) (n > a ? false : true)
void push_back(string s, char c)
{
	if (!ENSURE_SPACE(s->capacity - s->size, 1)) {
		string new;
		reserve(&new, s->capacity + 10);
		strcpy_s_s(new, s);
		*s = *new;
		free(new);
		s->buffer[s->size] = c;
		s->size++;
		s->buffer[s->size] = 0;
		s->end++;
	} else {
		s->buffer[s->size++] = c;
		s->buffer[s->size] = 0;
		s->end++;
	}
}

#define EXTEND_CAP(s, n)
#define String(a) _Generic((a), string : cpy_ctor, default : string_init) (a)
#define strlen(a) _Generic((a), string  : length, default : strlen) (a)
#define strcpy(a, b) _Generic((a), string : _Generic(b, string : strcpy_s_s, char * : strcpy_s_c), default : strcpy) (a, b)
#define strcat(a, b) _Generic((a), string : _Generic(b, string : strcat_s_s, char * : strcat_s_c), default : strcat) (a, b)
#define strchr(a, b) _Generic((a), string : strchr_s, default : strchr) (a, b)
#define sstrchr(a, b) _Generic((a), string : sstrchr_s, default : strchr) (a, b)
#define strrchr(a, b) _Generic((a), string : strrchr_s, default : strrchr) (a, b)
#define sstrrchr(a, b) _Generic((a), string : sstrrchr_s, default : sstrrchr) (a, b)
#define strstr(a, b) _Generic((a), string : _Generic(b, string: strstr_s_s, char * : strstr_s_c), default : strstr) (a, b)
#define sstrstr(a, b) _Generic((a), string : _Generic(b, string: sstrstr_s_s, char * : sstrstr_s_c), default : strstr) (a, b)
#define read_str_std(a, b, ...) read(a, b, __VA_ARGS__)
#define read_str(a, b) read_string(a, b)
#define read(a, b, ...) read_str ## __VA_OPT__(_std)(a, b __VA_OPT__(,) __VA_ARGS__)

ssize_t read_string(int fd, string buf)
{
	size_t old_size = buf->size;
	ssize_t n = read(fd, (void *)buf->buffer, buf->capacity);
	if (n == -1) {
		return n;
	} else if (n > 0) {
		// if read is successfull remaining buff is cleared
		while (n <= buf->size) {
			buf->buffer[buf->size--] = 0;
		}
		buf->size = n;
	}
	return n;
}