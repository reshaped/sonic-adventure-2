/* the templates the seven computer entrants are made from, and the fields
 * of each group's races. a field names a template for each entrant */
#include "chao/race.h"
#include "chao/race_opponents.h"

/* clang-format off */
struct race_template opponent_templates[115] = {
#include "chao/race_fields/opponent_templates.inc"
};

struct race_field beginner_fields[RACE_BEGINNER_COURSES][RACE_BEGINNER_LEVELS] = {
#include "chao/race_fields/beginner_fields.inc"
};

struct race_field jewel_fields[RACE_JEWEL_COURSES][RACE_JEWEL_LEVELS] = {
#include "chao/race_fields/jewel_fields.inc"
};

struct race_field challenge_fields[RACE_CHALLENGE_LEVELS] = {
#include "chao/race_fields/challenge_fields.inc"
};

struct race_field dark_fields[RACE_ALIGNMENT_LEVELS] = {
#include "chao/race_fields/dark_fields.inc"
};

/* the hero courses race the first four fields, a field a level. the twelve
 * after them name template 25 in every slot */
struct race_field hero_fields[16] = {
#include "chao/race_fields/hero_fields.inc"
};

char opponent_names_jp[10][8] = {
#include "chao/race_fields/opponent_names_jp.inc"
};

char opponent_names_en[10][8] = {
#include "chao/race_fields/opponent_names_en.inc"
};

/* clang-format on */
