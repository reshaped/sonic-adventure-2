/* C runtime error codes, as far as the sources use them */
#ifndef ERRNO_H
#define ERRNO_H

/* a result out of its type's range, and an argument outside a function's
 * domain */
#define ERANGE 1100
#define EDOM 1101
/* a division by zero, which the runtime's division answers with a quotient
 * of 0 */
#define EDIV 1102
#define ESTRN 1104 /* a numeric string longer than the conversions take */
#define PTRERR 1106 /* a null FILE */
#define ECBASE 1200 /* a base no conversion takes */
#define ENUM 1208 /* an argument that is not a number */
#define NOTOPN 1300 /* a FILE that isn't open */
#define EBADF 1302 /* a FILE not open for the operation */
#define ECSPEC 1304 /* an invalid directive in a format */

/* the C library's last error */
extern volatile int _errno;
#define errno _errno

#endif
