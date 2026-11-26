#include <assert.h>
#include <string.h>

#include "atcom.h"
#include "countrydb.h"
#include "ee_drver.h"
#include "ee_ram.h"
#include "modemdef.h"
#include "target.h"

/* On the console the country selection comes from the modem's straps and
 * its register limits come from the country database. */
Uint8 targetGetCountryCode(void)
{
    return 0;
}

void target_set_country(Uint8 code)
{
    assert(code == 0);
}

void targetGetCountryInfo(Uint8 code, struct country_st **record,
                          struct Slimit **limits)
{
    static struct country_st country;
    static struct Slimit end[] = {{255, 0, 0, 0}};

    assert(code == 0);
    *record = &country;
    *limits = end;
}

static void store(Uint8 slot, const char *text, int expected)
{
    Uint8 *cursor = (Uint8 *)text;

    assert(store_dial_string(slot, &cursor, 0) == expected);
    assert(cursor == (Uint8 *)text + (expected ? strlen(text) : 0));
}

static void check_append(void)
{
    Uint8 bytes[6];
    Uint8 *cursor = bytes + 1;

    store(0, "1234", 1);
    memset(bytes, '!', sizeof(bytes));
    assert(!eedrvAppendDialString(0, &cursor, 3));
    assert(cursor == bytes + 1 && !memcmp(bytes, "!!!!!!", sizeof(bytes)));
    assert(eedrvAppendDialString(0, &cursor, 4));
    assert(cursor == bytes + 5 && !memcmp(bytes, "!1234!", sizeof(bytes)));
}

static void dial(const char *input, char resume, Uint16 limit, Uint8 expected,
                 const char *written, size_t length)
{
    Uint8 *cursor = (Uint8 *)input;
    Uint8 bytes[10];
    size_t i;

    memset(bytes, '!', sizeof(bytes));
    assert(get_dial_string(&cursor, bytes + 1, resume, limit) == expected);
    assert(bytes[0] == '!' && !memcmp(bytes + 1, written, length));
    for (i = 1 + length; i < sizeof(bytes); i++) {
        assert(bytes[i] == '!');
    }
}

static void check_dial(void)
{
    store(0, "1234", 1);
    store(1, "56", 1);
    store(2, "789", 1);
    Sregisters[27] = 2;
    dial("", 1, 3, 0, "", 0);
    dial("", 1, 4, 1, "1234\r", 5);
    dial("S1S2\r", 0, 5, 1, "56789\r", 7);
    dial("S1S2\r", 0, 4, 0, "56", 2);
    dial("S256\r", 0, 5, 0, "", 0);
    dial("S257\r", 0, 5, 0, "", 0);
    dial("S00001\r", 0, 2, 1, "56\r", 4);
}

static void check_scratch(void)
{
    Uint8 text[] = "5 6;7";
    Uint8 *cursor = text;

    memset(dial_buf, '!', sizeof(dial_buf));
    store(0, "12 34", 1);
    assert(!memcmp(dial_buf, "1234!", 5));

    assert(eedrvPrepareToStoreString(1, &cursor, 0) == 1);
    assert(cursor == text + 3);
    assert(!memcmp(dial_buf, "5634!", 5));
}

static void check_store(void)
{
    char long_number[EEPROM_NUMBER_LENGTH + 1];
    Uint8 before[EEPROM_SIZE], after[EEPROM_SIZE];
    Uint8 slot;

    memset(long_number, '7', EEPROM_NUMBER_LENGTH);
    long_number[EEPROM_NUMBER_LENGTH] = 0;
    for (slot = 0; slot < 8; slot++) {
        store(slot, long_number, 1);
    }
    /* Eight 35-digit numbers leave 16 of the 296 usable bytes. */
    store(8, "1234567890123456", 1);
    eeprom_read_data(0, sizeof(before), before);
    store(9, "0", 0);
    assert(!memcmp(dial_buf, "02", 2));
    eeprom_read_data(0, sizeof(after), after);
    assert(!memcmp(before, after, sizeof(before)));
    store(2, "", 1);
    store(9, long_number, 1);
    assert(GetLengthOfDialString(8) == 16);
    assert(GetLengthOfDialString(9) == EEPROM_NUMBER_LENGTH);
}

int main(int argc, char **argv)
{
    assert(argc == 2);
    eedrvInitEeprom(Sregisters);
    S3_THEN = '\r';
    if (!strcmp(argv[1], "append")) {
        check_append();
    } else if (!strcmp(argv[1], "dial")) {
        check_dial();
    } else if (!strcmp(argv[1], "scratch")) {
        check_scratch();
    } else {
        assert(!strcmp(argv[1], "store"));
        check_store();
    }
    return 0;
}
