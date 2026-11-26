#include <iaskdic.h>
#include <string.h>
#include "ask_rom.h"

#pragma section ATA
const char *ntgDicBuild = ask_dictionary_build_text;
#pragma section
static struct {
    int remaining;
    const unsigned char *cursor;
} dictionary;

int FAskRom_Open(char *filename, int mode)
{
    dictionary.remaining = sizeof(ask_dictionary_rom);
    dictionary.cursor = ask_dictionary_rom;
    return 1;
}

int FAskRom_Close(int stream)
{
    return 0;
}

int FAskRom_Read(int stream, unsigned char *buffer, int length)
{
    int count = dictionary.remaining;
#ifdef NONMATCHING
    /* this side refuses a negative length */
    if (length < 0) {
        return -1;
    }
#endif
    if (count >= length) {
        count = length;
    }
    memcpy(buffer, dictionary.cursor, count);
    dictionary.remaining -= count;
    dictionary.cursor += count;
    return count;
}

int FAskRom_Write(int stream, unsigned char *buffer, int length)
{
    return -1;
}

long FAskRom_Seek(int stream, long offset, int origin)
{
    long remaining;
    switch (origin) {
    case ASK_FROM_START:
        if (offset < 0) {
            return -1;
        }
#ifdef NONMATCHING
        /* this side refuses an offset past the dictionary's end */
        if ((unsigned long)offset > sizeof(ask_dictionary_rom)) {
            return -1;
        }
#endif
        dictionary.remaining = sizeof(ask_dictionary_rom) - offset;
        dictionary.cursor = ask_dictionary_rom + offset;
        break;
    case ASK_FROM_CURSOR:
#ifdef NONMATCHING
        /* this side refuses an offset that would put the cursor outside the
         * dictionary, for which the subtraction below can overflow */
        if (offset > dictionary.remaining ||
            offset < dictionary.remaining - (long)sizeof(ask_dictionary_rom)) {
            return -1;
        }
#endif
        remaining = dictionary.remaining - offset;
        if (remaining < 0 ||
            (unsigned long)remaining > sizeof(ask_dictionary_rom)) {
            return -1;
        }
        dictionary.remaining = remaining;
        dictionary.cursor += offset;
        break;
    case ASK_FROM_END:
        if (offset > 0) {
            return -1;
        }
#ifdef NONMATCHING
        /* this side repairs the bug tagged below: the cursor lies where offset
         * names. it refuses an offset before the dictionary's start */
        if (offset < -(long)sizeof(ask_dictionary_rom)) {
            return -1;
        }
        dictionary.remaining = -offset;
        dictionary.cursor =
            ask_dictionary_rom + (sizeof(ask_dictionary_rom) + offset);
#else
        /* @bug logic: a seek from the end leaves the cursor a byte before
         * where offset names, at 0x0c9f2088. the offset it returns is a byte
         * short too. remaining counts from where offset names, so a read
         * starts a byte early and ends a byte short */
        dictionary.remaining = -offset;
        dictionary.cursor =
            ask_dictionary_rom + (sizeof(ask_dictionary_rom) - 1 + offset);
#endif
        break;
#ifdef NONMATCHING
    /* this side refuses an origin none of the three cases takes, for which the
     * matching side returns the cursor's offset unchanged */
    default:
        return -1;
#endif
    }
    return dictionary.cursor - ask_dictionary_rom;
}
