/* curves through keyed records: a record of any layout, blended field by
 * field from the four keys around a place on a closed curve */
#ifndef SYSTEM_CURVE_H
#define SYSTEM_CURVE_H
#include <sg_xpt.h>

/* the curves curve_evaluate draws through the keys. CURVE_THROUGH is the
 * cubic that passes through each key. CURVE_B_SPLINE is the uniform cubic
 * B-spline, which the keys pull without lying on it */
#define CURVE_THROUGH 0
#define CURVE_B_SPLINE 1

/* the most fields a format may spell */
#define CURVE_FIELDS 19

/* what curve_evaluate takes. keys holds count keys, back to back, each a record
 * of the fields format spells. the blended record goes to out.
 *
 * format has a letter for each field, in order:
 *
 *   c        Uint8     C     Sint8
 *   s        Uint16    S     Sint16
 *   l        Uint32    I, i  Int
 *   L, A, a  Sint32    F, f  Float
 *
 * the record's layout is the one a C struct of those fields has: each field
 * at the next offset its size divides, and the size rounded up to the widest
 * field's. any other letter stops the program with a store to address 1 */
struct curve {
    void *keys;
    Sint32 count;
    char *format;
    Sint32 unk_0c;
    void *out;
    Sint32 kind; /* CURVE_THROUGH or CURVE_B_SPLINE */
    Sint32 unk_18;
};

/* fill the weights curve_evaluate blends by, for each kind of curve. they weigh
 * four keys in a row, at each hundredth from the second key to the third.
 * system_boot_init calls it once */
void curve_weights_init(void);
/* set curve's out to the record at place on the closed curve its keys
 * shape. place counts keys from 0. its fraction lies that far on toward the
 * next key, taken down to a hundredth. each field is the weighted sum of its
 * value in the four keys around place. the sum takes the field's type. the keys
 * around an end wrap to the other end.
 *
 * place must be from 0 up to count, and count 3 or more. format must spell
 * at most CURVE_FIELDS fields, and curve_weights_init must have run. none of
 * this is checked */
void curve_evaluate(struct curve *curve, Float place);

#endif
