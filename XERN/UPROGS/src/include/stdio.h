#ifndef STDIO_H
#define STDIO_H

#define STDIN_FILENO   0
#define STDOUT_FILENO  1
#define STDERR_FILENO  2

void putc(char c);
void puts(char *s);
void printf(char *format, ...);
int  gets(char buf[], int maxlength);

#endif
