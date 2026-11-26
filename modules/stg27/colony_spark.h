/* Lost Colony's sparks: SPARK, which the stage places, and the particles the
 * spark emitter throws */
#ifndef STG27_COLONY_SPARK_H
#define STG27_COLONY_SPARK_H
#include <ninja.h>

struct particle;
struct task;

/* the sparks' textures, tx_spark001 to tx_spark012, in the order they show
 * them */
extern NJS_TEXLIST colony_spark_texlist;
/* the sparks' texture lists for texture_load_shared, ended by null */
extern NJS_TEXLIST *colony_spark_texlists[];

/* make a spark at pos, moving at spd, of size scale, and return its
 * particle. it returns null where the heap can't give one.
 *
 * a spark adds its color to what lies behind it. it slows, falls and grows.
 * its frame starts at a random 0 to 1 and gains 0.5 a step. it ends as the
 * frame reaches 12 */
struct particle *colony_spark_create(NJS_POINT3 *pos, NJS_VECTOR *spd,
                                     Float scale);
/* SPARK. task needs a work. it shows the sparks' textures in turn, scl.x of
 * them a step, and starts over after the last. it draws at pos as a square
 * 2 * (scl.y + 1) across. ang.z turns the square on the screen. it draws
 * without fog, and adds its color to what lies behind it */
void colony_spark_init(struct task *task);
/* the spark emitter. task needs a work. the low byte of its ang.x is the
 * effect, and a byte other than 0 throws nothing. bits 8 to 15 are the chance
 * of a throw each step, out of 255. no kind of lost_colony_kinds names this
 * init.
 *
 * a throw starts at pos, at the speed (0, scl.x, 0). each axis of the speed
 * gains a random part from -scl.y / 2 up to scl.y / 2. ang.y and ang.z then
 * turn the speed, as njRotateY and then njRotateZ turn a matrix. the spark's
 * scale is scl.z + 0.05, at most 20. a random factor from 0.9 up to 1.1 then
 * multiplies it */
void colony_spark_emitter_init(struct task *task);

#endif
