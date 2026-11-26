/* the C runtime's variable arguments, by GCC's builtins or by SHC's walk of
 * the stack */
#ifndef STDARG_H
#define STDARG_H

#ifdef NONMATCHING
/* this side takes GCC's own va_list and macros. GCC passes arguments in
 * registers while any are free, where the #else side's walk wouldn't find
 * them */
typedef __builtin_va_list va_list;
#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_arg(ap, type) __builtin_va_arg(ap, type)
#define va_end(ap) __builtin_va_end(ap)
#else
/* SHC passes a function's last named parameter on the stack, and every
 * argument after it. each lies at an address that is a multiple of _VA_ALIGN.
 * a va_list points past the argument last read */
typedef char *va_list;
#define _VA_ALIGN 4U
/* p moved size bytes on. where p + size isn't a multiple of _VA_ALIGN, p moves
 * _VA_ALIGN - size % _VA_ALIGN more. from an aligned p, it moves on by size
 * rounded up to a multiple of _VA_ALIGN */
#define _VA_PAST(p, size)                                                      \
    ((int)((p) + (size)) % _VA_ALIGN                                           \
         ? (p) + (size) + (_VA_ALIGN - ((size) % _VA_ALIGN))                   \
         : (p) + (size))
/* p moved size bytes back. where p - size isn't a multiple of _VA_ALIGN, p
 * moves _VA_ALIGN - size % _VA_ALIGN more. from an aligned p, it moves back by
 * size rounded up to a multiple of _VA_ALIGN */
#define _VA_BEFORE(p, size)                                                    \
    ((int)((p) - (size)) % _VA_ALIGN                                           \
         ? (p) - (size) - (_VA_ALIGN - ((size) % _VA_ALIGN))                   \
         : (p) - (size))
#define va_start(ap, param)                                                    \
    ((void)((ap) = _VA_PAST((char *)&(param), sizeof(param))))
#define va_arg(ap, type)                                                       \
    (*((ap) = _VA_PAST(ap, sizeof(type)), (type *)_VA_BEFORE(ap, sizeof(type))))
#define va_end(ap)
#endif

#endif
