/* the garden's small animals, which a player lets out of the item stock */
#ifndef CHAO_MINIMAL_H
#define CHAO_MINIMAL_H
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/motion.h"
#include "object/animal.h"
#include "system/task.h"

/* an animal's mode, its motion table's clips and its collision shapes */
enum minimal_mode { MINIMAL_ROAM, MINIMAL_HELD, MINIMAL_CARRIED, MINIMAL_EXIT };
enum minimal_clip {
    MINIMAL_IDLE,
    MINIMAL_WALK,
    MINIMAL_CARRY,
    MINIMAL_FLY,
    MINIMAL_CLIPS
};
enum minimal_shape {
    MINIMAL_PICKUP,
    MINIMAL_CONTACT,
    MINIMAL_LOWER,
    MINIMAL_SHAPES
};

struct garden_minimal_work {
    struct task_work base;
    Float speed; /* the roam's walking and swimming speed */
    /* the frames a released animal may yet stay out of view before it leaves
     * the garden */
    Sint32 hidden_frames;
    struct motion_control motion;
    struct motion_table_entry clips[MINIMAL_CLIPS];
    Bool released; /* a Chao that carried the animal has let it go */
};
#define MINIMAL_WORK(animal) ((struct garden_minimal_work *)(animal)->work)
/* the fields of base that hold the animal's kind, an enum animal_kind, and
 * its roam's step count */
#define MINIMAL_KIND(work) ((work)->base.tick)
#define ROAM_STEPS(work) ((work)->base.ang.z)

/* each kind's model and motions */
extern NJS_CNK_OBJECT *garden_minimal_models[ANIMAL_KINDS];
extern NJS_MOTION *garden_minimal_held_motions[ANIMAL_KINDS];
extern NJS_MOTION *garden_minimal_walk_motions[ANIMAL_KINDS];
extern NJS_MOTION *garden_minimal_idle_motions[ANIMAL_KINDS];
extern NJS_MOTION *garden_minimal_fly_motions[ANIMAL_KINDS];
void garden_minimal_land(struct task *animal);
/* make an animal of kind at position, facing yaw and moving at velocity. it
 * comes to rest on a floor before it wanders */
#ifdef NONMATCHING
/* this side returns FALSE where it can't allocate the animal or register it */
Bool garden_minimal_create(Sint8 kind, NJS_POINT3 *position, Angle yaw,
                           NJS_VECTOR *velocity);
#else
void garden_minimal_create(Sint8 kind, NJS_POINT3 *position, Angle yaw,
                           NJS_VECTOR *velocity);
#endif
/* fill clips with MINIMAL_CLIPS entries that play kind's motions */
void garden_minimal_motion_table(struct motion_table_entry *clips, Uint32 kind);
/* the roaming step of every kind */
void garden_minimal_roam(struct task *task);

#endif
