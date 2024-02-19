#pragma once

# include <string.h>
# include <stdlib.h>
# include <stdio.h>
# include <stdbool.h>
# include <printf.h>
# include <stdarg.h>
# include <unistd.h>

#include <assert.h>

#define SMALL_CHUNK 128
#define MEDIUM_CHUNK 512
#define BIG_CHUNK 2048
#define ENSURE_SPACE(a, n) (n > a ? false : true)

enum STR_ERR {
	NO_ERR = 0,
	OUT_OF_BOUND,
	HEAP_ALLOC_ERROR,
	TRUNCATED,
};

typedef void * iterator;

/* struct to store an array with known size */
typedef struct s_slice {
	size_t size;
	char *const buffer;
}	t_slice;

typedef struct s_view {
	size_t size;
	const char *const buffer;
}	t_view;

typedef const t_view string_view;

typedef t_slice slice;
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
		return NULL;
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
/* string constructor for empty string */
string string_init_empty(size_t cap)
{
	// printf spec
	if (g_initialize_printf_spec == false) {
		register_printf_specifier('T', printf_output_T, printf_arginfo_T);
		g_initialize_printf_spec = true;
	}
	string s = malloc(sizeof(t_string) + cap);
	if (!s) {
		return NULL;
	}
	*s = (t_string){.capacity=cap, .begin=&s->buffer[0], .end=&s->buffer[0]};
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
/* support function for when you need to create a new string from another knowing its size */
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
/* reserve n memory to s */
void reserve(string *s, size_t n) {
	*s = malloc(sizeof(t_string) + n);
	(*s)->begin = &(*s)->buffer[0];
	(*s)->end = (*s)->begin;
	(*s)->capacity = n;
	(*s)->size = 0;
	(*s)->buffer[0] = 0;
}
void clear(string s) { memset(s->buffer, 0, s->size); s->size = 0; s->error=NO_ERR; s->end=&s->buffer[0];}
bool empty(string s) { return s->size > 0; }
/* Returning length of string */
size_t length(string s) { return s->size; }

int strcmp_s_c(const string s, const char *s2)
{
	size_t i = 0;
	while (s->buffer[i] && s2[i] && i < s->size && s->buffer[i] == s2[i]) {
		++i;
	}
	return (s->buffer[i] - s2[i]);
}
int strcmp_s_s(const string s, const string s2)
{
	size_t i = 0;
	while (s->buffer[i] && s2->buffer[i] && i < s->size && i < s2->size && s->buffer[i] == s2->buffer[i]) {
		++i;
	}
	return (s->buffer[i] - s2->buffer[i]);
}
int strcmp_c_s(const char * s, const string s2)
{
	size_t i = 0;
	while (s[i] && s2->buffer[i] && i < s2->size && s[i] == s2->buffer[i]) {
		++i;
	}
	return (s[i] - s2->buffer[i]);
}
/* strcpy overload accepting a string as first parameter and char * as second */
string strcpy_s_c(string dest, const char *search)
{
	const size_t len = strlen(search);
	// constexpr len_diff = len >= dest->capacity;
	// if (len_diff < 0) {
	// 	static_assert(len_diff < 0, "out_of_range");
	// 	dest->error = TRUNCATED;
	// }
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
/* strcpy overload accepting a string as first and second parameter */
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
/* strcat overload accepting string as first parameter and char * as second */
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
/* strcat overload accepting string as first and second parameter */
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
/* strchr overload accepting string as first parameter and int as second. Returns string */
string sstrchr_s(string s, int c)
{
	for (size_t i = 0; i < s->size; i++) {
		if (s->buffer[i] == c) {
			return string_init_size(&s->buffer[i], s->size - i);
		}
	}
	return NULL;
}
/* strrchr overload accepting string as first parameter and int as second. Returns string */
string sstrrchr_s(string s, int c)
{
	for (size_t i = s->size; i > 0; --i) {
		if (s->buffer[i] == c) {
			return string_init_size(&s->buffer[i], s->size - i);
		}
	}
	return NULL;
}
/* strstr overload accepting string as first and second paramter. Returns string */
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
/* strstr overload accepting string as first paramter and char * as second paramter. Returns string */
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

/* strchr overload accepting string as first paramter and int as second. Returns char * */
char *strchr_s(string s, int c)
{
	for (size_t i = 0; i < s->size; i++) {
		if (s->buffer[i] == c) {
			return &s->buffer[i];
		}
	}
	return NULL;
}
/* strrchr overload accepting string as first paramter and int as second. Returns char * */
char *strrchr_s(string s, int c)
{
	for (size_t i = s->size; i > 0; --i) {
		if (s->buffer[i] == c) {
			return &s->buffer[i];
		}
	}
	return NULL;
}
/* strstr overload accepting string as first paramter and string as second. Returns char * */
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
/* strrstr overload accepting string as first paramter and char * as second. Returns char * */
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

/* build a new string with extra capacity and*/
static void extend_cap(string s, size_t cap)
{
	string new;
	reserve(&new, cap);
	strcpy_s_s(new, s);
	new->capacity = cap;
	new->size = s->size;
	*s = *new;
	free(new);
}

/* push 1 char into end of the string */
void push_back(string s, char c)
{
	if (!ENSURE_SPACE(s->capacity - s->size, 1)) {
		extend_cap(s, s->capacity + 10);
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

/* insert elements at pos. If pos is greater than size, return and print error to stderr */
void insert_s_c(string s, size_t pos, char *to_insert)
{
	const size_t s_len = s->size;
	if (pos > s_len) {
		fprintf(stderr, "out of range\n");
		return ;
	}
	size_t len = strlen(to_insert);
	if (s->size + len > s->capacity) {
		string new;
		reserve(&new, len + s->capacity + 1);
		size_t i = 0;
		for (; i < pos; i++) {
			new->buffer[i] = s->buffer[i];
		}
		size_t j = i;
		while (*to_insert)
			new->buffer[i++] = *to_insert++;
		while (j < s->size) {
			new->buffer[i++] = s->buffer[j++];
		}
		new->buffer[i] = 0;
		new->size = i;
		new->end = &new->buffer[new->size];
		new->begin = &new->buffer[0];
		*s = *new;
		free(new);
	} else {
		s->size--;
		while (*to_insert) {
			s->buffer[pos + len] = s->buffer[s->size++];
			s->buffer[pos++] = *to_insert++;
		}
		s->size++;
		s->buffer[s->size] = 0;
		s->end += len;
	}

}
/* insert with string overload */
void insert_s_s(string s, size_t pos, string to_insert)
{
	const size_t s_len = s->size;
	if (pos > s_len) {
		fprintf(stderr, "out of range\n");
		return ;
	}
	size_t len = to_insert->size;
	if (s->size + len > s->capacity) {
		string new;
		reserve(&new, len + s->capacity + 1);
		size_t i = 0;
		for (; i < pos; i++) {
			new->buffer[i] = s->buffer[i];
		}
		size_t j = i;
		int k = 0;
		while (to_insert->buffer[k])
			new->buffer[i++] = to_insert->buffer[k++];
		while (j < s->size) {
			new->buffer[i++] = s->buffer[j++];
		}
		new->buffer[i] = 0;
		new->size = i;
		new->end = &new->buffer[new->size];
		new->begin = &new->buffer[0];
		*s = *new;
		free(new);
	} else {
		s->size--;
		int k = 0;
		while (to_insert->buffer[k]) {
			s->buffer[pos + len] = s->buffer[s->size++];
			s->buffer[pos++] = to_insert->buffer[k++];
		}
		s->size++;
		s->buffer[s->size] = 0;
		s->end += len;
	}

}

/* overload of standard read function. It works with its intern size to perform reading operations. If read is less than current data, it clears all excessive data */
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

/* slice are pointer to string with size. Needs to be allocated before */
slice to_slice_s(string s)
{
	return (t_slice){.size=s->size, .buffer=s->begin};
}

slice to_slice_c(char *s)
{
	return (t_slice){.size=strlen(s), .buffer=s};
}

/* string_view are read-only pointer to string with size */
string_view s_view_s(const string s) {
	return (t_view){.size = s->size, .buffer=s->begin};
}

string_view s_view_c(const char *s) {
	return (t_view){.size = strlen(s), .buffer=s};
}

/* overload on memcpy. Append at max dest capacity, can't buffer overflow */
void * memcpy_s(string dest, const char *source) {
	printf("memcpy_s\n");
	size_t len = strlen(source);
	if (strlen(source) > dest->capacity) {
		memcpy(dest->buffer, source, dest->capacity);
	} else {
		memcpy(dest->buffer, source, len);
	}
}
void * memcpy_s_with_size(string dest, const char *source, size_t size) {
	if (size > strlen(source)) {
		size = strlen(source);
	}
	if (size > dest->capacity) {
		memcpy(dest->buffer, source, dest->capacity);
	} else {
		memcpy(dest->buffer, source, size);
	}
}
void * memcpy_s_s(string dest, const string source) {
	if (source->size > dest->capacity) {
		memcpy(dest->buffer, source->buffer, dest->capacity);
	} else {
		memcpy(dest->buffer, source, source->size);
	}
}
void * memcpy_s_s_with_size(string dest, const string source, size_t size) {
	if (size > source->size) {
		size = source->size;
	}
	if (size > dest->capacity) {
		memcpy(dest->buffer, source, dest->capacity);
	} else {
		memcpy(dest->buffer, source, size);
	}
}

#define append strcat
/* macro to check if accessing string is between range */
#define at(s, pos) \
	if (pos < 0) { fprintf(stderr,"%s line %d: error:\n\tpos must be positive\n", __FILE__, __LINE__); } \
	else if (pos >= s->size) { fprintf(stderr, "%s line %d:\n\terror: out of range\n", __FILE__, __LINE__); } \
	else s->buffer[pos]
#define expand_memcpy_args(a, b, c) _Generic((a), string : _Generic((b), char * : memcpy_s_with_size, string : memcpy_s_s_with_size), default : memcpy) (a, b, c)
#define expand_memcpy(a, b) _Generic((a), string : _Generic(b, string : memcpy_s_s, char * : memcpy_s)) (a, b)
#define memcpy(a, b, ...) expand_memcpy ## __VA_OPT__(_args)(a, b __VA_OPT__(,) __VA_ARGS__)
#define String_view(a) _Generic((a), string : s_view_s, char * : s_view_c) (a)
#define Slice(a) _Generic ((a), string : to_slice_s, char * : to_slice_c) (a)
#define insert(a, b, c) _Generic((c), string : insert_s_s, char * : insert_s_c) (a, b, c)
#define String(a) _Generic((a), string : cpy_ctor, int : string_init_empty, char * : string_init) (a)
#define strlen(a) _Generic((a), string  : length, default : strlen) (a)
#define strcmp(a, b) _Generic((a), \
	string : _Generic(b, char * : strcmp_s_c, string : strcmp_s_s), \
	char * : _Generic(b, string : strcmp_c_s, char * : strcmp), \
	default : strcmp) (a, b)
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

void shrink_to_fit(string s)
{
	string new;
	reserve(&new, s->size);
	strcpy(new, s);
	*s = *new;
	free(new);
}

void pop_back(string s)
{
	s->size--;
	s->buffer[s->size] = 0;
}