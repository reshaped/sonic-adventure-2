#ifndef ASK_STRING_H
#define ASK_STRING_H
#ifdef NONMATCHING
/* this side takes string.h's declarations, which the hack tag below gives as
 * the plain form */
#include <string.h>
#else
/* @hack type: strncpy declared with an int count, where string.h's size_t
 * would do */
char *strncpy(char *destination, const char *source, int count);
int strncmp(const char *first, const char *second, unsigned long count);
#endif
#endif
