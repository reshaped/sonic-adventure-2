/* the helpers the C runtime's number conversions share, most of them
 * multiple-precision arithmetic. a number is n bytes, most significant first. a
 * fraction is such a number whose top bit is set, and its exponent is the power
 * of 2 that bit stands for */
#ifndef MULTIPREC_H
#define MULTIPREC_H

/* a fraction's top bit, in its first byte */
#define TOP_BIT 0x80
/* a 9-byte fraction holds 64 bits and three more for _rnd. the third is the
 * sticky bit, which is set where any less significant bit was set.
 * STICKY_BIT is the sticky bit's number, counting from 0 at the first byte's
 * top bit */
#define STICKY_BIT 66
/* the sticky bit, and all three bits for _rnd, in the ninth byte */
#define STICKY_MASK 0x20
#define ROUNDING_MASK 0xe0
/* the bytes of a 9-byte fraction, through the one that holds its sticky
 * bit */
#define STICKY_BYTES 9

/* shift bytes left by bits, from 0 to 7. it returns 1 if a set bit leaves the
 * top, else 0. a bits of 8 changes nothing, and a caller must not read its
 * result */
int _lsfts(unsigned char *bytes, int n, short bits);
/* shift bytes right by bits, from 0 to 7. it returns 1 if a set bit leaves
 * the bottom, else 0. a bits of 8 changes nothing, and a caller must not read
 * its result */
int _rsfts(unsigned char *bytes, int n, short bits);
/* _lsft and _rsft shift bytes left and right by 1, and return the bit
 * shifted out */
int _lsft(unsigned char *bytes, int n);
int _rsft(unsigned char *bytes, int n);
/* 1 when every byte of bytes is 0, else 0 */
int _allzero(const unsigned char *bytes, int n);
/* set bit number bit of the n bytes where any less significant bit is set.
 * bits count from 0, the first byte's top bit. the call clears no bit */
void _setsbit(unsigned char *bytes, int n, int bit);
/* the comparison memcmp makes of n bytes. it returns s1's byte less s2's where
 * the two first differ, or 0 where none does */
int _umemcmp(const unsigned char *s1, const unsigned char *s2, int n);
/* add from into to. a carry out of the first byte is lost */
void _add(unsigned char *to, const unsigned char *from, int n);
/* store to minus from at to, wrapped to n bytes. it returns -1 where from was
 * the greater and mode is SUB_SIGNED. otherwise it returns 0 for a zero
 * difference and 1 for any other */
int _sub(int mode, unsigned char *to, const unsigned char *from, int n);
#define SUB_SIGNED 0
/* store a times b, 8 bytes each, in the 16 bytes at product */
void _mult(const unsigned char *a, const unsigned char *b,
           unsigned char *product);
/* store 10 to the n, for n from -6911 to 6911, as an 8-byte fraction and its
 * exponent */
void _power(int n, short *exponent, unsigned char *fraction);
/* store 5 to the n, for n from 0 to 27, in the 8 bytes at bytes */
void _pow5(int n, unsigned char *bytes);
/* store 10 to the n, for n from 0 to 27, as an 8-byte fraction and its
 * exponent */
void _pow10(int n, short *exponent, unsigned char *fraction);
/* multiply a by b, each an 8-byte fraction. the product replaces a as a
 * 9-byte fraction, so a must have room for 9 bytes. the product's exponent
 * replaces *a_exponent */
void _mult64(short *a_exponent, unsigned char *a, short b_exponent,
             const unsigned char *b);
/* divide dividend by divisor, each an 8-byte fraction. the quotient replaces
 * divisor as a 9-byte fraction, so divisor must have room for 9 bytes. the
 * quotient's exponent replaces *divisor_exponent */
void _div64(short *divisor_exponent, unsigned char *divisor,
            short dividend_exponent, const unsigned char *dividend);
/* store *word in the four bytes at bytes, most significant first. bytes must
 * not overlap *word */
void _mvito4b(unsigned char *bytes, const unsigned long *word);
/* store *half in the two bytes at bytes, most significant first. bytes must
 * not overlap *half */
void _mvsto2b(unsigned char *bytes, const unsigned short *half);

/* divide the number that exponent and fraction give by 10 to the n. the 8
 * bytes at integer get the quotient, rounded to a whole number. the quotient
 * must be 2^-3 or more, and below 2^64 */
void _calcnpw(short exponent, const unsigned char *fraction, int n,
              unsigned char *integer);
/* round the number in bytes to its first bits bits. it returns 1 for a carry
 * out of the first byte, else 0. the bits after the kept ones are the rest,
 * which the call leaves as it is. the rest's first three bits decide the
 * rounding. 101, 110 and 111 round up, and 100 rounds to even. the call reads
 * the three from the byte that holds the first. where the second or third
 * lies in the next byte, it reads as 0 */
int _rnd(unsigned char *bytes, int bits);
/* whether a conversion's scan of the string at start has run too far. it
 * returns 1 where *end isn't 0 and end is 512 or more bytes past start.
 * otherwise it returns 0. a 1 comes with errno set to ESTRN */
int _strchek(const char *end, const char *start);
/* write value as n decimal digits, the least significant first, and a null
 * after them. value is 8 bytes, most significant first, and below 10 to the
 * n. n is 0 to 17 */
void _its(const unsigned char *value, char *digits, int n);
/* _unpack, _log10, _duchek and _xti take an 8-byte double, which they spell
 * long double. SHC's runtime64 route builds them without -fpu=single, where a
 * long double is 8 bytes, as a double is. under GCC's -m4-single-only a
 * double is 4 bytes, and a long double 8 */

/* a double is little-endian. its top halfword is its fourth, and its high
 * word its second */
#define DOUBLE_TOP_HALF 3
#define DOUBLE_HIGH_WORD 1
/* the top halfword holds the sign and the exponent field. below them lie
 * DOUBLE_TOP_FRACTION_BITS of the fraction's bits */
#define DOUBLE_TOP_SIGN 0x8000
#define DOUBLE_TOP_EXPONENT 0x7ff0
#define DOUBLE_TOP_FRACTION 0x000f
#define DOUBLE_TOP_FRACTION_BITS 4
/* the exponent and fraction fields of the high word */
#define DOUBLE_HIGH_EXPONENT 0x7ff00000
#define DOUBLE_HIGH_FRACTION 0x000fffff
/* the fraction field's width in bits */
#define DOUBLE_FRACTION_BITS 52
/* the exponent field of an infinity or a NaN, shifted down */
#define DOUBLE_EXPONENT_MAX (DOUBLE_TOP_EXPONENT >> DOUBLE_TOP_FRACTION_BITS)
/* the exponent field, shifted down, of a double from 1 up to 2 */
#define DOUBLE_BIAS 1023

/* unpack value into its sign, its biased exponent and 7 bytes of fraction. the
 * fraction's leading 1 is its first byte's top bit. the sign is 1 or -1. a
 * subnormal is normalized, with an exponent of 0 or below. value must not be
 * zero: the call never returns for a zero */
void _unpack(long double value, signed char *sign, short *exponent,
             unsigned char *fraction);
/* an estimate of value's decimal exponent: frexp's exponent of value times
 * the double nearest 0.301, truncated toward zero. value must be finite. the
 * product rounds in the current FPSCR mode before conversion to int. an
 * exponent of 1000 gives 301 in nearest mode and 300 in round-toward-zero
 * mode */
int _log10(long double value);
/* what value is: finite, a NaN or an infinity */
int _duchek(long double value);
enum { DUCHEK_FINITE, DUCHEK_NAN, DUCHEK_INFINITE };
/* split value into a sign and an integer of digits decimal digits, times 10
 * to a scale. *sign gets 1 or -1, the 8 bytes at integer get the integer, and
 * *scale gets the scale. a zero gives an integer and a scale of 0. an
 * infinity or a NaN writes only *sign. digits must be 1 or more, and 10 to
 * the digits must fit in 8 bytes.
 *
 * it returns XTI_NAN for a NaN, XTI_PLUS_INFINITY or XTI_MINUS_INFINITY for
 * an infinity, and XTI_FINITE otherwise */
int _xti(long double value, int digits, unsigned char *integer, int *scale,
         signed char *sign);
enum { XTI_MINUS_INFINITY = -1, XTI_FINITE, XTI_PLUS_INFINITY, XTI_NAN };

#endif
