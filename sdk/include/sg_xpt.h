/* SDK scalar types and Bool */
#ifndef SG_XPT_H
#define SG_XPT_H

/* SHC's int and long each have 32 bits */
typedef unsigned char Uint8;
typedef signed char Sint8;
typedef unsigned short Uint16;
typedef short Sint16;
/* where long has 64 bits (__LP64__), Uint32 and Sint32 take int, which has
 * 32 bits as SHC's long does */
#ifdef __LP64__
typedef unsigned int Uint32;
typedef int Sint32;
#else
typedef unsigned long Uint32;
typedef long Sint32;
#endif
typedef int Int;
typedef float Float;
typedef Sint32 Bool;
#define FALSE (0)
#define TRUE (1)

#define NULL ((void *)0)

#endif
