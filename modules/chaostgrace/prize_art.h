/* the models of the twelve toys a race awards, by toy. one table holds
 * personal toys, and one the garden's. the kind race_present_toy is given
 * picks the table */
#ifndef CHAOSTGRACE_PRIZE_ART_H
#define CHAOSTGRACE_PRIZE_ART_H
#include <ninja.h>
#include <sg_xpt.h>

enum { RACE_PRIZES = 12 };
struct personal_toy {
    NJS_CNK_OBJECT *model;
    Sint32 reserved;
};

extern NJS_CNK_OBJECT *race_garden_toys[RACE_PRIZES];
extern struct personal_toy race_personal_toys[RACE_PRIZES];

#endif
