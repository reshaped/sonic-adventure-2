/* ASK's input. it keeps the keystrokes as typed in inpc_buf, beside the kana
 * they compose in kana_buf. so a reading can be shown again, or turned back
 * into its spelling */
#ifndef ASK_INPUT_H
#define ASK_INPUT_H
#include <sg_xpt.h>

#define ASK_KANA_LIMIT 36
#define ASK_INPUT_CELLS 100
#define ASK_KANA_CELLS 39
#define ASK_READING_BYTES 40
#define ASK_DISPLAY_CELLS 152

struct ask_kana {
    Uint16 code;
    Sint32 input_bytes;
};
extern Sint32 roman_japan;
extern Sint16 ic_flag;
extern Sint32 kana_buf_size;
extern Uint16 inpc_buf[ASK_INPUT_CELLS];
extern Uint16 *pinpc_buf, *qinpc_buf, *cinpc_buf, *einpc_buf;
extern struct ask_kana kana_buf[ASK_KANA_CELLS];
extern struct ask_kana *pkana_buf, *ekana_buf;
extern Sint32 kana_len;
extern Uint8 kana_ustr[ASK_READING_BYTES];
extern Uint8 *p_ustr, *e_ustr;
extern Uint16 meanbuf[ASK_DISPLAY_CELLS];
extern Uint8 outbuf[ASK_DISPLAY_CELLS];
extern Uint16 ud_inpc_buf[ASK_INPUT_CELLS];
extern Sint32 ud_inpc_len;
extern struct ask_kana ud_kana_buf[ASK_KANA_CELLS];
extern Uint8 ud_kana_ustr[ASK_READING_BYTES];
extern Sint32 ud_kana_len;

struct ask_roman_row {
    char consonant;
    char vowel[5][2];
};
extern struct ask_roman_row rmtab[49];
extern struct ask_roman_row *prmtab[27];
extern char rmspec[];

/* convert the count letters of romaji at letters into kana at kana. *produced
 * gets the count of kana written. it returns the letters it used, or 0 where
 * they begin a spelling but don't finish one. it returns -1 where the next
 * letter may still change its kana. it returns -2 where no kana has their
 * spelling */
int to_roman(const Sint16 *letters, int count, Uint16 *kana, int *produced);
int add_kana_buf(const char *text);
int del_kana_buf(int bytes);
int count_byte_kana_buf(int first, int count);
#endif
