#include "boss_bigbogy/settings.h"
#include "boss_bigbogy/stage.h"
#include "shared/boss_title.h"

static struct boss_title_letter bar[16] = {
#include "boss_bigbogy/title/bar.inc"
};

static struct boss_title_letter name[13] = {
#include "boss_bigbogy/title/name.inc"
};

struct boss_title bigbogy_title = {
#include "boss_bigbogy/title/bigbogy_title.inc"
};
