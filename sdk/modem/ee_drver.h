/* ee_drver: the modem's EEPROM settings and dial directory */
#ifndef MODEM_EE_DRVER_H
#define MODEM_EE_DRVER_H
#include <sg_xpt.h>

#define EEPROM_PROFILES 4
#define EEPROM_DIAL_SLOTS 10
/* the most bytes a stored number holds before its logon string */
#define EEPROM_NUMBER_LENGTH 35

/* load registers, the S-registers, from the power-up profile. where BMR(4)
 * has BMR4_Z_PROFILE set, the profile Z_COMMAND names takes its place. the
 * saved country's limits then apply, or country 0's where the database lacks
 * it.
 * an EEPROM without the signature is reset by eedrvInitEeprom instead.
 * either way, ENCRYPT_KEY takes the saved key, and S56 in registers the saved
 * country */
void eedrvTestEeprom(Uint8 *registers);
/* reset the EEPROM to its initial settings, with no stored numbers. the
 * country targetGetCountryCode gives is saved and made active. registers
 * must hold a whole set of S-registers. the country's initial values replace
 * S35, S57 and the registers its limits list. every profile then takes
 * registers. ENCRYPT_KEY takes the initial key */
void eedrvInitEeprom(Uint8 *registers);
/* overwrite the signature, so the next eedrvTestEeprom resets the EEPROM */
void eedrvClearEeprom(void);
/* write_profile_to_eeprom saves into profile the S-registers a profile keeps,
 * from registers. read_profile_from_eeprom loads them back into registers.
 * profile must be below EEPROM_PROFILES */
void write_profile_to_eeprom(Uint8 profile, Uint8 *registers);
void read_profile_from_eeprom(Uint8 profile, Uint8 *registers);
/* save S49, as Sregisters holds it, into the power-up profile */
void set_dumb_bit(void);
void eedrvSaveCountryCode(Uint8 country);
/* make profile the power-up profile. profile must be below EEPROM_PROFILES */
void eedrvSavePwrUpProfile(Uint8 profile);
/* save key's first length bytes as the encryption key. length must be at
 * most EE_KEY_BYTES */
void eedrvSaveEncryptKey(Uint8 *key, Uint8 length);

/* the contracts below of store_dial_string, eedrvPrepareToStoreString,
 * read_dial_and_logon_strings and send_dial_logon_str_to_dte are the GCC
 * side's. the matching side checks no pointer, slot or directory. its
 * read_dial_and_logon_strings appends nothing to *destination and copies no
 * logon string to log_buf, as its tag in ee_drver.c says */

/* store the number at *text in slot, in place of slot's number. the number
 * ends at a nul, S3_THEN, a logon code, or a ; where mode is 0 or 1. its
 * spaces are dropped.
 *
 * on success the call returns 1 and moves *text to the byte that ends the
 * number. log_size becomes 0. store_dial_string then calls
 * eeprom_save_complete, which eedrvPrepareToStoreString leaves to its caller.
 *
 * the call returns 0 for a null text or *text, or a slot past the last. it
 * returns 0 for a number longer than EEPROM_NUMBER_LENGTH, and for a damaged
 * or full directory. a 0 leaves the EEPROM and *text as they were. a 0 can
 * leave part or all of the number in dial_buf, where the scan copies it
 * first */
char store_dial_string(Uint8 slot, Uint8 **text, Uint8 mode);
char eedrvPrepareToStoreString(Uint8 slot, Uint8 **text, Uint8 mode);
/* return 1 for a byte that begins a logon string, and 0 for any other. the
 * bytes that begin one are ^A, ^C, ^P, ^R, ^T and ^X */
char is_logon_code(Uint8 value);

/* append slot's number to *destination, without a terminator, and move
 * *destination past it. the logon string after the number goes to log_buf,
 * and its length to log_size. slot goes to logon_to_use, and BMR(20) gains
 * BMR20_LOGON. the call then returns 1. it returns 0 for a null destination
 * or *destination, or a slot past the last. so does an empty or damaged
 * record, or a number longer than capacity. a 0 changes nothing. only the GCC
 * build defines eedrvAppendDialString */
char eedrvAppendDialString(Uint8 slot, Uint8 **destination, Uint16 capacity);
/* append as eedrvAppendDialString does, with a capacity of
 * EEPROM_NUMBER_LENGTH. *destination must have room for that many bytes */
char read_dial_and_logon_strings(Uint8 slot, Uint8 **destination);
/* return the bytes slot's record holds, its logon string's included. slot
 * must be below EEPROM_DIAL_SLOTS */
Uint16 GetLengthOfDialString(Uint8 slot);
/* send slot's record to the terminal, between two labels. a line end and the
 * dial label go before it. a line end, the logon label and another line end
 * go after it. a logon code in the record goes as a line end. the
 * record's text from [ to ] is left out */
void send_dial_logon_str_to_dte(Uint8 slot);

#endif
