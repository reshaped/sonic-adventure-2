/* SHC's intrinsics, declared for a compiler that lacks them. SHC builds each
 * in, and the headers that name them define each for GCC. compile_flags.txt
 * has clang-tidy read this header ahead of each source */
#ifndef CONFIG_INTRINSICS_H
#define CONFIG_INTRINSICS_H

#include "intrinsics_machine.h"

/* mathf.h's and ninja.h's: the float square root and absolute value */
#define _builtin_fabsf __builtin_fabsf
#define _builtin_sqrtf __builtin_sqrtf

#endif
