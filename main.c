#include "t_string.h"

# include <stdio.h>

void print(string s)
{
    printf("cap %ld\n", sizeof(s));
}

int main()
{
    // string s = String("prova");
    // char *str = malloc(100);
    // strcat(str, "prova");
    // printf("str %s\n", str);
    // strcat(s, "prova");
    // printf("str %s\n", s->buffer);
    // string s3 = sstrchr(s, 'o');
    // printf("%s\n", s3->buffer);
    // push_back(s, 'c');
    // printf("%s\n", s->buffer);
    // string fail;
    // reserve(&fail, 1);
    // push_back(fail, 'a');
    // printf("1: %s %ld\n", fail->buffer, fail->size);
    // push_back(fail, 'a');
    // printf("2: %s %ld\n", fail->buffer, fail->size);
    // push_back(fail, 'a');
    // printf("3: %s %ld\n", fail->buffer, fail->size);
    // free(s);
    // free(s3);
    // free(fail);
    // string insert_test;
    // reserve(&insert_test, 200);
    // insert(insert_test, 500, "provaprova");
    // insert(insert_test, 0, "provaprova");
    // printf("%s\n", insert_test->buffer);
    // pop_back(insert_test);
    // printf("%s\n", insert_test->buffer);
    // pop_back(insert_test);
    // printf("%s\n", insert_test->buffer);
    // pop_back(insert_test);
    // printf("%s\n", insert_test->buffer);
    // free(insert_test);
    string s = String("prova");
    string s2 = String("prova2");
    printf("strcmp %d\n", strcmp(s, s2));
}