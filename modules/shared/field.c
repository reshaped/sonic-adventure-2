/* fields: what a stage gives at a point, such as a speed for the things
 * there */
#include <ninja.h>
#include <sg_xpt.h>

#include "shared/field.h"
#include "system/task.h"

#ifdef NONMATCHING
/* memcpy, with which this side copies the callback's bytes */
#include <string.h>
#else
#include "system/areas.h"

/* the bits of an address that name its megabyte */
#define MEGABYTE_BITS 0xfff00000
#endif

#define MIN_LENGTH 0.0001F
#ifdef NONMATCHING
/* ISO C defines no conversion of a function pointer to an object pointer,
 * such as task->motion.p. so this side copies the callback's bytes there.
 * callback_fits checks at compile time that the bytes fit. where the two
 * pointers' sizes differ, its size is -1, which fails the build */
typedef char callback_fits[sizeof(field_callback *) == sizeof(void *) ? 1 : -1];
#else
#define CALLBACK(field) (*(field_callback **)&(field)->motion)
#endif
#define NEXT(field) ((field)->extra.task)

static struct task *field_tasks = NULL;

void field_add(struct task *task, field_callback *callback)
{
#ifdef NONMATCHING
    /* this side adds no field without a callback. nor does it add one to a task
     * that already holds a link or a callback. the matching side spins for good
     * on a linked task */
    if (callback == NULL || task->motion.p != NULL || task->extra.p != NULL) {
        return;
    }
    NEXT(task) = field_tasks;
    memcpy(&task->motion, &callback, sizeof(callback));
#else
    if (NEXT(task) != NULL) {
        for (;;) {
        }
    }
    NEXT(task) = field_tasks;
    CALLBACK(task) = callback;
#endif
    field_tasks = task;
}

void field_remove(struct task *task)
{
    struct task *at;
    struct task *previous = NULL;

    for (at = field_tasks; at != NULL; at = NEXT(at)) {
        if (at == task) {
            if (at == field_tasks) {
                field_tasks = NEXT(at);
            } else {
                NEXT(previous) = NEXT(at);
            }
            break;
        }
        previous = at;
    }
#ifdef NONMATCHING
    /* this side leaves a task the list doesn't hold as it is */
    if (at == NULL) {
        return;
    }
#endif
    NEXT(task) = NULL;
#ifdef NONMATCHING
    /* this side clears the callback's bytes in task->motion */
    task->motion.p = NULL;
#else
    CALLBACK(task) = NULL;
#endif
}

void field_sample_at(NJS_POINT3 *pos, struct field_sample *sample)
{
    struct task *task;
    Float length;
    Float scale;
#ifdef NONMATCHING
    /* the callback, which this side copies out of task->motion */
    field_callback *callback;
#else
    Float *coord;
#endif

    sample->flags = 0;
    sample->spd.x = 0.0F;
    sample->spd.y = 0.0F;
    sample->spd.z = 0.0F;
    sample->gravity = sample->spd;
    for (task = field_tasks; task != NULL; task = NEXT(task)) {
#ifdef NONMATCHING
        /* the list is this image's own, and a stage destroys its tasks
         * before the loader replaces the image. so this side calls each
         * callback at once. the original spins first until the callback
         * lies in the stage slot */
        memcpy(&callback, &task->motion, sizeof(callback));
        callback(task, pos, sample);
#else
        /* @hack type: the slot as the array stage_slot, where the constant
         * STAGE_SLOT would do */
        while (((Uint32)CALLBACK(task) & MEGABYTE_BITS) != (Uint32)stage_slot) {
        }
        CALLBACK(task)(task, pos, sample);
#endif
    }
    if (sample->flags & FIELD_GRAVITY) {
        length = njScalor(&sample->gravity);
        if (length > MIN_LENGTH) {
            scale = 1.0F / length;
#ifdef NONMATCHING
            /* the plain form the hack tag names */
            sample->gravity.x *= scale;
            sample->gravity.y *= scale;
            sample->gravity.z *= scale;
#else
            /* @hack spelling: gravity walked as Floats, where member reads
             * would do */
            coord = &sample->gravity.x;
            *coord++ *= scale;
            *coord++ *= scale;
            *coord *= scale;
#endif
        }
    }
}
