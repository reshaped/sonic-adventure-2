#include <apiask.h>
#include "ask_requests.h"

/* @hack spelling: each request filled through request, a pointer to its
 * storage, where the storage itself would do */
#define REQUEST(type)                                                          \
    struct type storage;                                                       \
    struct type *request = &storage

int apiask_15_RomaToKata(char *input, char *output)
{
    REQUEST(ask_string_pair);
    request->command = ASK_ROMAN_TO_KATAKANA;
    request->first = input;
    request->second = output;
    return api_funcent(request);
}

int apiask_16_RomaToHira(char *input, char *output)
{
    REQUEST(ask_string_pair);
    request->command = ASK_ROMAN_TO_HIRAGANA;
    request->first = input;
    request->second = output;
    return api_funcent(request);
}

int apiask_19_Henkan(char *reading, char *candidate, char *suffix)
{
    REQUEST(ask_conversion);
    request->command = ASK_CONVERT;
    request->reading = reading;
    request->candidate = candidate;
    request->suffix = suffix;
    return api_funcent(request);
}

int apiask_20_PrevKouho(char *candidate, char *suffix)
{
    REQUEST(ask_string_pair);
    request->command = ASK_PREVIOUS_CANDIDATE;
    request->first = candidate;
    request->second = suffix;
    return api_funcent(request);
}

int apiask_21_NextKouho(char *candidate, char *suffix)
{
    REQUEST(ask_string_pair);
    request->command = ASK_NEXT_CANDIDATE;
    request->first = candidate;
    request->second = suffix;
    return api_funcent(request);
}

int apiask_22_PrevKouhoBlock(char *candidate, char *suffix)
{
    REQUEST(ask_string_pair);
    request->command = ASK_PREVIOUS_GROUP;
    request->first = candidate;
    request->second = suffix;
    return api_funcent(request);
}

int apiask_23_NextKouhoBlock(char *candidate, char *suffix)
{
    REQUEST(ask_string_pair);
    request->command = ASK_NEXT_GROUP;
    request->first = candidate;
    request->second = suffix;
    return api_funcent(request);
}

int apiask_24_AllKakutei(char *text)
{
    REQUEST(ask_commit);
    request->command = ASK_COMMIT_ALL;
    request->text = text;
    return api_funcent(request);
}

int apiask_25_FirstKakutei(int index, char *text, char *candidate, char *suffix,
                           short *count)
{
    REQUEST(ask_first_commit);
    request->command = ASK_COMMIT_FIRST;
    request->index = index;
    request->text = text;
    request->candidate = candidate;
    request->suffix = suffix;
    request->count = count;
    return api_funcent(request);
}

int apiask_28_OpenDic(void)
{
    int command = ASK_OPEN_DICTIONARY;
    return api_funcent(&command);
}

int apiask_29_CloseDic(void)
{
    int command = ASK_CLOSE_DICTIONARY;
    return api_funcent(&command);
}

int apiask_30_TourokuAdd(int dictionary, char *reading, char *word,
                         int category)
{
    REQUEST(ask_registration);
    request->command = ASK_ADD_WORD;
    request->dictionary = dictionary;
    request->reading = reading;
    request->word = word;
    request->category = category;
    return api_funcent(request);
}

int apiask_31_TourokuDelete(int dictionary, char *reading, char *word,
                            int category)
{
    REQUEST(ask_registration);
    request->command = ASK_DELETE_WORD;
    request->dictionary = dictionary;
    request->reading = reading;
    request->word = word;
    request->category = category;
    return api_funcent(request);
}

int apiask_32_SetDicName(char *main_name, char *sub_name)
{
    REQUEST(ask_string_pair);
    request->command = ASK_SET_DICTIONARY_NAME;
    request->first = main_name;
    request->second = sub_name;
    return api_funcent(request);
}

int apiask_33_LongerKouho(char *candidate, char *suffix)
{
    REQUEST(ask_string_pair);
    request->command = ASK_EXTEND_PHRASE;
    request->first = candidate;
    request->second = suffix;
    return api_funcent(request);
}

int apiask_34_ShorterKouho(char *candidate, char *suffix)
{
    REQUEST(ask_string_pair);
    request->command = ASK_SHORTEN_PHRASE;
    request->first = candidate;
    request->second = suffix;
    return api_funcent(request);
}

int apiask_35_PrevBunsetu(char *candidate, char *suffix)
{
    REQUEST(ask_string_pair);
    request->command = ASK_PREVIOUS_PHRASE;
    request->first = candidate;
    request->second = suffix;
    return api_funcent(request);
}

int apiask_36_NextBunsetu(char *candidate, char *suffix)
{
    REQUEST(ask_string_pair);
    request->command = ASK_NEXT_PHRASE;
    request->first = candidate;
    request->second = suffix;
    return api_funcent(request);
}

int apiask_37_FirstHenkanToKata(char *candidate, char *suffix)
{
    REQUEST(ask_string_pair);
    request->command = ASK_FIRST_KATAKANA;
    request->first = candidate;
    request->second = suffix;
    return api_funcent(request);
}

int apiask_38_FirstHenkanToHira(char *candidate, char *suffix)
{
    REQUEST(ask_string_pair);
    request->command = ASK_FIRST_HIRAGANA;
    request->first = candidate;
    request->second = suffix;
    return api_funcent(request);
}

int apiask_39_FirstHenkanToHan(char *candidate, char *suffix)
{
    REQUEST(ask_string_pair);
    request->command = ASK_FIRST_HALF_WIDTH;
    request->first = candidate;
    request->second = suffix;
    return api_funcent(request);
}

int apiask_40_FirstHenkanToZen(char *candidate, char *suffix)
{
    REQUEST(ask_string_pair);
    request->command = ASK_FIRST_FULL_WIDTH;
    request->first = candidate;
    request->second = suffix;
    return api_funcent(request);
}
