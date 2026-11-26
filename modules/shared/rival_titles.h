/* Sonic and Shadow's intro titles and the letter pieces each presenter uses. */
#ifndef SHARED_RIVAL_TITLES_H
#define SHARED_RIVAL_TITLES_H

#include "shared/boss_title.h"

static struct boss_title_letter title_letters[11] = {
#include "shared/rival_titles/title_letters.inc"
};

static struct boss_title title_sonic = {
#include "shared/rival_titles/title_sonic.inc"
};

static struct boss_title title_shadow = {
#include "shared/rival_titles/title_shadow.inc"
};

#endif
