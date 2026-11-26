/* the effect object: EFFOBJ0 in a stage's object list. it throws one kind of
 * effect by chance */
#ifndef SHARED_EFFECT_OBJECT_H
#define SHARED_EFFECT_OBJECT_H

struct task;

/* what an effect object throws */
enum effect_object_kind {
    EFFECT_OBJECT_SPLASH,
    EFFECT_OBJECT_BURST,
    EFFECT_OBJECT_SPARKLE,
    EFFECT_OBJECT_SMOKE_SINKING,
    EFFECT_OBJECT_RING_SPARKLE,
    EFFECT_OBJECT_THRUST,
    EFFECT_OBJECT_DUST_SHORT,
    EFFECT_OBJECT_BREATH_SHORT,
    EFFECT_OBJECT_DIRT,
    EFFECT_OBJECT_SPARK_POINT,
    EFFECT_OBJECT_SPARK_SPRITE,
    EFFECT_OBJECT_FLAME,
    EFFECT_OBJECT_DUST,
    EFFECT_OBJECT_BREATH_FADE,
    EFFECT_OBJECT_SMOKE_RISING,
    EFFECT_OBJECT_FLASH,
    EFFECT_OBJECT_WARP_HOLE,
    EFFECT_OBJECT_BREATH_TINTED
};

/* the object list's step for EFFOBJ0. task needs a work. the low byte of its
 * ang.x is an effect_object_kind, and a byte past EFFECT_OBJECT_BREATH_TINTED
 * throws nothing. bits 8 to 15 are the chance of a throw each step, out of
 * 255. no throw comes during a time stop.
 *
 * a throw starts at pos, at the speed (0, scl.x, 0). each axis of the speed
 * gains a random part from -scl.y / 2 up to scl.y / 2. ang.y and ang.z then
 * turn the speed, as njRotateY and then njRotateZ turn a matrix. the throw's
 * scale is scl.z + 0.05, at most 20. a random factor from 0.9 up to 1.1 then
 * multiplies it.
 *
 * EFFECT_OBJECT_SPARK_POINT and EFFECT_OBJECT_SPARK_SPRITE take no scale. their
 * sparks take a spread of 0, and a floor_y 0.1 below pos.y. EFFECT_OBJECT_FLASH
 * takes no speed */
void effect_object_init(struct task *task);

#endif
