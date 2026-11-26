#include <apiask.h>
#include <apinecnf.h>
#include <string.h>
#include "ask_candidates.h"
#include "ask_dictionary.h"
#include "ask_pages.h"
#include "ask_rom.h"

#define DICTIONARY_HEADER_PAGES 13
static unsigned parse_size(const unsigned char *bytes)
{
    return bytes[0] | (bytes[1] << 8);
}

/* The two synthetic pages contain あ (two spellings) and ねこ (one). */
static int sample_word(const char *reading, int length, int meanings,
                       Uint8 kind, const char *writing)
{
    struct ask_reading_match match;
    struct ask_dictionary_word word;
    int found;
    if (dic_getsyn(reading, (Sint16)length, &match) != 1) {
        return 0;
    }
    found = match.length == length && match.count == meanings &&
            dic_get1wd((unsigned)match.entry, kind, 0, &word) == 1 &&
            strcmp((const char *)word.text, writing) == 0;
    dic_freeentid((unsigned)match.entry);
    return found;
}

int main(int argc, char **argv)
{
    struct ask_reading_match match;
    char reading[82];
    char first_candidate[128];
    char suffix[128];
    unsigned records = 0;
    unsigned found = 0;
    unsigned tails = 0;
    unsigned last_misses = 0;
    unsigned page;
    if (comkan_init() != 0) {
        return 1;
    }
    if (argc == 2 && strcmp(argv[1], "sample") == 0) {
        return !(sample_word("\xa2", 1, 2, 24, "\x88\x9f") &&
                 sample_word("\xa2", 1, 2, 40, "\x88\xa2") &&
                 sample_word("\xcd\xb3", 2, 1, 24, "\x94\x4c"));
    }
    if (apiask_19_Henkan("\x82\xcb\x82\xb1", first_candidate, suffix) != 2) {
        return 2;
    }
    for (page = DICTIONARY_HEADER_PAGES;
         page < ASK_DICTIONARY_BYTES / ASK_PAGE_BYTES; ++page) {
        const unsigned char *data = ask_dictionary_rom + page * ASK_PAGE_BYTES;
        unsigned i = 0;
        while (i + 2 <= ASK_PAGE_BYTES) {
            unsigned j, n = parse_size(data + i);
            unsigned length;
            if (!n) {
                break;
            }
            length = data[i + 2];
            if (!length || length + 2 >= sizeof(reading)) {
                return 3;
            }
            for (j = 0; j < length; ++j) {
                reading[j] = (char)data[i + 3 + j];
            }
            reading[length] = 0;
            if (dic_getsyn(reading, (Sint16)length, &match) == 1) {
                if (!match.count || match.length != length) {
                    return 4;
                }
                ++found;
                dic_freeentid((unsigned)match.entry);
            }
            i += n;
            ++records;
            if (ASK_PAGE_BYTES - i < 2) {
                ++tails;
                /* A spelling with two extra kana sorts after this page's last
                 * entry. The index still routes such a reading to this page. */
                reading[length++] = (char)0xf3;
                reading[length++] = (char)0xf3;
                reading[length] = 0;
                if (!dic_getsyn(reading, (Sint16)length, &match)) {
                    ++last_misses;
                } else {
                    dic_freeentid((unsigned)match.entry);
                }
            }
        }
    }
    return found != 19380 || found != records || tails != 34 ||
           tails != last_misses;
}
