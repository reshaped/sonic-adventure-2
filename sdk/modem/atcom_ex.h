/* atcom_ex: the fields of an extended AT command, which commas separate */
#ifndef MODEM_ATCOM_EX_H
#define MODEM_ATCOM_EX_H
#include <sg_xpt.h>

/* what read_parameter_fv and read_string find at *cursor, past any spaces.
 * AT_FIELD_VALUE moves *cursor past what the reader takes, then past any
 * spaces and one comma. AT_FIELD_EMPTY is a comma, which *cursor moves past.
 * AT_FIELD_MISSING leaves *cursor past the spaces */
enum at_field_result { AT_FIELD_VALUE, AT_FIELD_EMPTY, AT_FIELD_MISSING };

/* cursor, *cursor and each call's output must not be null. GCC's build
 * returns AT_FIELD_MISSING for a null, and the image reads through it */

/* read at most eight digits of a number into *value, which changes only for
 * AT_FIELD_VALUE. the kinds 'd', 'l', 'n' and 'z' read decimal. the kinds
 * 'h', 'm' and 's' read hexadecimal, 0 to 9 and A to F */
Uint8 read_parameter_fv(Uint32 *value, char **cursor, char kind);
/* copy at most width letters and digits into buffer. spaces pad them to
 * width, and a nul follows. buffer needs width + 1 bytes, and changes only
 * for AT_FIELD_VALUE */
Uint8 read_string(char *buffer, char **cursor, Uint8 width);

#endif
