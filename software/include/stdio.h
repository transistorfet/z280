
#ifndef STDIO_H
#define STDIO_H

#include <stdarg.h>

#include <stddef.h>

#define FOPEN_MAX	20
#define BUFSIZE		1024

#define EOF		(-1)

// WHENCE argument to fseek
#define SEEK_SET	0	// Seek relative to the beginning of file
#define SEEK_CUR	1	// Seek relative to the current position
#define SEEK_END	2	// Seek relative to the end of file

#define	_IOFBF		0x000
#define	_IONBF		0x004
#define	_IOFREEBUF	0x008
#define	_IOEOF		0x010
#define	_IOERR		0x020

struct _FILE;

typedef struct _FILE FILE;

extern FILE __stdin;
extern FILE __stdout;
extern FILE __stderr;

#define	stdin		(&__stdin)
#define	stdout		(&__stdout)
#define	stderr		(&__stderr)


//int sprintf(char *buffer, const char *fmt, ...);
//int vsprintf(char *buffer, const char *fmt, va_list args);
//int snprintf(char *buffer, size_t n, const char *fmt, ...);
int vsnprintf(char *buffer, size_t n, const char *fmt, va_list args);

int printf(const char *fmt, ...);
//int vprintf(const char *fmt, va_list args);
//int fprintf(FILE *stream, const char *fmt, ...);
int vfprintf(FILE *stream, const char *fmt, va_list args);

int puts(const char *str);


// Platform-Defined Functions

int getchar(void);
int putchar(int);

//int fputc(int ch, FILE *stream);
//int fgetc(FILE *stream);

int fputs(const char *str, FILE *stream);
//char *fgets(char *str, int max, FILE *stream);

#define getc(stream)		fgetc((stream))
#define putc(ch, stream)	fputc((ch), (stream))

#endif
