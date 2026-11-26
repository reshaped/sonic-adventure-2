/* Sofdec's 64-bit arithmetic, time comparison and copies */
#ifndef SOFDEC_UTY_H
#define SOFDEC_UTY_H
#include <sg_xpt.h>

/* a 64-bit value as two words, the high word first, as UTY_Dmulu and
 * UTY_Dmuls store their products */
struct uty_u64 {
    Uint32 high, low;
};
struct uty_s64 {
    Sint32 high;
    Uint32 low;
};
void UTY_Dmulu(Uint32 left, Uint32 right, struct uty_u64 *product);
void UTY_Dmuls(Sint32 left, Sint32 right, struct uty_s64 *product);
/* high:low divided by divisor. it returns 0xffffffff where divisor is 0 or the
 * quotient needs more than 32 bits. a true quotient can also be 0xffffffff */
Uint32 UTY_Div64u(Uint32 high, Uint32 low, Uint32 divisor);
/* left * right / divisor, from the 64-bit product of their magnitudes,
 * truncated toward 0. where divisor is 0 or the quotient's magnitude is 2^32
 * or more, it returns 0x7fffffff, or (Sint32)0x80000000 where an odd number
 * of the three are negative. a magnitude from 2^31 to 2^32 - 1 returns its
 * low 32 bits, with the sign applied */
Sint32 UTY_MulAbDivC(Sint32 left, Sint32 right, Sint32 divisor);

/* TRUE where the call's relation holds: left == right for Eq, left <= right
 * for Le, and left < right for Lt. where two values' high words are the same
 * and negative, UTY_CmpLeSint64 and UTY_CmpLtSint64 compare their low words
 * reversed */
Bool UTY_CmpEqUint64(const struct uty_u64 *left, const struct uty_u64 *right);
Bool UTY_CmpEqSint64(const struct uty_s64 *left, const struct uty_s64 *right);
Bool UTY_CmpLeUint64(const struct uty_u64 *left, const struct uty_u64 *right);
Bool UTY_CmpLeSint64(const struct uty_s64 *left, const struct uty_s64 *right);
Bool UTY_CmpLtUint64(const struct uty_u64 *left, const struct uty_u64 *right);
Bool UTY_CmpLtSint64(const struct uty_s64 *left, const struct uty_s64 *right);
/* TRUE where value is positive, or for UTY_CmpPzSint64, positive or 0 */
Bool UTY_CmpPlSint64(const struct uty_s64 *value);
Bool UTY_CmpPzSint64(const struct uty_s64 *value);
/* TRUE where ticks_a / unit_a is no later than ticks_b / unit_b. each unit
 * is a positive count of ticks a second. the cross products are 64-bit, so
 * they can't overflow */
Bool UTY_CmpTime(Sint32 ticks_a, Sint32 unit_a, Sint32 ticks_b, Sint32 unit_b);

/* copy count bytes or halfwords from src to dst, lowest address first */
void UTY_MemcpyByte(Uint8 *dst, const Uint8 *src, Uint32 count);
void UTY_MemcpyWord(Uint16 *dst, const Uint16 *src, Uint32 count);

#endif
