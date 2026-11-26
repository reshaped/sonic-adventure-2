/* the kinds that a stage's model objects choose among. a kind is a model with
 * its collision shapes, draw and build */
#ifndef SHARED_MODEL_KIND_H
#define SHARED_MODEL_KIND_H
#include <ninja.h>
#include <sg_xpt.h>

struct collision_shape;
struct task;
struct task_work;

/* the bits of model_kind's flags */
#define KIND_BUILD 0x1 /* the stage builds its model */
/* an object of the kind keeps its place and turn in a matrix of its own */
#define KIND_MATRIX 0x2
/* the kind's whole tree builds and draws, not its first model alone */
#define KIND_TREE 0x4
/* shapes holds two sets of shape_count shapes, and the object turns the
 * second set off */
#define KIND_DOUBLE_SHAPES 0x8
/* Lost Colony's build compiles the kind's model with fog off */
#define KIND_NO_FOG 0x10
/* City Escape's road objects build and draw this kind under
 * DRAW_CONTROL_SHADOW */
#define KIND_SHADOWED 0x20
#define KIND_COMPILED 0x8000 /* built holds the compiled model */

/* what a kind draws in place of its own model while the object's submode is
 * nonzero */
struct model_alternate {
    NJS_TEXLIST *texlist;
    NJS_CNK_OBJECT *object;
};

struct model_kind {
    Uint32 flags;
    NJS_TEXLIST *texlist; /* the texture list to make current, or null */
    NJS_CNK_OBJECT *object;
    /* the shapes of the object's collision record, or null for none */
    struct collision_shape *shapes;
    Sint32 shape_count;
    void (*draw)(struct task *task);
    /* a step of the kind's own, or null for collision_register's */
    void (*step)(struct task *task, struct task_work *work,
                 struct model_kind *kind);
    struct model_alternate *alternate; /* null for none */
    Sint32 unk_20;
    NJS_DIRECT_HEAD *built; /* the build, once flags holds KIND_COMPILED */
};

/* the kinds one object chooses among. task->motion points at the list, and
 * the object reads its kind's index from its own placement */
struct model_kind_list {
    struct model_kind *kinds;
    Sint32 count;
};

#endif
