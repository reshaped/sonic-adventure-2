/* C runtime character classes, as far as the sources use them */
#ifndef CTYPE_H
#define CTYPE_H

/* the class bits of each of the 256 character codes, indexed by the code as
 * an unsigned char. a code may have several bits */
extern const unsigned char _ctype[256];
/* _SPACE marks the space character. _WHITE also includes tab through carriage
 * return. _HEX includes decimal digits and the letters A to F in both cases. */
#define _UPPER 0x01
#define _LOWER 0x02
#define _DIGIT 0x04
#define _SPACE 0x08
#define _PUNCT 0x10
#define _CNTRL 0x20
#define _WHITE 0x40
#define _HEX 0x80

/* tests of the class of c's low 8 bits. each returns the class's bits that
 * c has, which are 0 where c is outside the class */
#define isdigit(c) (_ctype[(c) & 0xff] & _DIGIT)
#define isspace(c) (_ctype[(c) & 0xff] & _WHITE)
#define isalnum(c) (_ctype[(c) & 0xff] & (_UPPER | _LOWER | _DIGIT))
#define isupper(c) (_ctype[(c) & 0xff] & _UPPER)
/* c's small letter where c is a capital, else c. it reads c twice */
#define tolower(c) (isupper(c) ? (((c) - 'A') + 'a') : (c))

/* return 'A' to 'Z' for 'a' to 'z', and any other c as it is. c must be EOF
 * or an unsigned char's value */
int toupper(int c);

#endif
