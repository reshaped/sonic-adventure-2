#include <sg_xpt.h>

#include "country.h"
#include "ecdc_def.h"
#include "ee_drver.h"
#include "ee_layout.h"
#include "ee_ram.h"
#include "messages.h"
#include "modemdef.h"
#include "target.h"
#include "v42def.h"

#define CONTROL(letter) ((letter) - '@')
/* the byte eedrvClearEeprom writes over the signature */
#define NO_SIGNATURE 0xff
/* the byte that ends eeprom_initial_header's settings after the signature */
#define HEADER_END 0xff

#ifdef NONMATCHING
/* the directory as this side reads it whole: each number's offset, and the
 * end of the last */
struct dial_directory {
    Uint16 offsets[EEPROM_DIAL_SLOTS + 1];
};
#endif

static const Uint8 eeprom_profile_registers[EE_PROFILE_SIZE] = {
#include "modem/ee_drver/eeprom_profile_registers.inc"
};

static const Uint8 eeprom_initial_header[EE_DIRECTORY] = {
#include "modem/ee_drver/eeprom_initial_header.inc"
};
static const char eeprom_dial_label[16] = {
#include "modem/ee_drver/eeprom_dial_label.inc"
};
static const char eeprom_logon_label[13] = {
#include "modem/ee_drver/eeprom_logon_label.inc"
};

static char prepare_dial_string(Uint8 slot, Uint8 **text, Uint8 mode);
static void load_eeprom_settings(Uint8 *registers);

void eedrvTestEeprom(Uint8 *registers)
{
    Uint8 signature[EE_SIGNATURE_BYTES];

    eeprom_read_data(EE_SIGNATURE, EE_SIGNATURE_BYTES, signature);
    if (signature[0] != eeprom_initial_header[0] ||
        signature[1] != eeprom_initial_header[1] ||
        signature[2] != eeprom_initial_header[2] ||
        signature[3] != eeprom_initial_header[3]) {
        eedrvInitEeprom(registers);
    } else {
        load_eeprom_settings(registers);
    }
    eeprom_read_data(EE_COUNTRY, 1, registers + 56);
}

void eedrvInitEeprom(Uint8 *registers)
{
    Uint8 index;
    Uint8 first_number;
    Uint16 offset;

#ifdef NONMATCHING
    /* spoil the signature first, so a reset cut short leaves no valid one */
    eeprom_write_data(NO_SIGNATURE, EE_SIGNATURE);
#endif
    offset = EE_PROFILE_SELECTOR;
    index = EE_PROFILE_SELECTOR;
    while (eeprom_initial_header[index] != HEADER_END) {
        eeprom_write_data(eeprom_initial_header[index++], offset++);
    }
    eeprom_read_data(EE_KEY_OFFSET, EE_KEY_BYTES, ENCRYPT_KEY);
    eeprom_write_data(targetGetCountryCode(), EE_COUNTRY);
    country_InitCountry(targetGetCountryCode());
    country_CheckAllSregDefaults(registers);
    offset = EE_DIRECTORY;
    for (index = 0; index <= EEPROM_DIAL_SLOTS; index++) {
        /* @hack spelling: EE_NUMBERS held in first_number, where writing it
         * would do */
        first_number = EE_NUMBERS;
        eeprom_write_data(first_number, offset++);
        eeprom_write_data(EE_NUMBERS >> 8, offset++);
    }
    index = 0;
    while (index < EEPROM_PROFILES) {
        write_profile_to_eeprom(index++, registers);
    }
    offset = EE_SIGNATURE;
    index = EE_SIGNATURE;
    while (offset < EE_SIGNATURE + EE_SIGNATURE_BYTES) {
        eeprom_write_data(eeprom_initial_header[index++], offset++);
    }
    eeprom_save_complete();
}

void write_profile_to_eeprom(Uint8 profile, Uint8 *registers)
{
    /* @hack spelling: the profile's offset in the EEPROM held in an int first,
     * where read_profile_from_eeprom's one expression would do */
    int displacement = profile * EE_PROFILE_SIZE;
    Uint16 offset = displacement + EE_PROFILES;
    Uint8 index;

#ifdef NONMATCHING
    /* this side refuses a profile past the EEPROM's EEPROM_PROFILES */
    if (profile >= EEPROM_PROFILES) {
        return;
    }
#endif
    index = 0;
    while (eeprom_profile_registers[index] != EE_REGISTER_END) {
        eeprom_write_data(registers[eeprom_profile_registers[index++]],
                          offset++);
    }
    eeprom_save_complete();
}

static void load_eeprom_settings(Uint8 *registers)
{
    Uint8 profile;
    Uint8 country;

    eeprom_read_data(EE_KEY_OFFSET, EE_KEY_BYTES, ENCRYPT_KEY);
    eeprom_read_data(EE_PROFILE_SELECTOR, 1, &profile);
    if (BMR(4) & BMR4_Z_PROFILE) {
        profile = Z_COMMAND;
    }
    eeprom_read_data(EE_COUNTRY, 1, &country);
    if (!country_CheckCountry(country)) {
        country = 0;
    }
    country_InitCountry(country);
#ifdef NONMATCHING
    /* this side takes profile 0 for a selector past the EEPROM's
     * EEPROM_PROFILES */
    if (profile >= EEPROM_PROFILES) {
        profile = 0;
    }
#endif
    read_profile_from_eeprom(profile, registers);
    country_CheckAllSregLimits(registers);
}

void read_profile_from_eeprom(Uint8 profile, Uint8 *registers)
{
    Uint8 saved[EE_PROFILE_SIZE];
    Uint8 *value;
    Uint8 index;
    Uint16 offset = (profile * EE_PROFILE_SIZE) + EE_PROFILES;

#ifdef NONMATCHING
    /* this side refuses a profile past the EEPROM's EEPROM_PROFILES */
    if (profile >= EEPROM_PROFILES) {
        return;
    }
#endif
    eeprom_read_data(offset, EE_PROFILE_SIZE, saved);
    index = 0;
    value = saved;
    while (eeprom_profile_registers[index] != EE_REGISTER_END) {
        registers[eeprom_profile_registers[index++]] = *value++;
    }
}

void set_dumb_bit(void)
{
    Uint8 profile;
    Uint16 offset;
    Uint8 index;

    eeprom_read_data(EE_PROFILE_SELECTOR, 1, &profile);
#ifdef NONMATCHING
    /* this side does nothing for a stored selector past the EEPROM's
     * EEPROM_PROFILES */
    if (profile >= EEPROM_PROFILES) {
        return;
    }
#endif
    offset = profile * EE_PROFILE_SIZE;
    index = 0;
    while (eeprom_profile_registers[index] != 49) {
        index++;
        if (eeprom_profile_registers[index] == EE_REGISTER_END) {
#ifdef NONMATCHING
            /* this side returns where the matching side spins. neither runs
             * while eeprom_profile_registers holds 49 */
            return;
#else
            /* eeprom_profile_registers holds 49, so the scan never reaches
             * EE_REGISTER_END */
            for (;;) {
            }
#endif
        }
    }
    eeprom_write_data(Sregisters[49], offset + index + EE_PROFILES);
    eeprom_save_complete();
}

char store_dial_string(Uint8 slot, Uint8 **text, Uint8 mode)
{
    char result = prepare_dial_string(slot, text, mode);
    if (result == 1) {
        eeprom_save_complete();
    }
    return result;
}

char eedrvPrepareToStoreString(Uint8 slot, Uint8 **text, Uint8 mode)
{
    /* @hack spelling: prepare_dial_string's result held in a local, where
     * returning it would do */
    char result = prepare_dial_string(slot, text, mode);
    return result;
}

#ifdef NONMATCHING
/* this side repairs the bugs tagged on the matching side below: a store
 * leaves log_size 0, and a read copies the number to *destination and the
 * logon string to log_buf from its start. it also reads the directory whole
 * and checks it before it trusts an offset, and refuses a slot past
 * EEPROM_DIAL_SLOTS and a number or a logon string longer than its room */
static char read_directory(struct dial_directory *directory)
{
    Uint16 *offsets = directory->offsets;
    Uint16 *end;
    unsigned int slot;
    Uint8 *words = (Uint8 *)offsets;

    eeprom_read_data(EE_DIRECTORY, sizeof(directory->offsets),
                     (Uint8 *)offsets);
    /* each offset is built from its two bytes, low first, as the EEPROM
     * stores them, whatever the host's byte order */
    for (slot = 0; slot <= EEPROM_DIAL_SLOTS; slot++) {
        offsets[slot] = words[0] | ((Uint16)words[1] << 8);
        words += 2;
    }
    if (offsets[0] != EE_NUMBERS ||
        offsets[EEPROM_DIAL_SLOTS] >= EE_NUMBERS_LIMIT) {
        return 0;
    }
    end = offsets + EEPROM_DIAL_SLOTS;
    do {
        if (offsets[0] > offsets[1]) {
            return 0;
        }
        offsets++;
    } while (offsets != end);
    return 1;
}

static char prepare_dial_string(Uint8 slot, Uint8 **text, Uint8 mode)
{
    struct dial_directory directory;
    Uint8 tail[EE_NUMBERS_LIMIT - EE_NUMBERS];
    Uint8 *cursor;
    int length;
    Uint16 start;
    int end;
    int used;
    int index;
    Uint16 offset;
    Uint8 signature_byte;
    int difference;

    if (slot >= EEPROM_DIAL_SLOTS) {
        return 0;
    }
    cursor = text ? *text : 0;
    if (!cursor) {
        return 0;
    }
    length = 0;
    while (*cursor && *cursor != S3_THEN && !is_logon_code(*cursor) &&
           (*cursor != ';' || mode > 1)) {
        if (*cursor != ' ') {
            if (length == EEPROM_NUMBER_LENGTH) {
                return 0;
            }
            dial_buf[length++] = *cursor;
        }
        cursor++;
    }
    if (!read_directory(&directory)) {
        return 0;
    }
    start = directory.offsets[slot];
    end = directory.offsets[slot + 1];
    used = directory.offsets[EEPROM_DIAL_SLOTS];
    difference = length - (end - start);
    if (used + difference >= EE_NUMBERS_LIMIT) {
        return 0;
    }
    eeprom_read_data(end, used - end, tail);
    eeprom_read_data(EE_SIGNATURE, 1, &signature_byte);
    eeprom_write_data(signature_byte + 1, EE_SIGNATURE);
    for (index = 0; index < length; index++) {
        eeprom_write_data(dial_buf[index], start++);
    }
    for (index = 0; index < used - end; index++) {
        eeprom_write_data(tail[index], start++);
    }
    /* each offset after slot's moves by difference, the end of the last
     * number's included */
    for (index = slot + 1; index < EEPROM_DIAL_SLOTS + 1; index++) {
        offset = directory.offsets[index] + difference;
        eeprom_write_data((Uint8)offset,
                          EE_DIRECTORY + (index * EE_ENTRY_BYTES));
        eeprom_write_data((Uint8)(offset >> 8),
                          EE_DIRECTORY + (index * EE_ENTRY_BYTES) + 1);
    }
    eeprom_write_data(signature_byte, EE_SIGNATURE);
    log_size = 0;
    *text = cursor;
    return 1;
}

static void remember_logon(Uint8 slot, Uint8 *logon, unsigned int length)
{
    Uint8 *cursor;

    logon_to_use = slot;
    log_size = (Uint8)length;
    for (cursor = log_buf; length; --length) {
        *cursor++ = *logon++;
    }
    BMR(20) |= BMR20_LOGON;
}

char eedrvAppendDialString(Uint8 slot, Uint8 **destination, Uint16 capacity)
{
    struct dial_directory directory;
    Uint16 *offsets = directory.offsets;
    Uint8 record[EEPROM_NUMBER_LENGTH + sizeof(log_buf)];
    Uint8 *logon;
    Uint8 *end;
    Uint16 length;
    Uint8 number;
    Uint8 *source;
    Uint16 offset;

    if (slot >= EEPROM_DIAL_SLOTS || !destination || !*destination ||
        !read_directory(&directory)) {
        return 0;
    }
    offsets += slot;
    length = offsets[1] - offsets[0];
    /* an empty record fails too, since its length - 1 converts to the largest
     * size_t */
    if (length - 1 >= sizeof(record)) {
        return 0;
    }
    offset = offsets[0];
    end = record + length;
    logon = end;
    for (source = record; source < end; source++) {
        eeprom_read_data(offset++, 1, source);
        if (logon == end && is_logon_code(*source)) {
            logon = source;
        }
    }
    number = (Uint8)(logon - record);
    if ((unsigned int)(end - logon) > sizeof(log_buf) ||
        number > EEPROM_NUMBER_LENGTH || number > capacity) {
        return 0;
    }
    source = record;
    while (source < logon) {
        **destination = *source++;
        (*destination)++;
    }
    remember_logon(slot, logon, (unsigned int)(end - logon));
    return 1;
}

char read_dial_and_logon_strings(Uint8 slot, Uint8 **destination)
{
    return eedrvAppendDialString(slot, destination, EEPROM_NUMBER_LENGTH);
}

void send_dial_logon_str_to_dte(Uint8 slot)
{
    struct dial_directory directory;
    Uint16 *entry;
    Uint8 byte;
    char hidden = 0;

    if (slot >= EEPROM_DIAL_SLOTS || !read_directory(&directory)) {
        return;
    }
    entry = &directory.offsets[slot];
    send_cr_lf();
    send_message(eeprom_dial_label);
    while (entry[0] < entry[1]) {
        eeprom_read_data(entry[0]++, 1, &byte);
        if (is_logon_code(byte)) {
            send_cr_lf();
        } else if (hidden) {
            if (byte == ']') {
                hidden = 0;
            }
        } else if (byte == '[') {
            hidden = 1;
        } else {
            message_byte_to_dte(byte);
        }
    }
    send_cr_lf();
    send_message(eeprom_logon_label);
    send_cr_lf();
}
#else
static char prepare_dial_string(Uint8 slot, Uint8 **text, Uint8 mode)
{
    Uint8 *cursor = *text;
    Uint8 *out;
    Uint8 count;
    /* @hack redundant: quoted, which stays 0, and its two tests */
    char quoted;
    Uint16 start;
    Uint16 end;
    Uint16 total;
    int index;
    Uint16 new_total;
    /* @hack spelling: the old record's length held in a Uint8, where change's
     * expression could compute it for a record under 256 bytes */
    Uint8 old_length;
    /* @hack type: change as a char, where an int would do for an old record
     * under 129 bytes */
    char change;
    Uint8 signature;
    Uint8 tail[EE_NUMBERS_LIMIT - EE_NUMBERS];
    Uint8 *tail_cursor;

    quoted = 0;
    out = dial_buf;
    count = 0;
    while ((!is_logon_code(*cursor) || quoted) && *cursor != S3_THEN &&
           (*cursor != ';' || mode > 1)) {
        if (*cursor != ' ' || quoted) {
            /* @hack spelling: the else after a return, where the statements
             * after the if would do */
            if (count >= EEPROM_NUMBER_LENGTH) {
                return 0;
            } else {
                count++;
                *out++ = *cursor;
            }
        }
        cursor++;
    }
    log_size = 0;
    /* @hack spelling: slot copied to index inside the first call, where
     * slot itself would do */
    eeprom_read_data(((index = slot) << 1) + EE_DIRECTORY, EE_ENTRY_BYTES,
                     (Uint8 *)&start);
    eeprom_read_data(((index + 1) * EE_ENTRY_BYTES) + EE_DIRECTORY,
                     EE_ENTRY_BYTES, (Uint8 *)&end);
    old_length = end - start;
    eeprom_read_data(EE_DIRECTORY + (EEPROM_DIAL_SLOTS * EE_ENTRY_BYTES),
                     EE_ENTRY_BYTES, (Uint8 *)&total);
    /* @hack redundant: log_size, which is 0, in the sum */
    change = count + log_size - old_length;
    new_total = total + change;
    if (new_total >= EE_NUMBERS_LIMIT) {
        return 0;
    }
    eeprom_read_data(end, total - end, tail);
    eeprom_read_data(EE_SIGNATURE, 1, &signature);
    eeprom_write_data(signature + 1, EE_SIGNATURE);
    out = dial_buf;
    while (count-- > 0) {
        eeprom_write_data(*out++, start++);
    }
    /* @bug logic: leaves log_size 255 after each store, since the test that
     * ends this loop takes 1 from a log_size of 0, at 0x0caf96a4. nothing
     * reads it before the next store or read clears it */
    out = log_buf;
    while (log_size-- > 0) {
        eeprom_write_data(*out++, start++);
    }
    tail_cursor = tail;
    while (start < new_total) {
        eeprom_write_data(*tail_cursor++, start++);
    }
    /* each offset after slot's moves by change, the end of the last number's
     * included */
    for (count = slot + 1; count <= EEPROM_DIAL_SLOTS; count++) {
        /* @hack spelling: the offset's bytes held in locals, and the high one
         * masked before its shift, where each call could shift and convert
         * start */
        Uint8 low;
        Uint8 high;

        eeprom_read_data((count * EE_ENTRY_BYTES) + EE_DIRECTORY,
                         EE_ENTRY_BYTES, (Uint8 *)&start);
        start += change;
        low = start;
        high = (start & 0xff00) >> 8;
        eeprom_write_data(low, (count * EE_ENTRY_BYTES) + EE_DIRECTORY);
        eeprom_write_data(high, (count * EE_ENTRY_BYTES) + EE_DIRECTORY + 1);
    }
    eeprom_write_data(signature, EE_SIGNATURE);
    *text = cursor;
    return 1;
}

char read_dial_and_logon_strings(Uint8 slot, Uint8 **destination)
{
    Uint16 start;
    Uint16 end;
    int index;
    Uint8 byte;
    Uint8 *logon;
    Uint8 *cursor;

    /* @hack spelling: slot copied to index inside the first call, where
     * slot itself would do */
    eeprom_read_data(((index = slot) << 1) + EE_DIRECTORY, EE_ENTRY_BYTES,
                     (Uint8 *)&start);
    eeprom_read_data(((index + 1) * EE_ENTRY_BYTES) + EE_DIRECTORY,
                     EE_ENTRY_BYTES, (Uint8 *)&end);
    /* @hack spelling: *destination read ahead of the two stores, where the
     * loop below could read it */
    cursor = *destination;
    logon_to_use = slot;
    log_size = 0;
    /* the record's first byte, which nothing uses */
    eeprom_read_data(start, 1, &byte);
    if (start == end) {
        return 0;
    }
    while (start < end) {
        eeprom_read_data(start++, 1, &byte);
        /* @hack spelling: BMRregisters[20], where BMR(20) would do */
        BMRregisters[20] |= BMR20_LOGON;
        /* @hack redundant: a block that never runs, whose statements set
         * logon to log_buf and copy a byte to *destination */
        if (0) {
            logon = log_buf;
            *cursor++ = byte;
            BMRregisters[20] = 1;
            cursor = logon;
        }
        /* @bug memory: stores each byte of the record, the number's and the
         * logon string's alike, through logon, which no path sets, at
         * 0x0caf97e0. so the number never reaches *destination, and log_size
         * counts the whole record */
        /* NOLINTNEXTLINE(clang-diagnostic-uninitialized) */
        *logon++ = byte;
        log_size++;
    }
    *destination = cursor;
    return 1;
}

void send_dial_logon_str_to_dte(Uint8 slot)
{
    Uint16 start;
    Uint16 end;
    int index;
    Uint8 byte;
    /* @hack type: hidden, a flag, held in a Uint8 and compared with 1, where
     * a char tested as it is would do */
    Uint8 hidden;

    /* @hack spelling: slot copied to index inside the first call, where
     * slot itself would do */
    eeprom_read_data(((index = slot) << 1) + EE_DIRECTORY, EE_ENTRY_BYTES,
                     (Uint8 *)&start);
    eeprom_read_data(((index + 1) * EE_ENTRY_BYTES) + EE_DIRECTORY,
                     EE_ENTRY_BYTES, (Uint8 *)&end);
    send_cr_lf();
    send_message(eeprom_dial_label);
    /* @hack spelling: hidden cleared here, where its declaration could */
    hidden = 0;
    while (start < end) {
        eeprom_read_data(start++, 1, &byte);
        if (is_logon_code(byte)) {
            send_cr_lf();
        } else if (hidden == 1) {
            if (byte == ']') {
                hidden = 0;
            }
        } else if (byte == '[') {
            hidden = 1;
        } else {
            message_byte_to_dte(byte);
        }
    }
    send_cr_lf();
    send_message(eeprom_logon_label);
    send_cr_lf();
}
#endif

char is_logon_code(Uint8 value)
{
    switch (value) {
    case CONTROL('A'):
    case CONTROL('C'):
    case CONTROL('P'):
    case CONTROL('R'):
    case CONTROL('T'):
    case CONTROL('X'):
        return 1;
    }
    return 0;
}

void eedrvSaveCountryCode(Uint8 country)
{
    eeprom_write_data(country, EE_COUNTRY);
    eeprom_save_complete();
}

void eedrvSavePwrUpProfile(Uint8 profile)
{
#ifdef NONMATCHING
    /* this side refuses a profile past the EEPROM's EEPROM_PROFILES */
    if (profile >= EEPROM_PROFILES) {
        return;
    }
#endif
    eeprom_write_data(profile, EE_PROFILE_SELECTOR);
    eeprom_save_complete();
}

void eedrvSaveEncryptKey(Uint8 *key, Uint8 length)
{
    Uint8 index;

#ifdef NONMATCHING
    /* this side refuses a key longer than the EEPROM's EE_KEY_BYTES */
    if (length > EE_KEY_BYTES) {
        return;
    }
#endif
    for (index = 0; index < length; index++) {
        eeprom_write_data(key[index], EE_KEY_OFFSET + index);
    }
    eeprom_save_complete();
}

void eedrvClearEeprom(void)
{
    Uint8 offset = EE_SIGNATURE;

    while (offset < EE_SIGNATURE + EE_SIGNATURE_BYTES) {
        eeprom_write_data(NO_SIGNATURE, offset++);
    }
    eeprom_save_complete();
}

Uint16 GetLengthOfDialString(Uint8 slot)
{
#ifdef NONMATCHING
    /* this side reads the directory whole and checks it, and refuses a slot
     * past EEPROM_DIAL_SLOTS. the matching side reads the slot's two offsets
     * unchecked */
    struct dial_directory directory;

    if (slot >= EEPROM_DIAL_SLOTS || !read_directory(&directory)) {
        return 0;
    }
    return directory.offsets[slot + 1] - directory.offsets[slot];
#else
    Uint16 start;
    Uint16 end;
    int index;

    /* @hack spelling: slot copied to index inside the first call, where
     * slot itself would do */
    eeprom_read_data(((index = slot) << 1) + EE_DIRECTORY, EE_ENTRY_BYTES,
                     (Uint8 *)&start);
    eeprom_read_data(((index + 1) * EE_ENTRY_BYTES) + EE_DIRECTORY,
                     EE_ENTRY_BYTES, (Uint8 *)&end);
    return end - start;
#endif
}
