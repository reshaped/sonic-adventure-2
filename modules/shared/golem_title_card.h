#ifndef SHARED_GOLEM_TITLE_CARD_H
#define SHARED_GOLEM_TITLE_CARD_H

#include "shared/boss_title.h"
#include "shared/golem_title.h"

static struct boss_title_letter title_bar[12] = {
#include "shared/golem_title_card/title_bar.inc"
};
static struct boss_title_letter title_letters[9] = {
#include "shared/golem_title_card/title_letters.inc"
};
static struct boss_title golem_title = {
#include "shared/golem_title_card/golem_title.inc"
};

#endif
