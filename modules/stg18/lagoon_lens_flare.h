/* Dry Lagoon's lens flare, EFLENSF0 in its object list. it is the flare of
 * shared/canyon_lens_flare.h, hidden by a test of its own */
#ifndef STG18_LAGOON_LENS_FLARE_H
#define STG18_LAGOON_LENS_FLARE_H

struct task;

/* do what canyon_lens_flare_init does, with a list of rivals of its own and
 * its own test for hiding. the flare hides while camera 0's land_block_mask
 * holds SOUTH_EAST_BLOCKS, LOW_BLOCKS or HALL_BLOCKS of stg18/lagoon_util.h,
 * whichever camera draws */
void lagoon_lens_flare_init(struct task *task);

#endif
