
#ifndef _UART_H
#define _UART_H

#include <stdio.h>

void init_uart();
int putchar(int ch);
int getchar();
int puts(const char *str);
int fputs(const char *str, FILE *stream);

#endif

