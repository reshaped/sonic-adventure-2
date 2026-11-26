/* Operand-cache control and the parent of Ninja's current matrix. */
#ifndef SYSTEM_CACHE_H
#define SYSTEM_CACHE_H
#include <sg_xpt.h>

/* each returns at once */
void fun_8c0187a0(void);
void fun_8c0187b4(void);
void fun_8c018818(void);

/* Write back and invalidate both halves of the operand cache through P2. */
void cache_empty(void);

/* load the matrix below Ninja's current one on its matrix stack into
 * XMTRX, the FPU's back bank, and return TRUE. the stack stays as it is */
Bool matrix_load_parent(void);

#endif
