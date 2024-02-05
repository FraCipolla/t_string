#include "t_string.h"

# include <stdio.h>

int main()
{
    string s = String("prova");
    char *s2 = strchr(s, 'o');
    printf("%s\n", s2);
    string s3 = sstrchr(s, 'o');
    printf("%s\n", s3->buffer);
    push_back(s, 'c');
    printf("%s\n", s->buffer);
    string fail;
    reserve(&fail, 1);
    push_back(fail, 'a');
    printf("1: %s %ld\n", fail->buffer, fail->size);
    push_back(fail, 'a');
    printf("2: %s %ld\n", fail->buffer, fail->size);
    push_back(fail, 'a');
    printf("3: %s %ld\n", fail->buffer, fail->size);
    free(s);
    free(s3);
    free(fail);
}