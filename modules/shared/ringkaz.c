/* RINGKAZ in a stage's object list: a ring that turns about its z axis. a
 * texture scrolls on its child. it plays one sound as a player comes near,
 * and another while a player is in range */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/texture_anim.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/ringkaz.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"

/* the sounds, of the stage's sound list. PASS_SOUND plays as a player comes
 * within PASS_RANGE. it plays again only after PASS_STEPS steps with no
 * player in range. HUM_SOUND plays at the ring while a player is within
 * HUM_RANGE */
#define PASS_SOUND 8
#define PASS_RANGE 15.0F
#define PASS_STEPS 3
#define HUM_SOUND 7
#define HUM_RANGE 300.0F
#define HUM_PRIORITY 1
#define HUM_VOLUME 80
#define HUM_STEPS 120

NJS_TEXLIST *ringkaz_texlists[] = {&ringkaz_texlist, NULL};
/* the ring's turn about z a step, which the work's timer adds up */
static Angle turn_speed = 0x90;
/* the scroll's speeds in u and v, a step. the draw reads each as a Sint16 */
static Sint32 scroll_u_speed = 6;
static Sint32 scroll_v_speed = 7;

static void ringkaz_destroy(struct task *task);
static void ringkaz_step(struct task *task);
static void ringkaz_draw(struct task *task);

void ringkaz_init(struct task *task)
{
    if (!placement_try_leave(task)) {
        task->draw = ringkaz_draw;
        task->step = ringkaz_step;
        task->destructor = ringkaz_destroy;
    }
}

static void ringkaz_destroy(struct task *task)
{
}

/* the work's tick holds PASS_STEPS while a player is within PASS_RANGE. it
 * counts down to 0 once none is */
static void ringkaz_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!placement_try_leave(task)) {
        /* @hack redundant: a block no path reaches, kept for the register
         * the step saves. the image holds none of its code, so its local and
         * its two draws are a stand-in */
        if (0) {
            NJS_CNK_OBJECT *object = &ringkaz_object;

            njCnkEasyDrawModel(object->model);
            njCnkEasyDrawModel(object->child->model);
        }
        work->timer += turn_speed;
        if (player_near(&work->pos, PASS_RANGE)) {
            if (!work->tick) {
                sound_play(PASS_SOUND, NULL, 0, 0);
            }
            work->tick = PASS_STEPS;
        } else if (work->tick) {
            work->tick--;
        }
        if (player_near(&work->pos, HUM_RANGE)) {
            /* @hack type: the first key cast through Sint32, where (void *)
             * alone, as in the call after it, would do */
            sound_play_at_for(HUM_SOUND, (void *)(Sint32)ringkaz_step,
                              HUM_PRIORITY, HUM_VOLUME, HUM_STEPS, &work->pos);
            sound_pitch_by_pos(HUM_SOUND, (void *)ringkaz_step, &work->pos);
        }
    }
}

static void ringkaz_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_CNK_OBJECT *object;

    /* @hack redundant: a block no path reaches, which leaves its strings in
     * the object */
    if (0) {
        (void)"<- ANGLE X";
        (void)"<- ANGLE Y";
        (void)"<- ANGLE Z";
    }
    njSetTexture(&ringkaz_texlist);
    object = &ringkaz_object;
    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateEx(&work->ang.x, XYZ_TURN);
    njRotateZ(NULL, work->timer);
    njCnkEasyDrawModel(object->model);
    object = object->child;
    texture_anim_scroll(object->model, &ringkaz_anim,
                        ((Sint16)game_state.steps * (Sint16)scroll_u_speed) &
                            (SCROLL_WRAP - 1),
                        ((Sint16)game_state.steps * (Sint16)scroll_v_speed) &
                            (SCROLL_WRAP - 1));
    njCnkEasyDrawModel(object->model);
    njPopMatrixEx();
}
