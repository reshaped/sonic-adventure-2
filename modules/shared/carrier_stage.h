/* Weapons Bed, its two-player deck and the first walker duel */
#ifndef SHARED_CARRIER_STAGE_H
#define SHARED_CARRIER_STAGE_H
#include <ninja.h>

#include "object/set.h"

struct path;
extern struct path *carrier_paths[];

struct task;
/* the stage's callbacks, which carrier_stage.c, stg15/weapons_bed_2p.c and
 * stg20/versus_mech_1.c each compile from carrier_stage_task.h. in the duel,
 * carrier_stage_step holds the boss camera on a point above the rival. in
 * either Weapons Bed it does nothing */
void carrier_stage_init(void);
void carrier_stage_exit(void);
void carrier_stage_step(struct task *task);
extern struct object_list carrier_objects;
extern NJS_CNK_OBJECT carrier_sky[];

#endif
