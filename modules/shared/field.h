/* fields: what a stage gives at a point, such as a speed for the things
 * there */
#ifndef SHARED_FIELD_H
#define SHARED_FIELD_H
#include <ninja.h>
#include <sg_xpt.h>

struct task;

/* what the fields give at a point. under FIELD_GRAVITY in flags, gravity is
 * the way down there. under FIELD_SPD, spd is a speed for what lies there.
 * FIELD_KILLS marks a point that kills */
#define FIELD_GRAVITY 0x1
#define FIELD_SPD 0x2
/* FIELD_LIFTS marks a point where stg24_step raises a player by its gravity */
#define FIELD_LIFTS 0x4
#define FIELD_KILLS 0x8
struct field_sample {
    Uint32 flags;
    NJS_VECTOR gravity;
    NJS_VECTOR spd;
};

/* the callback field_add gives a task. field_sample_at calls it with that
 * task, the point it samples and the sample it fills */
typedef void field_callback(struct task *task, NJS_POINT3 *pos,
                            struct field_sample *sample);

/* make task a field with callback, and take it off the fields again. a field
 * keeps callback in task->motion and the next field in task->extra, until
 * field_remove clears both. field_remove of a task that is no field clears
 * them too. each image that links this source has fields of its own.
 *
 * task->extra must be null, or field_add hangs. the oldest field's extra
 * stays null, so adding it again makes a ring, which hangs field_sample_at.
 *
 * the GCC build adds no task whose motion or extra is set, nor a null
 * callback. its field_remove leaves a task that is no field as it is */
void field_add(struct task *task, field_callback *callback);
void field_remove(struct task *task);

/* clear sample's flags and both its vectors. then call each field's callback
 * on pos and sample, from the last added. where FIELD_GRAVITY is then set,
 * scale gravity to unit length, unless it is 0.0001 long or less.
 *
 * a callback must lie in the stage slot's first megabyte, or the call hangs.
 * system/areas.h's STAGE_SLOT is the slot's base. the GCC build calls any
 * callback */
void field_sample_at(NJS_POINT3 *pos, struct field_sample *sample);

#endif
