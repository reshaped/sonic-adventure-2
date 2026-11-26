/* FOGCOL, a box that brightens Lost Colony's fog while the first camera is in
 * it */
#ifndef STG27_FLASH_ZONE_H
#define STG27_FLASH_ZONE_H

struct task;

/* task needs a work. the box is centered on pos and turned by ang.y. it
 * reaches scl.x, scl.y and scl.z from pos along its own axes. while the first
 * camera is inside, the zone's level rises by 1 / n a step. it stops at 1. n
 * is ang.x's low 12 bits, or 1 where they are 0. outside, the level is 0. each
 * step the zone adds its level to colony_flash of stg27/light.h */
void flash_zone_init(struct task *task);

#endif
