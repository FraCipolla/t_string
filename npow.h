#pragma once

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <unistd.h>
#include "t_string.h"

typedef char * iterator;

#define it(a, exp) {iterator a = __it__; exp}
#define val(a, exp) {typeof(*__it__) a = __it__; exp}
#define for_each(a, b, lamb) assert(a < b); \
    for (iterator __it__ = a; __it__ != b; __it__++) lamb

// da controllare con iteratore
#define expand_range_for_it(a, s) typeof(*s) a = *s.begin; \
    for (iterator __it__ = s.begin; __it__ != s.end; __i__++, a = *__it__)
#define expand_range_for(a, s) typeof(*s) a = s[0]; \
    for (int __i__ = 0; __i__ < (int)(sizeof(s) / sizeof(*s)) && s[__i__]; __i__++, a = s[__i__])
#define range_for(a) expand_range_for(a)
#define in ,

// #define print(...) _Generic(& # __VA_ARGS__, char : pchar, int : pint, char (*)[] : pchars, string : pstring) (__VA_ARGS__)

// void pchar (char c) { write(1, &c, 1); }
// void pchars(char *s) { while (*s) {write(1, s, 1); s++;};}
// void pint(int i) { printf("%d", i); }
