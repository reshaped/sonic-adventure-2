/* King Boom Boo's sandglass, which his holder's ghost carries, and the wall
 * points about the arena's center */
#ifndef BOSS_BIGBOGY_HOURGLASS_H
#define BOSS_BIGBOGY_HOURGLASS_H
#include <ninja.h>
#include <sg_xpt.h>
struct task;

/* the sandglass's id, which sandglass_find takes */
#define BIGBOGY_SANDGLASS_ID 0

/* make the sandglass of id BIGBOGY_SANDGLASS_ID, held above the holder's
 * ghost and turned to the ghost's ang.y. its sand runs BIGBOGY_SAND_STEPS once
 * it is turned over. holder must have its ghost, unchecked. boss isn't read.
 * the call returns sandglass_create's result */
struct task *bigbogy_sandglass_create(struct task *boss, struct task *holder);
#define BIGBOGY_SAND_STEPS 1380
/* move glass with the ghost: in front of it as it dashes, and above it
 * otherwise. where the ghost is gone or its first shape is off, throw glass
 * toward the center instead. the call returns TRUE where it carries glass */
Bool bigbogy_sandglass_carry(struct task *glass, struct task *holder);
/* draw glass's shadow on the floor below it, where glass isn't null.
 * sandglass_shadow_draw draws none, so the boss's draw calls this */
void bigbogy_sandglass_shadow(struct task *glass);

/* set out[0] to out[count - 1] to points evenly about the center. they lie at
 * pos's distance from it in x and z, BIGBOGY_WALL_HEIGHT high. out[0] lies at
 * pos's bearing. each next point lies FULL_TURN / count further about y.
 * count must be above 0 */
void bigbogy_wall_points(const NJS_POINT3 *pos, Sint8 count, NJS_POINT3 *out);
#define BIGBOGY_WALL_HEIGHT 140.0F
/* set *out to the point of points[0] to points[count - 1] nearest pos in x
 * and z. of points that tie, it takes the first. count must be above 0 */
void bigbogy_nearest_wall_point(Sint8 count, const NJS_POINT3 *points,
                                const NJS_POINT3 *pos, NJS_POINT3 *out);
#endif
