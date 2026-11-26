/* byte offsets in the modem's settings EEPROM. EE_DIRECTORY holds a
 * little-endian offset for each stored number, and one past the last. number n
 * runs from offset n up to offset n + 1. the numbers start at EE_NUMBERS and
 * end before EE_NUMBERS_LIMIT */
#ifndef MODEM_EE_LAYOUT_H
#define MODEM_EE_LAYOUT_H
#include <sg_xpt.h>

/* the signature eedrvTestEeprom tests. a reset writes it last. a store of a
 * number changes its first byte until the store ends */
#define EE_SIGNATURE 0
#define EE_SIGNATURE_BYTES 4
#define EE_PROFILE_SELECTOR 4
#define EE_KEY_OFFSET 5
#define EE_KEY_BYTES 6
#define EE_COUNTRY 11
#define EE_DIRECTORY 12
#define EE_ENTRY_BYTES 2
#define EE_PROFILES 34
#define EE_PROFILE_SIZE 40
#define EE_NUMBERS 194
#define EE_NUMBERS_LIMIT 491
#define EE_REGISTER_END 255

#endif
