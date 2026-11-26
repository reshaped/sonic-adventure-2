/* ASK's dictionary: the main dictionary's pages, and a temporary area of the
 * words ASK learns. both hold records. a record is its 16-bit little-endian
 * size, then its reading's length and bytes, then the reading's words. a size
 * of 0 ends a page.
 *
 * a word is its kind, its frequency, a form byte where it has one, and its
 * text. a byte of ASK_LAST_FORM or less after the frequency is a form. the
 * text is Shift-JIS in pairs of bytes, with ASK_ONE_BYTE before each one-byte
 * character. the next word's kind, a byte of ASK_LAST_KIND or less, ends it */
#ifndef ASK_DICTIONARY_H
#define ASK_DICTIONARY_H
#include <sg_xpt.h>

#define ASK_LAST_KIND 56
#define ASK_LAST_FORM 11
#define ASK_ONE_BYTE 0xff
/* conjugating words are of the kinds from 1 to ASK_LAST_CONJUGATING */
#define ASK_LAST_CONJUGATING 13
/* goku_connect and setu_end ignore the form of a word of a kind from
 * ASK_FORM_KINDS up to ASK_LAST_KIND. a record holds such a word's form byte
 * even where it is 0 */
#define ASK_FORM_KINDS 45
/* the bytes past a page's records that hold the size of 0 ending it */
#define ASK_END_MARK 2
/* kinds that conversion gives what it reads without the dictionary */
enum {
    /* letters, katakana, a bracket or a run of text with no reading */
    ASK_PLAIN_WORD = 25,
    ASK_NUMBER_WORD = 31, /* a run of digits */
    ASK_RAW_WORD = 40, /* kanji, or a reading left as it is */
    ASK_SYMBOL_WORD = 41,
    ASK_STOP_WORD = 255 /* a mark that ends a phrase */
};
/* the offsets of a record's reading length and reading, and of a word's
 * frequency and form */
#define ASK_READING_LENGTH_AT 2
#define ASK_READING_AT 3
#define ASK_FREQUENCY_AT 1
#define ASK_FORM_AT 2
#define ASK_MOST_MEANINGS 32
/* the bit of a word's id that places the word in the temporary area. the
 * rest is the word's offset in its record */
#define ASK_TEMP_WORD 0x8000

/* dic_rw's modes, which dic_open opens the dictionary in. under
 * ASK_DIC_READ_ONLY_MODE no word can be registered or deleted */
#define ASK_DIC_READ_ONLY_MODE 0x8000
#define ASK_DIC_READ_WRITE_MODE 0x8002

/* the forms of a number, as suji_mode and a number's id give them */
enum ask_number_form {
    ASK_FULL_WIDTH_DIGITS,
    ASK_KANJI_DIGITS,
    ASK_KANJI_NUMERAL, /* kanji digits and units */
    ASK_FORMAL_NUMERAL, /* the same, with the formal one, two, three and ten */
    ASK_NUMBER_FORMS
};

/* what the dic_ functions return. each that reads the dictionary returns
 * ASK_DIC_CLOSED while none is open */
enum ask_dic_result {
    ASK_DIC_OPEN_ERROR = -8, /* open_dic returns other than 0 or -1 */
    ASK_DIC_CANT_OPEN, /* open_dic returns -1 */
    ASK_DIC_READ_ONLY, /* the dictionary is open for reading alone */
    ASK_DIC_FULL, /* the reading's page has no room for the word */
    ASK_DIC_ALREADY_OPEN,
    ASK_DIC_CLOSED,
    /* a seek, read or close fails, or the header lacks ASK's copyright */
    ASK_DIC_BAD_FILE,
    ASK_DIC_INVALID, /* an empty name, or a mode of neither 0 nor 1 */
    ASK_DIC_NONE,
    ASK_DIC_FOUND,
    ASK_DIC_WHOLE, /* a dictionary reading begins with all of the key */
    ASK_DIC_DONE
};

/* a word to register, delete or learn. reading holds length bytes, and
 * writing is NUL-terminated Shift-JIS. nothing reads reserved */
struct ask_learn_word {
    Uint8 *reading;
    Sint16 length;
    Uint8 reserved, kind, form;
    Uint8 *writing;
};
/* a kind and form that a reading's words take, with the highest frequency
 * among those words. nothing reads reserved */
struct ask_synonym {
    Uint8 kind, form, reserved;
    Uint16 frequency;
};
/* a reading a lookup found: its length, and count kinds and forms of its
 * words. entry holds its records until dic_freeentid releases it.
 * prefix_length is one past the most bytes the key shares with a reading the
 * search compared. found is 1, and nothing reads it */
struct ask_reading_match {
    Sint16 length;
    Sint32 entry;
    Sint16 prefix_length, found, count;
    struct ask_synonym meanings[ASK_MOST_MEANINGS];
};
struct ask_dictionary_word;

/* dic_open opens name and returns ASK_DIC_DONE, or ASK_DIC_READ_ONLY under
 * ASK_DIC_READ_ONLY_MODE. dic_close writes back what changed, and returns
 * ASK_DIC_DONE */
int dic_open(char *name);
int dic_close(void);
/* the longest dictionary reading that the key begins with, the length bytes
 * at reading. dic_snssyn returns ASK_DIC_WHOLE, and fills nothing, where a
 * dictionary reading begins with all of the key. dic_getsyn finds the
 * reading that is the whole key. each returns ASK_DIC_FOUND, or ASK_DIC_NONE
 * where it finds none */
int dic_snssyn(const char *reading, Sint16 length,
               struct ask_reading_match *match);
int dic_getsyn(const char *reading, Sint16 length,
               struct ask_reading_match *match);
/* entry's word of kind and form with the highest frequency, or every such
 * word, highest first. dic_getallwd packs the words, each in 6 bytes more
 * than its text's length, rounded up to even */
int dic_get1wd(unsigned int entry, Uint8 kind, Uint8 form,
               struct ask_dictionary_word *output);
int dic_getallwd(unsigned int entry, Uint8 kind, Uint8 form,
                 struct ask_dictionary_word *output, int *count);
/* the length digits at reading as a number in suji_mode's form. where that
 * form can't write it, dic_get1num writes ASK_FULL_WIDTH_DIGITS.
 * dic_getallnum writes it in each form that can, suji_mode's first. a
 * number's id is its form */
int dic_get1num(const Uint8 *reading, Sint16 length,
                struct ask_dictionary_word *output);
int dic_getallnum(const Uint8 *reading, Sint16 length,
                  struct ask_dictionary_word *output, int *count);
/* make entry's word whose id is synonym the latest learned. its frequency
 * becomes one past the latest time of the count entries at ids. those
 * entries take that time too. with an entry of -1, synonym is a number's id,
 * and suji_mode takes it. the entries then lose every word's frequency, and
 * take time 0 */
int dic_learn(unsigned int entry, Uint16 synonym, Sint32 *ids, int count);
/* register or delete word in its reading's page of the main dictionary. a
 * success releases every entry */
int dic_touroku(struct ask_learn_word *word);
int dic_delete(struct ask_learn_word *word);
/* add word to the temporary area. dic_newlearn ranks it as dic_learn ranks a
 * word, and dic_tmptouroku gives it a frequency of 1. each rewrites
 * word->kind. dic_newlearn gives a reading of digits ASK_NUMBER_WORD.
 * dic_tmptouroku stores ASK_RAW_WORD and ASK_SYMBOL_WORD as kind 39 */
int dic_newlearn(struct ask_learn_word *word, Sint32 *ids, int count);
int dic_tmptouroku(struct ask_learn_word *word);
/* release an entry that dic_snssyn or dic_getsyn took */
int dic_freeentid(unsigned int entry);
/* set and get gaku_mode. dic_learn, dic_newlearn and dic_tmptouroku learn
 * while it is 1, and do nothing while it is 0. dic_setgaku refuses any other
 * mode */
int dic_setgaku(int mode);
int dic_getgaku(void);
/* do nothing, and return ASK_DIC_DONE */
int dic_clearmem(void);
#endif
