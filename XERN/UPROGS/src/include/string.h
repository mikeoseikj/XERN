
#ifndef STRING_H
#define STRING_H

int  strlen(char *s);
char *strcpy(char *dest, char *src);
char *strncpy(char *dest, char *src, unsigned int size);
int  strcmp(char *s1, char *s2);
int  strncmp(char *s1, char *s2, unsigned int size);
char *strcat(char *dest, char *src);
char *strstr(char *haystack, char *needle);
int  atoi(char *s);
void reverse(char str[], char output[]);
char *memcpy(char *dest, char *src, unsigned int size);
char *memmove(char *dest, char *src, unsigned int size);
char *memset(char *s, int c, unsigned int size);
int  memcmp(char *s1, char *s2, unsigned int size); 
int  readline(int fd, char buf[], int len);

#endif
