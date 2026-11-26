/* the big splash: a column of water that leaps up and falls outward, with its
 * drops and ripples */
#ifndef SHARED_BIG_SPLASH_H
#define SHARED_BIG_SPLASH_H
#include <ninja.h>
#include <sg_xpt.h>

#include "system/angle.h"

/* load the big splash's textures, and release them. the load reads BSPLASHDL on
 * STAGE_DRY_LAGOON. it reads BSPLASHLB1 on STAGE_BOSS_LAST1, STAGE_RELAY_SONIC,
 * STAGE_RELAY_ROUGE and STAGE_RELAY_KNUCKLES. it reads BSPLASH on every other
 * stage */
void big_splash_load_texture(void);
void big_splash_release_texture(void);

/* make a big splash at pos: a column of water, its drops and its ripples. the
 * column and the drops draw with big_splash_load_texture's textures. a part the
 * heap can't give is left out.
 *
 * a size above 0 makes the column, a task named Exec at task level 3. its model
 * draws size times as wide, and 0.8 times speed as high. a non-null ang turns
 * it by njRotateZ, njRotateX and njRotateY, in that order. the column leaps up,
 * falls outward and fades out in 45 steps.
 *
 * a speed above 0 throws drops around a full turn. it throws 16 for each unit
 * of size, cut toward 0. they leave at 1.7 times the square root of speed, and
 * last 51 steps. where the count of drops is 0, the call divides by 0, which
 * sets errno.
 *
 * where player_hooks.ripple isn't null, a speed above 0 also makes three
 * ripples at pos. they start at 1.1, 1.2 and 1.3 times size, and a faster
 * splash's ripples grow faster */
void big_splash_create(NJS_POINT3 *pos, struct angles *ang, Float size,
                       Float speed);

#endif
