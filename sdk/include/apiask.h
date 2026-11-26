/* ASK's kana-to-kanji conversion, a call for each of api_funcent's commands,
 * numbered as it numbers them. a call that steps through candidates or phrases
 * writes the candidate shown to candidate. it writes the reading left to
 * convert to suffix. both are Shift-JIS strings */
#ifndef APIASK_H
#define APIASK_H
int apiask_15_RomaToKata(char *input, char *output);
int apiask_16_RomaToHira(char *input, char *output);
int apiask_19_Henkan(char *reading, char *candidate, char *suffix);
/* apiask_20_PrevKouho and apiask_21_NextKouho step to the previous or next
 * candidate, wrapping round. each returns the candidate's place in its group,
 * from 1. without a conversion from apiask_19_Henkan to choose in, each
 * returns 0. a group holds at most ASK_MOST_IN_GROUP candidates */
#define ASK_MOST_IN_GROUP 9
int apiask_20_PrevKouho(char *candidate, char *suffix);
int apiask_21_NextKouho(char *candidate, char *suffix);
int apiask_22_PrevKouhoBlock(char *candidate, char *suffix);
int apiask_23_NextKouhoBlock(char *candidate, char *suffix);
int apiask_24_AllKakutei(char *text);
int apiask_25_FirstKakutei(int index, char *text, char *candidate, char *suffix,
                           short *count);
int apiask_28_OpenDic(void);
int apiask_29_CloseDic(void);
/* apiask_30_TourokuAdd and apiask_31_TourokuDelete register a word in, or
 * delete it from, the main dictionary. category names the word's kind, from 1
 * to ASK_CATEGORIES. each returns one of the ASK_REGISTER_ results */
#define ASK_CATEGORIES 30
enum {
    ASK_REGISTER_DONE = 0,
    ASK_REGISTER_BAD_DICTIONARY = 1, /* a dictionary other than 0 or 1 */
    ASK_REGISTER_BAD_CATEGORY = 2, /* a category outside 1 to ASK_CATEGORIES */
    ASK_REGISTER_BAD_READING = 3, /* a reading that isn't Shift-JIS kana */
    /* a conversion under way, or the dictionary's refusal */
    ASK_REGISTER_REFUSED = 4,
    ASK_REGISTER_MISSING = -1 /* a word to delete that isn't there */
};
int apiask_30_TourokuAdd(int dictionary, char *reading, char *word,
                         int category);
int apiask_31_TourokuDelete(int dictionary, char *reading, char *word,
                            int category);
int apiask_32_SetDicName(char *main_name, char *sub_name);
int apiask_33_LongerKouho(char *candidate, char *suffix);
int apiask_34_ShorterKouho(char *candidate, char *suffix);
int apiask_35_PrevBunsetu(char *candidate, char *suffix);
int apiask_36_NextBunsetu(char *candidate, char *suffix);
int apiask_37_FirstHenkanToKata(char *candidate, char *suffix);
int apiask_38_FirstHenkanToHira(char *candidate, char *suffix);
int apiask_39_FirstHenkanToHan(char *candidate, char *suffix);
int apiask_40_FirstHenkanToZen(char *candidate, char *suffix);
#endif
