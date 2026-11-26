/* the switch, SWITCH in a stage's object list: a button that turns a switch
 * id on or off. other objects test the ids. players push its cap, and so do
 * the pressers, points other objects add. SSS in an object list is a falling
 * cylinder whose pos is among the pressers */
#ifndef SHARED_SWITCH_H
#define SHARED_SWITCH_H
#include <ninja.h>
#include <sg_xpt.h>

#include "object/switch_state.h" /* IWYU pragma: export */

struct task;

/* what a switch does with its id. a switch's lamp is lit while its id is
 * on, except where its kind says otherwise */
enum switch_kind {
    /* each press turns the id off where it is on, and on where it is off */
    SWITCH_TOGGLE,
    /* the id and the lamp are on while the switch is pressed, and off
     * otherwise */
    SWITCH_PUSH,
    /* a press turns the id on. the id goes off once the switch has stayed
     * unpressed for its time. the lamp blinks meanwhile, faster as the time
     * runs out. the first SWITCH_TIMED switch of an id to step after
     * switch_set marks the id clears the mark. where the id is on, that
     * switch lights its lamp and restarts its time. where the id is off, it
     * puts its lamp out */
    SWITCH_TIMED,
    /* a press turns the id on. no step of the switch turns it off */
    SWITCH_ON_ONLY,
    SWITCH_UNDEFINED /* acts as SWITCH_TOGGLE */
};

/* the switch's texture list, of sikake_01_128 alone. a stage's texture
 * groups hold it */
extern NJS_TEXLIST switch_texlist;

/* the pressers: up to SWITCH_PRESSERS points, besides the players', that
 * push every switch's cap. the pressers' table keeps each point by its
 * address. a point's owner must remove it before freeing it */
#define SWITCH_PRESSERS 16

/* add pos at the pressers' first free entry. it returns the entry, or null
 * where every entry is taken */
NJS_POINT3 **switch_presser_add(NJS_POINT3 *pos);
/* clear the pressers' first entry that holds pos. it returns the entry, or
 * null where none holds pos */
NJS_POINT3 **switch_presser_remove(NJS_POINT3 *pos);

/* the object list's step for SWITCH. task needs a work, whose pos and ang
 * place the switch. a step where placement_try_leave(task) returns TRUE does
 * nothing more. the first step gives task a record in task->extra, a draw
 * and a destructor. the heap must have room for the record: the step writes
 * through it unchecked. each later step moves the cap, and acts on the id as
 * the switch's kind says. nothing else may write the work's mode, submode,
 * tick or timer, which hold the switch's state.
 *
 * the kind is scl.x, truncated. a kind outside switch_kind counts as
 * SWITCH_UNDEFINED. the id is the low 16 bits of scl.y, truncated. an id of
 * SWITCH_IDS or more counts as SWITCH_IDS - 1. a SWITCH_TIMED switch's
 * time is scl.z, truncated, in steps. an scl.z not strictly between 5 and
 * 30000 gives a time of 5 steps */
void switch_step(struct task *task);

/* turn switch id on where on isn't FALSE, and off where it is, and mark the
 * id. id must be below SWITCH_IDS, unchecked */
void switch_set(Uint32 id, Bool on);
/* TRUE while switch id is on, and FALSE otherwise. id as for switch_set */
Bool switch_is_on(Uint32 id);
/* turn every id off, and clear every id's mark */
void switch_clear_all(void);

/* the object list's step for SSS. task needs a work and a motion record,
 * unchecked. the call gives task a collision record of two upright
 * cylinders, and fall_set_kind gives the work FALL_KIND_11. it adds the work's
 * pos to the pressers, unless every entry is taken. it then gives task SSS's
 * own step, draw and destructor. each later step runs fall_step_medium, unless
 * placement_try_leave(task) returns TRUE. SSS draws as debug_draw_model's
 * cylinder. its destructor removes pos from the pressers */
void sss_init(struct task *task);

#endif
