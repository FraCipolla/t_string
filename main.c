#include "t_string.h"
#include <stdio.h>

int main() {
	t_string *s = String("hello world!");
	char *s2 = "hello";

	printf("t_string %ld\n", strlen(s));
	printf("string.h %ld\n", strlen(s2));

	string s3 = String(" world!");
	strcpy(s, s3);
	printf("t_string %s\n", s->buffer);
	strcpy(s, " aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	printf("t_string %s\n", s->buffer);
	if (s->error == TRUNCATED) {
		printf("truncated\n");
	}
	string s4 = String("Hello");
	string s5 = String(" World!");
	strcat(s4, s5);
	printf("s4 %s\n", s4->buffer);
}
