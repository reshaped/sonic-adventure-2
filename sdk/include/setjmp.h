/* the C runtime's nonlocal jumps, which setjmp.src and longjmp.src write in
 * assembly */
#ifndef SETJMP_H
#define SETJMP_H

/* the registers setjmp saves, a long word each. in order, they are R15, R14
 * and PR, then R13 down to R0. MACH, MACL and GBR follow, then the current
 * bank's FR0 to FR15, FPUL and FPSCR */
typedef int jmp_buf[38];

/* save the caller's registers in env and return 0. FPSCR.SZ must be clear */
#ifdef NONMATCHING
/* this side tells GCC that setjmp returns twice, so that no value it keeps in a
 * register is lost to the second return */
int setjmp(jmp_buf env) __attribute__((returns_twice));
#else
int setjmp(jmp_buf env);
#endif
/* return from the setjmp that filled env once more, with every register it
 * saved but R0. that setjmp returns value, or 1 for a value of 0. FPSCR.SZ
 * must be clear. longjmp loads FR0 to FR15 into the current bank before it
 * loads FPSCR. so FPSCR.FR must select the bank setjmp saved, or the floats
 * land in the back bank */
#ifdef NONMATCHING
/* this side tells GCC that longjmp doesn't return, so that GCC treats the
 * code after a call as unreachable */
void longjmp(jmp_buf env, int value) __attribute__((noreturn));
#else
void longjmp(jmp_buf env, int value);
#endif

#endif
