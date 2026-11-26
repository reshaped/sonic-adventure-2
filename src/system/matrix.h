/* Ninja's current matrix, set, read and multiplied in place, and a push of
 * the unit matrix on its stack */
#ifndef SYSTEM_MATRIX_H
#define SYSTEM_MATRIX_H
#include <ninja.h>
#include <sg_xpt.h>

/* the indexes of an NJS_MATRIX's translation */
#define MATRIX_X 12
#define MATRIX_Y 13
#define MATRIX_Z 14

/* matrix_set makes matrix the current matrix, and matrix_get copies the
 * current matrix to matrix. neither moves the matrix stack's pointer, so
 * the slot it points at keeps what a push stored there */
void matrix_set(const NJS_MATRIX *matrix);
/* The paired stores require an eight-byte-aligned destination. */
void matrix_get(NJS_MATRIX *matrix);
/* multiply the current matrix by matrix in place. after
 * matrix_multiply_before it is matrix times what it was: a point takes
 * matrix first, as it takes a turn of njRotateX. after matrix_multiply_after
 * it is what it was times matrix: a point takes matrix last.
 * matrix_multiply_after_pairs is matrix_multiply_after, with matrix read
 * eight bytes at a time. no image calls matrix_multiply_after */
void matrix_multiply_before(const NJS_MATRIX *matrix);
void matrix_multiply_after(const NJS_MATRIX *matrix);
void matrix_multiply_after_pairs(const NJS_MATRIX *matrix);

/* push the unit matrix, as njPushMatrix pushes a matrix: the current matrix
 * goes to the stack, and the unit matrix becomes current. it returns TRUE,
 * or FALSE, changing nothing, where the stack is full */
Bool matrix_push_unit(void);

#endif
