#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "boss_last2/arena.h"
#include "boss_last2/stars.h"
#include "camera/camera.h"
#include "player/player.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the stars' record, in their task's motion */
struct starfield {
    NJS_POINT3 *points; /* the stars, about the field's center */
    Sint32 count;
    struct task **owner; /* finalhazard_starfield_create's owner */
};

#define STARFIELD(task) ((struct starfield *)(task)->motion.p)
/* the player whose camera the stars follow, in their work's tick */
#define VIEWER(work) ((work)->tick)
#define STARS_LEVEL 1
#define SHELL_THICKNESS 30.0F
/* the stars stay where they were through a step that moves their center
 * farther than CAMERA_CUT_DISTANCE. the drift counts toward the move */
#define CAMERA_CUT_DISTANCE 100.0F
/* the texture of finalhazard_star_texlist the sprites take, and
 * njPtclSpriteStart's flag for the translucent list */
#define SPRITE_TEXTURE 0
#define TRANSLUCENT 1
/* the draw sets Ninja's mipmap D adjust to STAR_MIPMAP quarters, then back
 * to MIPMAP_1_00, an adjust of 1.00 */
#define STAR_MIPMAP 15
#define MIPMAP_1_00 4
/* a random value from -extent up to extent */
#define SPREAD(extent) (((rand() * RAND_SCALE) - 0.5F) * (extent) * 2.0F)

static const char task_name[] = "HosiExec";
#include "boss_last2/stars/text.inc"

/* the stars' one texture, which the sky's shared archive gives */
static NJS_TEXNAME star_name[1] = {
#include "boss_last2/stars/star_name.inc"
};
NJS_TEXLIST finalhazard_star_texlist = {star_name, 1};
/* the stars lie within radius of their center, which lies radius ahead of
 * the camera. a star that would pass radius moves through the center to the
 * far side */
static Float radius = 500.0F;
static Float star_size = 0.5F;
static Float unused_5270[2] = {0.125F, 13.0F};
/* each step moves the stars speed times as far as their center moved, the
 * other way. while time runs, it also moves them speed times (drift_x,
 * drift_y, drift_z) */
static Float drift_x = 2.15F;
static Float drift_y = 2.0F;
static Float drift_z = 0.05F;
static Float unused_5284 = 0.1F;
static Float speed = 2.0F;
static Uint32 star_color = 0xffffffd0;
static Float unused_5290 = 0.1F;
static Sint32 unused_5294 = 3500;

static void starfield_step(struct task *task);
static void starfield_draw(struct task *task);
static void starfield_destroy(struct task *task);

/* the center of player's stars: radius ahead of the camera */
static void field_center(NJS_POINT3 *out, int player)
{
    NJS_POINT3 ahead;
    ahead.x = ahead.y = 0.0F;
    ahead.z = -radius;
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateY(NULL, camera_views[player]->ang.y);
    njRotateX(NULL, camera_views[player]->ang.x);
    njCalcPoint(NULL, &ahead, &ahead);
    njPopMatrixEx();
    out->x = camera_views[player]->pos.x + ahead.x;
    out->y = camera_views[player]->pos.y + ahead.y;
    out->z = camera_views[player]->pos.z + ahead.z;
}

/* a random point within extent of the origin. with shell, a point less than
 * SHELL_THICKNESS inside extent instead, which no call asks for */
/* @hack type: shell a Uint8, where a Bool would do */
static void random_star(NJS_POINT3 *point, Uint8 shell, Float extent)
{
    if (shell) {
        point->x = 0.0F;
        point->y = extent - (rand() * (SHELL_THICKNESS * RAND_SCALE));
        point->z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, RANDOM_TURN());
        njRotateX(NULL, RANDOM_TURN());
        njCalcPoint(NULL, point, point);
        njPopMatrix(1);
    } else {
        do {
            point->x = SPREAD(extent);
            point->y = SPREAD(extent);
            point->z = SPREAD(extent);
        } while (njScalor(point) > extent);
    }
}

struct task *finalhazard_starfield_create(int player, int count,
                                          struct task **owner)
{
    struct task *task;
    struct task_work *work;
    NJS_POINT3 *point;
    int i;

    /* @hack redundant: an empty test of player's work */
    if (player_work[player] == NULL) {
    }
    task = task_create(TASK_WORK, STARS_LEVEL, starfield_step, task_name);
    if (task == NULL) {
        return NULL;
    }

    work = task->work;
    /* @hack redundant: the address of work, which nothing reads */
    (void)&work;
    field_center(&work->pos, player);
    VIEWER(work) = player;
    task->motion.p = syCalloc(1, sizeof(struct starfield));
    if (task->motion.p == NULL) {
        task_destroy(task);
        /* @hack spelling: a goto to the return of null after the else, where
         * return NULL would do */
        goto fail;
    } else {
        STARFIELD(task)->points = syCalloc(count, sizeof(NJS_POINT3));
        if (STARFIELD(task)->points == NULL) {
            syFree(task->motion.p);
            task->motion.p = NULL;
            task_destroy(task);
            return NULL;
        }
        task->draw = starfield_draw;
        task->step = starfield_step;
        task->destructor = starfield_destroy;
        STARFIELD(task)->count = count;
        /* @hack redundant: an if (0) around two stores */
        if (0) {
            task->motion.p = NULL;
            task->step = NULL;
        }
        /* @hack spelling: a while loop, with i set before point, where a for
         * loop would do */
        i = 0;
        point = STARFIELD(task)->points;
        while (i < count) {
            random_star(point, FALSE, radius);
            point++;
            i++;
        }
        STARFIELD(task)->owner = owner;
        if (owner != NULL) {
            *owner = task;
        }
        return task;
    }
fail:
    return NULL;
}

static void starfield_step(struct task *task)
{
    struct task_work *work = task->work;
    struct starfield *field;
    NJS_POINT3 *point;
    NJS_POINT3 center;
    NJS_VECTOR delta;
    Float x;
    Float y;
    Float z;
    /* @hack redundant: count declared register, where a plain int would do */
    register int count;
    int i;
    /* @hack redundant: an empty test of the viewer's work */
    if (player_work[VIEWER(work)] == NULL) {
    }
    field_center(&center, VIEWER(work));
    if (game_state.time_stopped == 0) {
        delta.x = -center.x + work->pos.x + drift_x;
        delta.y = -center.y + work->pos.y + drift_y;
        delta.z = -center.z + work->pos.z + drift_z;
    } else {
        delta.x = -center.x + work->pos.x;
        delta.y = -center.y + work->pos.y;
        delta.z = -center.z + work->pos.z;
    }
    work->pos = center;
    if (task->work->mode != 0) {
        return;
    }
    if (njScalor(&delta) > CAMERA_CUT_DISTANCE) {
        return;
    }
    field = task->motion.p;
    count = field->count;
    point = field->points;
    x = delta.x * speed;
    y = delta.y * speed;
    z = delta.z * speed;
    /* @hack spelling: i = i + 1, where i++ would do */
    for (i = 0; i < count; i = i + 1, point++) {
        Float next_x;
        Float next_y;
        Float next_z;
        Float distance_sq;
        next_x = point->x + x;
        next_y = point->y + y;
        /* @hack spelling: next_z set, then added to, where next_z =
         * point->z + z would do */
        next_z = point->z;
        next_z += z;
        distance_sq = (next_x * next_x) + (next_y * next_y) + (next_z * next_z);
        if (distance_sq > radius * radius) {
            point->x = -point->x + x;
            point->y = -point->y + y;
            point->z = -point->z + z;
        } else {
            point->x = next_x;
            point->y = next_y;
            point->z = next_z;
        }
    }
}

static void starfield_draw(struct task *task)
{
    struct task_work *work;
    struct task_work *view;
#ifdef NONMATCHING
    /* this side draws no stars before their texture loads, which leaves
     * star_name's texaddr 0 */
    if (star_name[0].texaddr == 0) {
        return;
    }
#endif
    /* @hack spelling: view, a second name for work, where work would do */
    work = view = task->work;
    if (!view->mode && camera_current_index == VIEWER(work)) {
        njFogDisable();
        njMipmapAdjust(STAR_MIPMAP);
        njPushMatrixEx();
        njTranslateEx(&task->work->pos);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(&finalhazard_star_texlist);
        njPtclSpriteStart(SPRITE_TEXTURE, star_color, TRANSLUCENT);
        njPtclDrawSprite(STARFIELD(task)->points, STARFIELD(task)->count,
                         star_size, star_size);
        njPtclSpriteEnd();
        njPopMatrixEx();
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njMipmapAdjust(MIPMAP_1_00);
        njFogEnable();
    }
}

static void starfield_destroy(struct task *task)
{
    /* @bug memory: reads owner through a null motion, at 0x8c50ca10, where
     * motion is null, which the test at 0x8c50ca26 expects */
    if (STARFIELD(task)->owner != NULL && *STARFIELD(task)->owner == task) {
        *STARFIELD(task)->owner = NULL;
    }
    if (task->motion.p != NULL) {
        if (STARFIELD(task)->points != NULL) {
            syFree(STARFIELD(task)->points);
            STARFIELD(task)->points = NULL;
        }
        syFree(task->motion.p);
        task->motion.p = NULL;
    }
}
