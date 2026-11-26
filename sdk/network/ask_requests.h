#ifndef ASK_REQUESTS_H
#define ASK_REQUESTS_H
/* the commands api_funcent runs. a request is one of the records below,
 * whose first word is the command's number. api_funcent returns the
 * command's result, or -1 for a number outside its table */
enum ask_command {
    ASK_ROMAN_TO_KATAKANA = 15,
    ASK_ROMAN_TO_HIRAGANA = 16,
    ASK_CONVERT = 19,
    ASK_PREVIOUS_CANDIDATE = 20,
    ASK_NEXT_CANDIDATE = 21,
    ASK_PREVIOUS_GROUP = 22,
    ASK_NEXT_GROUP = 23,
    ASK_COMMIT_ALL = 24,
    ASK_COMMIT_FIRST = 25,
    ASK_OPEN_DICTIONARY = 28,
    ASK_CLOSE_DICTIONARY = 29,
    ASK_ADD_WORD = 30,
    ASK_DELETE_WORD = 31,
    ASK_SET_DICTIONARY_NAME = 32,
    ASK_EXTEND_PHRASE = 33,
    ASK_SHORTEN_PHRASE = 34,
    ASK_PREVIOUS_PHRASE = 35,
    ASK_NEXT_PHRASE = 36,
    ASK_FIRST_KATAKANA = 37,
    ASK_FIRST_HIRAGANA = 38,
    ASK_FIRST_HALF_WIDTH = 39,
    ASK_FIRST_FULL_WIDTH = 40
};
struct ask_string_pair {
    int command;
    char *first;
    char *second;
};
struct ask_conversion {
    int command;
    char *reading;
    char *candidate;
    char *suffix;
};
struct ask_commit {
    int command;
    char *text;
};
struct ask_first_commit {
    int command;
    int index;
    char *text;
    char *candidate;
    char *suffix;
    short *count;
};
struct ask_registration {
    int command;
    int dictionary;
    char *reading;
    char *word;
    int category;
};

int api_funcent(void *request);
#endif
