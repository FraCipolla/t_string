#include "t_string.h"

int main() {
	string s = String("hello world!");
	string s2 = String(s);

	dstring *dstr = init("prova");

	for_each(dstr->begin, dstr->end, it(it, {
		putchar(*it);
		}));

	printf("\nt_string %ld\n", strlen(s));
	printf("string.h %ld\n", strlen(s2));

	string s3 = String(" world!");
	strcpy(s2, s3);
	printf("t_string %s\n", s2->buffer);
	strcpy(s2, " aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	printf("t_string %s\n", s2->buffer);
	if (s->error == TRUNCATED) {
		printf("truncated\n");
	}
	string s4 = String("Hello");
	string s5 = String(" World!");
	strcat(s4, s5);
	printf("s4 %s\n", s4->buffer);

	string ret = strchr(s4, 'W');
	string ret_null = strchr(s4, 'z');
	printf("strchr %s %ld\n", ret->buffer, ret->size);
	if (!ret_null)
		printf("strchr null\n");
	string retr = strrchr(s4, 'l');
	printf("strchr %s %ld\n", retr->buffer, retr->size);
	string str_str = strstr(s4, "lo");
	if (str_str)
		printf("str_str %s %ld\n", str_str->buffer, str_str->size);
	str_str = strstr(s4, s5);
	if (str_str)
		printf("str_str %s %ld\n", str_str->buffer, str_str->size);
}
