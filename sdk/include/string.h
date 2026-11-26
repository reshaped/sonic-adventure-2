/* C runtime memory and string functions, as far as the sources use them */
#ifndef STRING_H
#define STRING_H

#ifdef NONMATCHING
/* this side takes stddef.h's size_t, an unsigned int. GCC's built-in
 * functions and SH4ZAM's headers declare with it, and the unsigned long below
 * would conflict */
#include <stddef.h>
#define SIZE_T_DEFINED
#endif
/* the type of a size, defined by whichever header comes first */
#ifndef SIZE_T_DEFINED
#define SIZE_T_DEFINED
typedef unsigned long size_t;
#endif

/* the null pointer, defined the same way */
#ifndef NULL
#define NULL ((void *)0)
#endif

/* the functions from memcpy to memmove have the C standard's contracts */
void *memcpy(void *s1, const void *s2, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memset(void *s, int c, size_t n);
char *strcpy(char *s1, const char *s2);
char *strncpy(char *s1, const char *s2, size_t n);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcat(char *s1, const char *s2);
char *strncat(char *s1, const char *s2, size_t n);
char *strchr(const char *s, int c);
char *strpbrk(const char *string, const char *set);
size_t strlen(const char *s);
void *memmove(void *s1, const void *s2, size_t n);

/* return 0 for equal strings, or s1's byte minus s2's at the first byte
 * where the strings differ. each byte is read as a signed char, where the C
 * standard reads an unsigned char. so a byte of 0x80 to 0xff sorts below
 * every byte of 0 to 0x7f, even the terminator */
int strcmp(const char *s1, const char *s2);

#endif
