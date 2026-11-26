/* the gate, GATE in Cosmic Wall's object list: a solid frame with a panel
 * that scrolls. the panel lights while the gate's drill ride is under way */
#include <ninja.h>
#include <sg_xpt.h>

#include "stg43/gate/text.inc"

#include "collision/world.h"
#include "draw/texture_anim.h"
#include "object/set.h"
#include "player/player.h"
#include "sound/sound.h"
#include "stg43/drill.h"
#include "stg43/objects.h"
#include "system/task.h"

static NJS_TEXLIST gate_texlist;
static NJS_TEXLIST gate_lit_texlist;
static NJS_CNK_OBJECT gate_panel[1];
static NJS_CNK_OBJECT gate_housing[1];
static NJS_OBJECT gate_collision;
static struct texture_anim gate_scroll;

static Sint32 scroll_speed;

/* the panel's scroll, in task->unk_3c's word. the draw takes it modulo
 * SCROLL_WRAP */
#define SCROLL(task) ((task)->unk_3c.l)
/* a scl.x of SECOND_GATE or more makes a gate of the second drill ride. its
 * panel scrolls the other way */
#define SECOND_GATE 1.0F
/* the gate's copy in the world collides only while a player lies within
 * COLLIDE_RANGE of its pos */
#define COLLIDE_RANGE 50.0F
/* the sound the gate plays at its pos, and its priority, volume and time */
#define GATE_SOUND 17
#define GATE_SOUND_PRIORITY 1
#define GATE_SOUND_VOLUME 30
#define GATE_SOUND_STEPS 30

static void gate_destroy(struct task *task);
static void gate_step(struct task *task);
static void gate_draw(struct task *task);

#define SOLID_INIT cosmic_gate_init
#define SOLID_DRAW gate_draw
#define SOLID_STEP gate_step
#define SOLID_DESTROY gate_destroy
#define SOLID_COLLISION gate_collision
#include "stg43/solid_impl.h" /* IWYU pragma: keep -- the code it compiles here */

static void gate_step(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack spelling: the step's work in an else after the return, where
     * the work after the if would do */
    if (placement_try_leave(task)) {
        return;
    } else {
        if (work->scl.x >= SECOND_GATE) {
            SCROLL(task) -= scroll_speed;
            work->mode = cosmic_drill_active[COSMIC_DRILL_SECOND_RIDE];
        } else {
            SCROLL(task) += scroll_speed;
            work->mode = cosmic_drill_active[COSMIC_DRILL_FIRST_RIDE];
        }
        if (player_near(&work->pos, COLLIDE_RANGE)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
        sound_play_at_for(GATE_SOUND, task, GATE_SOUND_PRIORITY,
                          GATE_SOUND_VOLUME, GATE_SOUND_STEPS, &work->pos);
    }
}

static void gate_draw(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a block no path reaches, kept for the set editor's
     * labels, which SHC still places in this source's constants */
    if (0) {
        (void)GATE_EDITOR_ANGLE_Y;
        (void)GATE_EDITOR_NUM;
    }
    /* gate_step copies the gate's ride's entry of cosmic_drill_active into
     * the work's mode. the panel lights while it is TRUE */
    if (work->mode) {
        njSetTexture(&gate_lit_texlist);
    } else {
        njSetTexture(&gate_texlist);
    }
    texture_anim_scroll(gate_panel->model, &gate_scroll,
                        SCROLL(task) & (SCROLL_WRAP - 1),
                        SCROLL(task) & (SCROLL_WRAP - 1));
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njCnkEasyDrawModel(gate_housing->model);
    njCnkEasyDrawModel(gate_panel->model);
    njPopMatrixEx();
}

#include "stg43/foreground/gate.h"

NJS_TEXLIST *cosmic_gate_texlists[4] = {
#include "stg43/gate/cosmic_gate_texlists.inc"
};
/* the panel's scroll a step, along u and v alike */
static Sint32 scroll_speed = {
#include "stg43/gate/scroll_speed.inc"
};
