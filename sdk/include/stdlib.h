/* C runtime general utilities, as far as the sources use them */
#ifndef STDLIB_H
#define STDLIB_H

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

/* the largest result rand gives */
#define RAND_MAX 32767

/* the absolute value of value, which must not be INT_MIN: no int holds its
 * absolute value */
int abs(int value);
/* strtol(nptr, NULL, 10), as an int */
int atoi(const char *nptr);
/* the long that nptr spells in base, 2 to 36. it skips white space and a
 * sign, then reads base's digits. a base of 0 takes 16 after a 0x or 0X, 8
 * after a 0, and 10 otherwise. a base of 16 skips a 0, and an x or X after it.
 * a value out of range gives LONG_MAX or LONG_MIN, and sets errno to ERANGE.
 * where endptr isn't null, *endptr gets where the reading stopped.
 *
 * it departs from the C standard in four ways.
 * - after the digits, or where there are none, it skips a u and any l after
 *   it. it then skips an l and any u after it, each letter in either case.
 * - with no digits, *endptr points past all it skipped, where the standard
 *   points it at nptr.
 * - any other base returns 0 and sets errno to ECBASE. *endptr then points
 *   past the white space and sign.
 * - a string that runs on 512 or more bytes past nptr returns 0. errno then
 *   becomes ESTRN, and *endptr is left as it was */
long strtol(const char *nptr, char **endptr, int base);
/* a rand before any srand gives the sequence of srand(0), where the C
 * standard gives that of srand(1). otherwise rand and srand have the
 * standard's contract */
int rand(void);
void srand(unsigned int seed);
/* these four call the Shinobi heap. calloc, malloc, free and realloc have
 * the contracts of syCalloc, syMalloc, syFree and syRealloc in shinobi.h. so
 * realloc's ptr must not be null */
void *calloc(size_t nmemb, size_t size);
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);

#endif
