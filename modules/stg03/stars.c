/* the stars, the task HosiExec: a cloud of glowing points about a player.
 * the points hang still in the world while the cloud follows the player */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "camera/camera.h"
#include "player/player.h"
#include "stg03/stars.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

/* the cloud's groups. each group's points waver together, and draw as
 * sprites of one size */
#define GROUPS 10

/* the slot of the player the stars follow, in the work's tick */
#define PLAYER(work) ((work)->tick)
/* task_create's level for the stars */
#define STARS_LEVEL 1

/* a step that moves the cloud's center farther than JUMP_RANGE carries the
 * points with it. they keep their places about the center */
#define JUMP_RANGE 100.0F

/* a group's waver is a vector along y, turned by the group's turn. its length
 * is the size of the group's wave, a sum of WAVES sines of the group's step. a
 * group's step runs GROUP_DELAY ahead of the step of the group before it. each
 * step, the waver moves the group's points by its x and z. it lifts them by
 * WAVER_SCALE of its y's size */
#define WAVES 3
#define GROUP_DELAY 160
#define WAVER_SCALE 0.3F

/* the shell stars_place puts a point on reaches SHELL_DEPTH under the
 * sphere's surface */
#define SHELL_DEPTH 30.0F

/* a group's pulse runs PULSE_DELAY steps ahead of the pulse of the group
 * before it */
#define PULSE_DELAY 60

/* a term of a group's wave: amp times the sine of freq times the step.
 * freq is in Angle units a step */
struct stars_wave {
    Sint32 freq;
    Float amp;
};

/* the stars' record, at task->motion */
struct stars {
    /* the points, as offsets from the cloud's center. each group's lie
     * together */
    NJS_POINT3 *points;
    Sint32 count; /* the points of each group */
    struct task **owner; /* the caller's pointer to the task, or null */
};

#define STARS(task) ((struct stars *)(task)->motion.p)

/* The star sprite used by both players. */

/* clang-format off */
#include "stg03/stars/text.inc"

static NJS_TEXNAME stars_textures[1] = {
#include "stg03/stars/stars_textures.inc"
};

NJS_TEXLIST stars_texlist = {stars_textures, 1};

/* clang-format on */

static Float stars_radius = 350.0F;
/* the first group's sprite size, and what each group adds to it */
static Float stars_size = 0.1F;
static Float stars_size_step = 0.125F;
static Float dat_8c5fab60 = 13.0F;
static struct stars_wave stars_waves[WAVES] = {
    {368, 0.15F},
    {176, 0.1F},
    {21, 0.25F},
};
/* a group's turn is stars_turn_rate times its step about z, and twice that
 * about x */
static Sint32 stars_turn_rate = 112;
/* how far the points rise a step, and what each group adds to it */
static Float stars_rise = 0.05F;
static Float stars_rise_step = 0.03F;
/* the sprites' color, as 0xAARRGGBB */
static Uint32 stars_color = 0xd0ff5000;
/* the sprites' width and height swell and shrink against each other, by up
 * to stars_pulse_size of their size. the pulse's angle gains
 * stars_pulse_rate a step */
static Float stars_pulse_size = 0.1F;
static Sint32 stars_pulse_rate = 3500;

static void stars_step(struct task *task);
static void stars_draw(struct task *task);
static void stars_destroy(struct task *task);

/* set *pos halfway between the player in slot player and
 * camera_views[player]. where that camera is null, set it to the player's
 * pos */
static void stars_center(NJS_POINT3 *pos, int player)
{
    if (camera_views[player] != NULL) {
        pos->x =
            (player_work[player]->pos.x + camera_views[player]->pos.x) * 0.5F;
        pos->y =
            (player_work[player]->pos.y + camera_views[player]->pos.y) * 0.5F;
        pos->z =
            (player_work[player]->pos.z + camera_views[player]->pos.z) * 0.5F;
    } else {
        *pos = player_work[player]->pos;
    }
}

/* the sum at step of the count terms from waves. only step's low 16 bits
 * matter */
static Float stars_wave_sum(struct stars_wave *waves, int count, int step)
{
    Float sum = 0.0F;

    step = (Uint16)step;
    while (count--) {
        sum += njSin(waves->freq * step) * waves->amp;
        waves++;
    }
    return sum;
}

/* place *point at random within radius of the origin. with on_shell set,
 * place it on the shell SHELL_DEPTH deep under that radius. every call
 * passes an on_shell of 0 */
static void stars_place(NJS_POINT3 *point, Uint8 on_shell, Float radius)
{
    if (on_shell != 0) {
        point->x = 0.0F;
        point->y = radius - (rand() * (SHELL_DEPTH * RAND_SCALE));
        point->z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateZ(NULL, rand() * 2.0F);
        njRotateX(NULL, rand() * 2.0F);
        njCalcPoint(NULL, point, point);
        njPopMatrix(1);
    } else {
        do {
            point->x = ((rand() * RAND_SCALE) - 0.5F) * radius * 2.0F;
            point->y = ((rand() * RAND_SCALE) - 0.5F) * radius * 2.0F;
            point->z = ((rand() * RAND_SCALE) - 0.5F) * radius * 2.0F;
        } while (njScalor(point) > radius);
    }
}

struct task *stars_create(int player, int count, struct task **owner)
{
    struct task *task;
    struct task_work *work;
    NJS_POINT3 *point;
    int i;

    if (player_work[player] == NULL) {
        return NULL;
    }
    task = task_create(TASK_WORK, STARS_LEVEL, stars_step, "HosiExec");
    if (task != NULL) {
        work = task->work;
        /* @hack spelling: task->work read again for the center, where work
         * would do */
        stars_center(&task->work->pos, player);
        PLAYER(work) = player;
        task->motion.p = syCalloc(1, sizeof(struct stars));
        if (task->motion.p == NULL) {
            task_destroy(task);
            return NULL;
        }
        count = count / GROUPS;
        STARS(task)->points = syCalloc(count * GROUPS, sizeof(NJS_POINT3));
        if (STARS(task)->points == NULL) {
            syFree(STARS(task)->points);
            syFree(task->motion.p);
            task->motion.p = NULL;
            task_destroy(task);
            return NULL;
        }
        task->draw = stars_draw;
        task->step = stars_step;
        task->destructor = stars_destroy;
        STARS(task)->count = count;
        point = STARS(task)->points;
        for (i = 0; i < count * GROUPS; i++) {
            stars_place(point, 0, stars_radius);
            /* @hack spelling: the step written as an addition, where point++
             * would do */
            point = point + 1;
        }
        STARS(task)->owner = owner;
        if (owner != NULL) {
            *owner = task;
        }
        return task;
    }
    return NULL;
}

static void stars_step(struct task *task)
{
    struct task_work *work = task->work;
    struct stars *stars;
    /* @hack redundant: point declared register, where an ordinary local would
     * do */
    register NJS_POINT3 *point;
    NJS_POINT3 center;
    NJS_VECTOR moved;
    NJS_VECTOR waver;
    Float off_x;
    Float off_y;
    Float off_z;
    Float x;
    Float y;
    Float z;
    Float reach;
    Sint32 step;
    /* @hack redundant: turn declared register, where an ordinary local would
     * do */
    register Sint32 turn;
    /* @hack redundant: count declared register, where an ordinary local would
     * do */
    register int count;
    int i;
    int j;

    if (player_work[PLAYER(work)] == NULL) {
        task_destroy(task);
        return;
    }
    stars_center(&center, PLAYER(work));
    if (game_state.time_stopped == 0) {
        moved.x = -center.x + work->pos.x;
        moved.y = -center.y + work->pos.y + stars_rise;
    } else {
        moved.x = -center.x + work->pos.x;
        moved.y = -center.y + work->pos.y;
    }
    moved.z = -center.z + work->pos.z;
    work->pos = center;
    /* @hack spelling: task->work read again for the mode, where work would
     * do */
    if (task->work->mode != 0) {
        return;
    }
    if (njScalor(&moved) > JUMP_RANGE) {
        return;
    }
    stars = task->motion.p;
    count = stars->count;
    point = stars->points;
    for (i = 0; i < GROUPS; i++) {
        step = (i * GROUP_DELAY) + game_state.steps;
        turn = stars_turn_rate * step;
        waver.x = 0.0F;
        waver.y = _builtin_fabsf(stars_wave_sum(stars_waves, WAVES, step));
        waver.z = 0.0F;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateX(NULL, turn * 2);
        njRotateZ(NULL, turn);
        njCalcVector(NULL, &waver, &waver);
        njPopMatrix(1);
        if (game_state.time_stopped != 0) {
            off_x = moved.x;
            off_y = moved.y;
            off_z = moved.z;
        } else {
            off_x = waver.x + moved.x;
            off_y = _builtin_fabsf(waver.y * WAVER_SCALE) + moved.y +
                    (i * stars_rise_step);
            off_z = waver.z + moved.z;
        }
        for (j = 0; j < count; j++, point++) {
            /* @hack spelling: y, z and x in that order, and z's sum split in
             * two, where x, y and z each in one statement would do */
            y = point->y + off_y;
            z = point->z;
            z += off_z;
            x = point->x + off_x;
            reach = (x * x) + (y * y) + (z * z);
            if (reach > stars_radius * stars_radius) {
                point->x = -point->x + off_x;
                point->y = -point->y + off_y;
                point->z = -point->z + off_z;
            } else {
                point->x = x;
                point->y = y;
                point->z = z;
            }
        }
    }
}

static void stars_draw(struct task *task)
{
    struct task_work *work;
    struct task_work *same;
    Float pulse;
    int i;

    /* @hack spelling: the work under two names, where one would do */
    work = same = task->work;
    if (!same->mode && camera_current_index == PLAYER(work)) {
        njFogDisable();
        njPushMatrixEx();
        /* @hack spelling: task->work read again for the pos, where work would
         * do */
        njTranslateV(NULL, &task->work->pos);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
        njSetTexture(&stars_texlist);
        for (i = 0; i < GROUPS; i++) {
            /* @hack spelling: game_state.steps read through its second name,
             * game_state_steps, and cast to Sint32, which changes no value */
            pulse = njSin(stars_pulse_rate *
                          ((i * PULSE_DELAY) + (Sint32)game_state_steps)) *
                    stars_pulse_size;
            njPtclSpriteStart(0, stars_color, 1);
            njPtclDrawSprite(
                &STARS(task)->points[i * STARS(task)->count],
                STARS(task)->count,
                (1.0F - pulse) * (stars_size + (i * stars_size_step)),
                (stars_size + (i * stars_size_step)) * (pulse + 1.0F));
            njPtclSpriteEnd();
        }
        njPopMatrix(1);
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njFogEnable();
    }
}

static void stars_destroy(struct task *task)
{
    if (STARS(task)->owner != NULL && *STARS(task)->owner == task) {
        *STARS(task)->owner = NULL;
    }
    syFree(STARS(task)->points);
    syFree(task->motion.p);
    task->motion.p = NULL;
}
