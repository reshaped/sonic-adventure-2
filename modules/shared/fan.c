/* the fan, FAN in Wild Canyon's object list: a stand whose blades turn. it
 * comes bright or dark, and plays a sound while a player is near */
#include <ninja.h>
#include <sg_xpt.h>

#include "object/set.h"
#include "player/player.h"
#include "shared/canyon_util.h"
#include "shared/fan.h"
#include "sound/sound.h"
#include "system/game_state.h"
#include "system/port.h"
#include "system/task.h"

#include "shared/fan/text.inc"

/* the kinds. the whole part of |scl.x|, modulo FAN_KINDS, picks one */
enum fan_kind { FAN_BRIGHT, FAN_DARK, FAN_KINDS };
#define KIND(work) ((Sint32)njAbs((work)->scl.x) % FAN_KINDS)

/* the blades' turn about z, which each step adds the work's ang.z to */
#define TURN(task) ((task)->unk_3c.l)

/* a step plays FAN_SOUND for FAN_SOUND_STEPS + 1 steps where a player lies
 * nearer than fan_sound_range */
#define FAN_SOUND 8
#define FAN_SOUND_STEPS 15

/* Fan. */

/* the placement editor's labels for the fan's pitch, heading, speed, kind
 * and scale, which no code reads. each size rounds its label up to whole
 * words, so what follows starts on a word boundary */
static const char fan_edit_pitch[20] = "<- ANGLE X (   )";
static const char fan_edit_yaw[20] = "<- ANGLE Y (   )";
static const char fan_edit_speed[12] = "<- SPEED";
static const char fan_edit_variant[16] = "<- FAN (      )";
static const char fan_edit_scale_xy[20] = "<- SCALE X&Y(1.0+x)";
static const char fan_edit_scale_z[20] = "<- SCALE Z(1.0+x)";

static NJS_TEXNAME fan_bright_blades_texlist_names[3] = {
#include "shared/fan/fan_bright_blades_texlist_names.inc"
};

static NJS_TEXLIST fan_bright_blades_texlist = {
#include "shared/fan/fan_bright_blades_texlist.inc"
};

static Sint16 fan_bright_blades_polygons[917] = {
#include "shared/fan/fan_bright_blades_polygons.inc"
};

static Sint32 fan_bright_blades_vertices[1149] = {
#include "shared/fan/fan_bright_blades_vertices.inc"
};

static NJS_CNK_MODEL fan_bright_blades_model = {
#include "shared/fan/fan_bright_blades_model.inc"
};

static NJS_CNK_OBJECT fan_bright_blades_object = {
#include "shared/fan/fan_bright_blades_object.inc"
};

static NJS_TEXNAME fan_bright_stand_texlist_names[3] = {
#include "shared/fan/fan_bright_stand_texlist_names.inc"
};

static NJS_TEXLIST fan_bright_stand_texlist = {
#include "shared/fan/fan_bright_stand_texlist.inc"
};

static Sint16 fan_bright_stand_polygons[121] = {
#include "shared/fan/fan_bright_stand_polygons.inc"
};

static Sint32 fan_bright_stand_vertices[83] = {
#include "shared/fan/fan_bright_stand_vertices.inc"
};

static NJS_CNK_MODEL fan_bright_stand_model = {
#include "shared/fan/fan_bright_stand_model.inc"
};

static NJS_CNK_OBJECT fan_bright_stand_object = {
#include "shared/fan/fan_bright_stand_object.inc"
};

static NJS_TEXNAME fan_dark_blades_texlist_names[3] = {
#include "shared/fan/fan_dark_blades_texlist_names.inc"
};

static NJS_TEXLIST fan_dark_blades_texlist = {
#include "shared/fan/fan_dark_blades_texlist.inc"
};

static Sint16 fan_dark_blades_polygons[917] = {
#include "shared/fan/fan_dark_blades_polygons.inc"
};

static Sint32 fan_dark_blades_vertices[763] = {
#include "shared/fan/fan_dark_blades_vertices.inc"
};

static NJS_CNK_MODEL fan_dark_blades_model = {
#include "shared/fan/fan_dark_blades_model.inc"
};

static NJS_CNK_OBJECT fan_dark_blades_object = {
#include "shared/fan/fan_dark_blades_object.inc"
};

static NJS_TEXNAME fan_dark_stand_texlist_names[2] = {
#include "shared/fan/fan_dark_stand_texlist_names.inc"
};

static NJS_TEXLIST fan_dark_stand_texlist = {
#include "shared/fan/fan_dark_stand_texlist.inc"
};

static Sint16 fan_dark_stand_polygons[103] = {
#include "shared/fan/fan_dark_stand_polygons.inc"
};

static Sint32 fan_dark_stand_vertices[75] = {
#include "shared/fan/fan_dark_stand_vertices.inc"
};

static NJS_CNK_MODEL fan_dark_stand_model = {
#include "shared/fan/fan_dark_stand_model.inc"
};

static NJS_CNK_OBJECT fan_dark_stand_object = {
#include "shared/fan/fan_dark_stand_object.inc"
};

static void fan_destroy(struct task *task);
static void fan_step(struct task *task);
static void fan_draw(struct task *task);

KEPT static NJS_CNK_MODEL *fan_blades_models[FAN_KINDS] = {
#include "shared/fan/fan_blades_models.inc"
};
NJS_TEXLIST *fan_blades_texlists[FAN_KINDS + 1] = {
#include "shared/fan/fan_blades_texlists.inc"
};
KEPT static NJS_CNK_MODEL *fan_stand_models[FAN_KINDS] = {
#include "shared/fan/fan_stand_models.inc"
};
NJS_TEXLIST *fan_stand_texlists[FAN_KINDS + 1] = {
#include "shared/fan/fan_stand_texlists.inc"
};
/* the kinds' names */
static char *fan_names[FAN_KINDS] = {
#include "shared/fan/fan_names.inc"
};
static Sint32 fan_sound_volume = 90;
static Float fan_sound_range = 200.0F;

void fan_init(struct task *task)
{
    if (!half_try_leave(task) && !placement_try_leave(task)) {
        task->draw = fan_draw;
        task->step = fan_step;
        task->destructor = fan_destroy;
    }
}

static void fan_destroy(struct task *task)
{
}

static void fan_step(struct task *task)
{
    struct task_work *work = task->work;

    if (!half_try_leave(task) && !placement_try_leave(task)) {
        /* @hack redundant: a block no path reaches, a stand-in kept for the
         * stack frame it gives the step */
        if (0) {
            struct task_work *scaled = task->work;

            njScale(NULL, scaled->scl.x + 1.0F, scaled->scl.y + 1.0F,
                    scaled->scl.z + 1.0F);
        }
        if (njDistanceP2P(&player_work[0]->pos, &work->pos) < fan_sound_range ||
            (game_state.two_players &&
             njDistanceP2P(&player_work[1]->pos, &work->pos) <
                 fan_sound_range)) {
            sound_play_task_at_for(FAN_SOUND, task, 0, fan_sound_volume,
                                   FAN_SOUND_STEPS);
        }
        /* @hack spelling: task->work read again, where work would do */
        TURN(task) += task->work->ang.z;
    }
}

static void fan_draw(struct task *task)
{
    struct task_work *work = task->work;

    njPushMatrixEx();
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    njRotateX(NULL, work->ang.x);
    /* @hack spelling: task->work read again for scl, where work would do */
    njScale(NULL, task->work->scl.y + 1.0F, task->work->scl.y + 1.0F,
            task->work->scl.z + 1.0F);
    njSetTexture(fan_stand_texlists[KIND(task->work)]);
    njCnkEasyDrawModel(fan_stand_models[KIND(task->work)]);
    njRotateZ(NULL, TURN(task));
    njSetTexture(fan_blades_texlists[KIND(task->work)]);
    njCnkEasyDrawModel(fan_blades_models[KIND(task->work)]);
    njPopMatrixEx();
}
