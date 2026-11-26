/* the lens flare, EFLENSF0 in a stage's object list: a glow at a light. its
 * ghosts draw on the line from the light through the screen's center */
#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "draw/lines.h"
#include "draw/screen.h"
#include "shared/lens_flare.h"
#include "system/task.h"

/* the level of the task lens_flare_create makes */
#define LEVEL 3
/* the slots of lens_flare_rivals */
#define RIVALS 6

/* a flare's facing is the cosine of the angle from the camera's view to the
 * light. past a facing of GLARE_FACING, a glare draws over the light. the
 * glare's strength s is sqrt((facing - GLARE_FROM) * GLARE_RATE), which runs
 * from about 0.7 to 1. its sprite scales by GLARE_SIZE times s, at an alpha of
 * GLARE_ALPHA times s */
#define GLARE_FACING 0.99F
#define GLARE_FROM 0.98F
#define GLARE_RATE 50.0F
#define GLARE_ALPHA 0.9F
#define GLARE_SIZE 20.0F
/* the ghosts' strength is 0 at a facing of STRENGTH_FROM or less. above it,
 * the strength is the fourth root of (facing - STRENGTH_FROM) * STRENGTH_RATE.
 * it reaches 1 at a facing of about 0.93. each ghost's alpha is its own times
 * the strength */
#define STRENGTH_FROM 0.6F
#define STRENGTH_RATE 3.0F
/* the cells of the glow and the glare */
#define GLOW_CELL 0
#define GLARE_CELL 7
/* the sprites' priorities. njDrawSprite2D gives a sprite of a priority p
 * below -1 the depth of a point -p away. the glow lies behind the glare, and
 * the glare behind every ghost. lens_flare_draw adds GHOST_PRIORITY to each
 * ghost's own */
#define GLOW_PRIORITY (-13.0F)
#define GLARE_PRIORITY (-12.0F)
#define GHOST_PRIORITY (-10.0F)
#define SPRITE_ATTR (NJD_SPRITE_ALPHA | NJD_SPRITE_COLOR)

/* a ghost of the flare. it draws a cell of the sprite on the line from the
 * light through the screen's center */
struct lens_flare_ghost {
    Int cell; /* the cell the ghost draws, or GHOST_END to end the table */
    /* where the ghost lies on the line: 0 at the light, 1 at the center */
    Float offset;
    Float scale; /* the ghost's scale at a strength of 1 */
    /* the part of scale the ghost loses as the strength falls from 1 to 0 */
    Float shrink;
    Float alpha; /* the ghost's alpha at a strength of 1 */
    Float priority;
};
#define GHOST_END (-1)

/* nothing calls lens_flare_create or lens_flare_draw_facing. neither is
 * static, since SHC leaves out a static function that nothing calls */
/* NOLINTBEGIN(misc-use-internal-linkage) */
/* make a flare at pos: a task named EffectLensFlare at level LEVEL, with
 * lens_flare_init as its step. the flare is no rival. where task_create
 * fails, the call does nothing */
void lens_flare_create(NJS_POINT3 *pos);
/* draw the flare of a light at pos that faces along normal. camera_task_work
 * must not be null, and normal must be a unit vector, unchecked.
 *
 * the facing is lens_flare_draw's times the cosine of the angle from normal to
 * the camera. so the flare draws where the light lies within 90 degrees of the
 * view and faces the camera. it also draws where both cosines are negative: the
 * light lies behind the camera and faces away.
 *
 * otherwise the call draws as lens_flare_draw does, but its camera is
 * camera_task_work's pos and ang. it draws no glow, and leaves Ninja's fog as
 * it was. each ghost draws at its own priority, without GHOST_PRIORITY */
void lens_flare_draw_facing(NJS_POINT3 *pos, NJS_VECTOR *normal);
/* NOLINTEND(misc-use-internal-linkage) */

static void lens_flare_step(struct task *task);
static void lens_flare_task_draw(struct task *task);
static void lens_flare_destroy(struct task *task);

#define TEXTURES 8
#include "shared/lens_flare/text.inc"

static NJS_TEXNAME lens_flare_texname[8] = {
#include "shared/lens_flare/lens_flare_texname.inc"
};
NJS_TEXLIST lens_flare_texlist = {
#include "shared/lens_flare/lens_flare_texlist.inc"
};
/* cell i takes texture i */
static NJS_TEXANIM lens_flare_cells[8] = {
#include "shared/lens_flare/lens_flare_cells.inc"
};
/* the sprite every part of a flare draws with, moved and scaled for each */
static NJS_SPRITE lens_flare_sprite = {
#include "shared/lens_flare/lens_flare_sprite.inc"
};
static struct lens_flare_ghost lens_flare_ghosts[11] = {
#include "shared/lens_flare/lens_flare_ghosts.inc"
};
/* the listed rivals, each in a slot of its own. a free slot is null */
static struct task *lens_flare_rivals[RIVALS] = {
    NULL, NULL, NULL, NULL, NULL, NULL,
};

/* put task in a free slot of lens_flare_rivals, and return TRUE. it returns
 * FALSE where no slot is free */
static Bool lens_flare_add_rival(struct task *task)
{
    int i;

    for (i = 0; i < RIVALS; i++) {
        if (lens_flare_rivals[i] == NULL) {
            lens_flare_rivals[i] = task;
            return TRUE;
        }
    }
    return FALSE;
}

static void lens_flare_remove_rival(struct task *task)
{
    int i;

    for (i = 0; i < RIVALS; i++) {
        if (lens_flare_rivals[i] == task) {
            lens_flare_rivals[i] = NULL;
        }
    }
}

/* whether task's work lies at least as near the current camera as each
 * listed rival's work */
static Bool lens_flare_is_nearest_rival(struct task *task)
{
    int i;
    struct task *rival;
    Float distance;

    distance = njDistanceP2P(&camera_views[camera_current_index]->pos,
                             &task->work->pos);
    for (i = 0; i < RIVALS; i++) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((rival = lens_flare_rivals[i]) != NULL &&
            distance > njDistanceP2P(&camera_views[camera_current_index]->pos,
                                     &rival->work->pos)) {
            return FALSE;
        }
    }
    return TRUE;
}

void lens_flare_init(struct task *task)
{
    /* @hack redundant: a block no path reaches, kept for its two strings,
     * which SHC still places in this source's constants */
    if (0) {
        (void)"<- ALT";
        (void)"<- ALL";
    }
    task->step = lens_flare_step;
    /* @hack spelling: destructor set before draw, not in the members'
     * order */
    task->destructor = lens_flare_destroy;
    task->draw = lens_flare_task_draw;
    if (task->work->ang.x & LENS_FLARE_RIVAL) {
        lens_flare_add_rival(task);
    }
}

void lens_flare_draw(NJS_POINT3 *pos)
{
    NJS_POINT2 screen;
    NJS_VECTOR view;
    NJS_VECTOR dir;
    Float facing;
    Float strength;
    Float power;
    Float scale;
    Float fade;
    struct lens_flare_ghost *ghost;

#ifdef NONMATCHING
    /* An empty texture bank has no flare cells or ghosts to draw. */
    if (lens_flare_texlist.nbTexture == 0) {
        return;
    }
#endif
    dir.x = pos->x - camera_views[camera_current_index]->pos.x;
    dir.y = pos->y - camera_views[camera_current_index]->pos.y;
    dir.z = pos->z - camera_views[camera_current_index]->pos.z;
    njUnitVector(&dir);
    /* @hack redundant: a test of camera_current_index that never runs */
    if (0) {
        if (camera_current_index) {
        }
    }
    /* @hack spelling: a block of its own for forward, where forward declared
     * with the other locals would do */
    {
        NJS_VECTOR forward;

        forward.x = 0.0F;
        forward.y = 0.0F;
        forward.z = -1.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, camera_views[camera_current_index]->ang.y);
        njRotateX(NULL, camera_views[camera_current_index]->ang.x);
        njCalcVector(NULL, &forward, &view);
        njPopMatrix(1);
    }
    facing = njInnerProduct(&view, &dir);
    if (facing > 0.0F) {
        NJS_POINT2 to_center;

        njSetTexture(&lens_flare_texlist);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njFogDisable();
        njProjectScreen(NULL, pos, &screen);
        if (facing > GLARE_FACING) {
            scale = njSqrt((facing - GLARE_FROM) * GLARE_RATE);
            constant_material_set(scale * GLARE_ALPHA, 1.0F, 1.0F, 1.0F);
            lens_flare_sprite.p.x = screen.x;
            lens_flare_sprite.p.y = screen.y;
            lens_flare_sprite.sx = scale * GLARE_SIZE;
            lens_flare_sprite.sy = scale * GLARE_SIZE;
            njDrawSprite2D(&lens_flare_sprite, GLARE_CELL, GLARE_PRIORITY,
                           SPRITE_ATTR);
        }
        if (facing > STRENGTH_FROM) {
            power = (facing - STRENGTH_FROM) * STRENGTH_RATE;
        } else {
            power = 0.0F;
        }
        strength = njSqrt(njSqrt(power));
        to_center.x = (SCREEN_WIDTH / 2) - screen.x;
        to_center.y = (SCREEN_HEIGHT / 2) - screen.y;
        constant_material_set(1.0F, 1.0F, 1.0F, 1.0F);
        lens_flare_sprite.p.x = screen.x;
        lens_flare_sprite.p.y = screen.y;
        lens_flare_sprite.sx = 1.0F;
        lens_flare_sprite.sy = 1.0F;
        njDrawSprite2D(&lens_flare_sprite, GLOW_CELL, GLOW_PRIORITY,
                       SPRITE_ATTR);
        fade = 1.0F - strength;
        for (ghost = lens_flare_ghosts; ghost->cell != GHOST_END; ghost++) {
            scale = ghost->scale * (1.0F - (fade * ghost->shrink));
            if (scale < 0.0F) {
                scale = 0.0F;
            }
            constant_material_set(strength * ghost->alpha, 1.0F, 1.0F, 1.0F);
            lens_flare_sprite.p.x = screen.x + (to_center.x * ghost->offset);
            lens_flare_sprite.p.y = screen.y + (to_center.y * ghost->offset);
            lens_flare_sprite.sx = scale;
            lens_flare_sprite.sy = scale;
            njDrawSprite2D(&lens_flare_sprite, ghost->cell,
                           ghost->priority + GHOST_PRIORITY, SPRITE_ATTR);
        }
    }
    njFogEnable();
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    lens_flare_sprite.p.x = 0.0F;
    lens_flare_sprite.p.y = 0.0F;
    lens_flare_sprite.sx = 1.0F;
    lens_flare_sprite.sy = 1.0F;
}

static void lens_flare_step(struct task *task)
{
    /* @hack redundant: task's address taken, which nothing uses */
    (void)&task;
}

static void lens_flare_task_draw(struct task *task)
{
    if (!(task->work->ang.x & LENS_FLARE_RIVAL) ||
        lens_flare_is_nearest_rival(task)) {
        lens_flare_draw(&task->work->pos);
    }
}

static void lens_flare_destroy(struct task *task)
{
    lens_flare_remove_rival(task);
}

void lens_flare_create(NJS_POINT3 *pos)
{
    struct task *task =
        task_create(TASK_WORK, LEVEL, lens_flare_init, "EffectLensFlare");

    if (task != NULL) {
        /* @hack spelling: work, a local for task's work, where task->work
         * read in place would do */
        struct task_work *work = task->work;

        work->pos = *pos;
    }
}

void lens_flare_draw_facing(NJS_POINT3 *pos, NJS_VECTOR *normal)
{
    NJS_POINT2 screen;
    NJS_VECTOR view;
    NJS_VECTOR dir;
    Float facing;
    Float strength;
    Float power;
    Float scale;
    Float fade;
    struct lens_flare_ghost *ghost;
    struct task_work *camera = camera_task_work;

#ifdef NONMATCHING
    /* An empty texture bank has no flare cells or ghosts to draw. */
    if (lens_flare_texlist.nbTexture == 0) {
        return;
    }
#endif
    dir.x = pos->x - camera->pos.x;
    dir.y = pos->y - camera->pos.y;
    dir.z = pos->z - camera->pos.z;
    njUnitVector(&dir);
    /* @hack spelling: a block of its own for turn and forward, where the two
     * declared with the other locals would do */
    {
        NJS_MATRIX turn;
        NJS_VECTOR forward;

        forward.x = 0.0F;
        forward.y = 0.0F;
        forward.z = -1.0F;
        njUnitMatrix(&turn);
        njRotateY(&turn, camera->ang.y);
        njRotateX(&turn, camera->ang.x);
        njCalcVector(&turn, &forward, &view);
    }
    facing = -(njInnerProduct(&view, &dir) * njInnerProduct(normal, &dir));
    if (facing > 0.0F) {
        NJS_POINT2 to_center;

        njSetTexture(&lens_flare_texlist);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njProjectScreen(NULL, pos, &screen);
        if (facing > GLARE_FACING) {
            scale = njSqrt((facing - GLARE_FROM) * GLARE_RATE);
            constant_material_set(scale * GLARE_ALPHA, 1.0F, 1.0F, 1.0F);
            lens_flare_sprite.p.x = screen.x;
            lens_flare_sprite.p.y = screen.y;
            lens_flare_sprite.sx = scale * GLARE_SIZE;
            lens_flare_sprite.sy = scale * GLARE_SIZE;
            njDrawSprite2D(&lens_flare_sprite, GLARE_CELL, GLARE_PRIORITY,
                           SPRITE_ATTR);
        }
        if (facing > STRENGTH_FROM) {
            power = (facing - STRENGTH_FROM) * STRENGTH_RATE;
        } else {
            power = 0.0F;
        }
        strength = njSqrt(njSqrt(power));
        to_center.x = (SCREEN_WIDTH / 2) - screen.x;
        to_center.y = (SCREEN_HEIGHT / 2) - screen.y;
        constant_material_set(strength, 1.0F, 1.0F, 1.0F);
        lens_flare_sprite.p.x = screen.x;
        lens_flare_sprite.p.y = screen.y;
        /* @hack spelling: one assignment to sy and sx, where an assignment to
         * each would do */
        lens_flare_sprite.sy = lens_flare_sprite.sx = 1.0F;
        fade = 1.0F - strength;
        for (ghost = lens_flare_ghosts; ghost->cell != GHOST_END; ghost++) {
            scale = ghost->scale * (1.0F - (fade * ghost->shrink));
            if (scale < 0.0F) {
                scale = 0.0F;
            }
            constant_material_set(strength * ghost->alpha, 1.0F, 1.0F, 1.0F);
            lens_flare_sprite.p.x = screen.x + (to_center.x * ghost->offset);
            lens_flare_sprite.p.y = screen.y + (to_center.y * ghost->offset);
            lens_flare_sprite.sx = scale;
            lens_flare_sprite.sy = scale;
            njDrawSprite2D(&lens_flare_sprite, ghost->cell, ghost->priority,
                           SPRITE_ATTR);
        }
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    /* @hack spelling: one assignment to p.y and p.x, where an assignment to
     * each would do */
    lens_flare_sprite.p.y = lens_flare_sprite.p.x = 0.0F;
    /* @hack spelling: one assignment to sy and sx, where an assignment to
     * each would do */
    lens_flare_sprite.sy = lens_flare_sprite.sx = 1.0F;
}
