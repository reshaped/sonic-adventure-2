/* the lights the enemies show: glows that fade from full and end */
#include <ninja.h>
#include <sg_xpt.h>

#include "draw/color.h"
#include "draw/lines.h"
#include "object/set.h"
#include "shared/enemy_light.h"
#include "system/game_state.h"
#include "system/task.h"

/* a light keeps its state in its work's submode and scl. ALPHA falls from 1
 * by FADE a step, down to 0. the light ends at the first step that finds
 * ALPHA at 0. SCALE scales the sprite a light draws in place of its glow */
#define KIND(work) ((work)->submode)
#define ALPHA(work) ((work)->scl.x)
#define FADE(work) ((work)->scl.y)
#define SCALE(work) ((work)->scl.z)

/* each kind's FADE and SCALE. a kind outside 1 to 4 takes kind 0's */
#define KIND_0_FADE 0.1F
#define KIND_0_SCALE 0.3F
#define KIND_1_FADE 0.05F
#define KIND_1_SCALE 0.26F
#define KIND_2_FADE 0.3F
#define KIND_2_SCALE 0.26F
#define KIND_3_FADE 0.05F
#define KIND_3_SCALE 0.12F
#define KIND_4_FADE 0.05F
#define KIND_4_SCALE 0.4F

/* a light's glow, which its draw queues for the queue task to draw */
struct enemy_light_glow {
    /* the light's alpha in GLOW_ALPHA, as in an ARGB color, and its kind in
     * GLOW_KIND */
    Uint32 alpha_kind;
    NJS_POINT3 pos;
};
#define GLOW_ALPHA 0xff000000
#define GLOW_KIND 0xff
/* a glow's alpha, from 0 to 1, goes into GLOW_ALPHA as 0 to CHANNEL_MAX,
 * shifted up ALPHA_SHIFT bits */
#define CHANNEL_MAX 255.0F
/* a glow draws white, in its light's alpha, in the translucent list */
#define GLOW_WHITE 0x00ffffff
#define GLOW_TRANSLUCENT 1

#define QUEUE_LEVEL 5 /* the queue task's level */
#define LIGHT_LEVEL 3 /* the task level of a light without a parent */
#define TEXTURE_LEVEL 0 /* the texture task's level */

/* the sprite a light draws in place of its glow is one cell, CELL_SIZE pixels
 * on a side. the cell shows the whole texture, its u and v running to
 * TEXTURE_SIDE. the sprite draws with SPRITE_ATTR */
#define CELL_SIZE 16
#define TEXTURE_SIDE 256
#define SPRITE_ATTR (NJD_SPRITE_ALPHA | NJD_SPRITE_SCALE | NJD_SPRITE_COLOR)

/* the queue, whose first enemy_light_glow_count glows wait to be drawn */
#define ENEMY_LIGHT_GLOWS 512
static struct enemy_light_glow enemy_light_glows[ENEMY_LIGHT_GLOWS];

#include "shared/enemy_light/text.inc"

static NJS_TEXNAME enemy_light_texname[1] = {
#include "shared/enemy_light/enemy_light_texname.inc"
};
static NJS_TEXLIST enemy_light_texlist = {
#include "shared/enemy_light/enemy_light_texlist.inc"
};
/* the names of eyelight_texlist, a texture list that nothing uses */
static NJS_TEXNAME eyelight_texname[4] = {
#include "shared/enemy_light/eyelight_texname.inc"
};
static NJS_TEXLIST eyelight_texlist = {
#include "shared/enemy_light/eyelight_texlist.inc"
};

/* the queue task, which draws the queued glows, or null while none does */
static struct task *enemy_light_queue_task = NULL;
static Uint32 enemy_light_glow_count = 0;
/* the sprite's one cell, centered on its point */
static NJS_TEXANIM enemy_light_texanim[1] = {
#include "shared/enemy_light/enemy_light_texanim.inc"
};
static NJS_SPRITE enemy_light_sprite = {
#include "shared/enemy_light/enemy_light_sprite.inc"
};

/* the task that keeps enemy_light_texlist loaded, or null while none does.
 * its destructor releases the list */
static struct task *enemy_light_texture_task = NULL;

/* draw each queued glow, and empty the queue. a glow's half size is half a
 * cell times its kind's scale. a light's sprite takes the same size only
 * while _nj_scr_aspect_x_ and _nj_scr_aspect_y_ are 1 */
static void enemy_light_queue_draw(struct task *task)
{
    struct enemy_light_glow *glow = enemy_light_glows;
    /* @hack spelling: alpha_kind, a local for the glow's alpha_kind, where
     * glow->alpha_kind read at each use would do */
    Uint32 alpha_kind;
    /* @hack spelling: texid, set to 0 in each case, where 0 passed in place
     * would do */
    /* @hack type: texid a Uint32, where the Sint32 njPtclSpriteStart takes
     * would do */
    Uint32 texid;
    Float size;

    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    njSetTexture(&enemy_light_texlist);
    njFogDisable();
    while (enemy_light_glow_count > 0) {
        alpha_kind = glow->alpha_kind;
        switch ((Uint8)alpha_kind) {
        /* @hack spelling: kinds 1 and 2 as two cases with a body each, where
         * one body for both would do, the two scales being equal */
        case 1:
            texid = 0;
            size = (CELL_SIZE / 2.0F) * KIND_1_SCALE;
            break;
        case 2:
            texid = 0;
            size = (CELL_SIZE / 2.0F) * KIND_2_SCALE;
            break;
        case 3:
            texid = 0;
            size = (CELL_SIZE / 2.0F) * KIND_3_SCALE;
            break;
        case 4:
            texid = 0;
            size = (CELL_SIZE / 2.0F) * KIND_4_SCALE;
            break;
        /* @hack redundant: the label case 0, which default already covers */
        case 0:
        default:
            texid = 0;
            size = (CELL_SIZE / 2.0F) * KIND_0_SCALE;
            break;
        }
        njPtclSpriteStart(texid, (alpha_kind & GLOW_ALPHA) | GLOW_WHITE,
                          GLOW_TRANSLUCENT);
        njPtclDrawSprite(&glow->pos, 1, size, size);
        njPtclSpriteEnd();
        glow++;
        enemy_light_glow_count--;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static void enemy_light_queue_destroy(struct task *task)
{
    enemy_light_queue_task = NULL;
}

static void enemy_light_queue_step(struct task *task)
{
}

/* queue a glow of kind's low byte at pos, in color's alpha. the call first
 * makes the queue task where there is none. it returns FALSE where the heap
 * has no room for that task, or where the queue is full. it returns TRUE
 * otherwise */
static Bool enemy_light_queue_add(NJS_POINT3 *pos, int kind, Uint32 color)
{
    struct task *task;

    if (enemy_light_queue_task == NULL) {
        /* @hack spelling: the assignments to task and enemy_light_queue_task
         * inside the test, where statements of their own would do */
        if ((task = enemy_light_queue_task =
                 task_create(0, QUEUE_LEVEL, enemy_light_queue_step,
                             "EnemyLightPtclDrawExec")) == NULL) {
            return FALSE;
        }
        task->draw = enemy_light_queue_draw;
        task->destructor = enemy_light_queue_destroy;
    }
    if (enemy_light_glow_count < ENEMY_LIGHT_GLOWS) {
        enemy_light_glows[enemy_light_glow_count].alpha_kind =
            (color & ~GLOW_KIND) | (Uint8)kind;
        enemy_light_glows[enemy_light_glow_count].pos = *pos;
        enemy_light_glow_count++;
        return TRUE;
    }
    return FALSE;
}

/* queue the light's glow, or draw the light as a sprite where the queue can't
 * take the glow */
static void enemy_light_draw(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: scale, a local read before the queue's call, where
     * SCALE(work) read at its use would do */
    Float scale = SCALE(work);

    /* @hack spelling: the result compared with TRUE, where its truth would
     * do */
    if (enemy_light_queue_add(&work->pos, KIND(work),
                              (Uint32)(ALPHA(work) * CHANNEL_MAX)
                                  << ALPHA_SHIFT) == TRUE) {
        return;
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    constant_material_set(ALPHA(work), 1.0F, 1.0F, 1.0F);
    njSetTexture(&enemy_light_texlist);
    njFogDisable();
    enemy_light_sprite.sx = enemy_light_sprite.sy = scale;
    enemy_light_sprite.p = work->pos;
    /* @hack type: the sprite passed as the address of its p, where
     * &enemy_light_sprite would do */
    njDrawSprite3D((NJS_SPRITE *)&enemy_light_sprite.p, 0, SPRITE_ATTR);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    njFogEnable();
}

static void enemy_light_destroy(struct task *task)
{
}

static void enemy_light_step(struct task *task)
{
    struct task_work *work = task->work;

    if (task->parent == NULL && placement_try_leave(task)) {
        return;
    }
    if (!game_state.time_stopped) {
        if (ALPHA(work) <= 0.0F) {
            task_kill(task);
            return;
        }
        ALPHA(work) -= FADE(work);
        if (ALPHA(work) < 0.0F) {
            ALPHA(work) = 0.0F;
        }
        work->timer++;
    }
}

static void enemy_light_init(struct task *task)
{
    struct task_work *work = task->work;

    ALPHA(work) = 1.0F;
    switch (KIND(work)) {
    case 1:
        FADE(work) = KIND_1_FADE;
        SCALE(work) = KIND_1_SCALE;
        break;
    case 2:
        FADE(work) = KIND_2_FADE;
        SCALE(work) = KIND_2_SCALE;
        break;
    case 3:
        FADE(work) = KIND_3_FADE;
        SCALE(work) = KIND_3_SCALE;
        break;
    case 4:
        FADE(work) = KIND_4_FADE;
        SCALE(work) = KIND_4_SCALE;
        break;
    /* @hack redundant: the label case 0, which default already covers */
    case 0:
    default:
        FADE(work) = KIND_0_FADE;
        SCALE(work) = KIND_0_SCALE;
        break;
    }
    task->destructor = enemy_light_destroy;
    task->draw = enemy_light_draw;
}

static void enemy_light_texture_destroy(struct task *task)
{
    njReleaseTexture(&enemy_light_texlist);
    enemy_light_texture_task = NULL;
}

static void enemy_light_texture_step(struct task *task)
{
}

void enemy_light_load_texture(void)
{
    if (enemy_light_texture_task == NULL) {
        njLoadTexture(&enemy_light_texlist);
        enemy_light_texture_task =
            task_create(0, TEXTURE_LEVEL, enemy_light_texture_step, "ManTex");
        enemy_light_texture_task->destructor = enemy_light_texture_destroy;
    }
}

void enemy_light_create(struct task *parent, int kind, NJS_POINT3 *pos)
{
    struct task *task;

    if (parent != NULL) {
        task = task_create_child(TASK_WORK, enemy_light_step, parent);
    } else {
        task = task_create(TASK_WORK, LIGHT_LEVEL, enemy_light_step,
                           "EnemyLightExecutor");
    }
    if (task != NULL) {
        KIND(task->work) = kind;
        task->work->pos = *pos;
        enemy_light_init(task);
        enemy_light_load_texture();
    }
}
