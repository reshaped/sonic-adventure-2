/* bubbles: the air a player breathes out under water, which rises, grows and
 * ends at the surface */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdlib.h>

#include "collision/world.h"
#include "effect/bubbles.h"
#include "effect/texlist.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"

#define BUBBLES_LEVEL 1 /* the bubbles' task level */

/* a bubble starts only below the surface, and less than SURFACE_REACH below
 * it */
#define SURFACE_REACH 500.0F

/* bubble_params holds PARAM_SETS sets for each character. each step picks
 * one. a drowning player takes SET_BURST_BREATHING while it breathes out,
 * and SET_BURST while it doesn't. any other player takes SET_CALM */
#define PARAM_SETS 3
#define SET_CALM 0
#define SET_BURST_BREATHING 1
#define SET_BURST 2

/* a breath out starts at a step that finds the player's anim's frame at
 * BREATH_FRAME or less. that step sets the timer to BREATH_STEPS. the breath
 * would end at the step that finds the timer at 0. no breath ends, though,
 * since each step counts the timer down and then up again */
#define BREATH_STEPS 40
#define BREATH_FRAME 1.0F

/* a new bubble's speed adds SPEED_KEEP of the player's. its size starts at
 * GROW_START of its full size */
#define SPEED_KEEP 0.9F
#define GROW_START 0.1F
/* a bubble's angle starts at random, from 0 up to START_TURNS turns */
#define START_TURNS 3
/* a bubble that overlaps the head's sphere moves out until PUSH_CLEAR lies
 * between their edges. one whose squared distance from the center is
 * PUSH_MIN_SQUARED or less stays */
#define PUSH_CLEAR 0.1F
#define PUSH_MIN_SQUARED 0.01F

/* the task's work. scl.z holds the player's slot, as a Float. submode is 1
 * while the player breathes out, and tick holds the set the step picks */
#define BUBBLES_PLAYER(work) ((Sint8)(work)->scl.z)
#define BREATHING(work) ((work)->submode)
#define PARAM_SET(work) ((work)->tick)
/* the task's motion holds its list of bubbles, and its force its
 * character's sets in bubble_params */
#define BUBBLE_LIST(task) ((struct bubble *)(task)->motion.p)
#define PARAMS(task) ((struct bubble_params *)(task)->force.p)
#define PARAM(task, work) (&PARAMS(task)[PARAM_SET(work)])

/* a bubble, which bubble_alloc takes from the Shinobi heap */
struct bubble {
    Float surface; /* the height of the water's surface at its start */
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    Sint16 ang;
    Sint16 ang_spd;
    Float aspect; /* the sprite's height over its width */
    Float size;
    Float max_size;
    /* the list of the task's bubbles, the newest first */
    struct bubble *prev;
    struct bubble *next;
};

/* how a character's bubbles start and move */
struct bubble_params {
    Sint32 character; /* as player_character gives it */
    /* nothing reads these three */
    Sint32 unk_04;
    Float unk_08;
    Float unk_0c;
    /* the head's point lies reach along unk_2b4 from unk_218 of the player's
     * record. the head's sphere passes through unk_224. its center lies past
     * the head's point from unk_224, as far again */
    Float reach;
    Float speed; /* the speed a bubble leaves unk_224 at, along unk_2c0 */
    Float size_max;
    Float size_min;
    Float squash; /* the most the sprite's aspect falls short of 1 */
    Float drag; /* what each step multiplies a bubble's speed by */
    Float lift; /* what each step adds to its speed's y */
    /* the most a new bubble's speed strays along each axis, either way */
    Float spread;
    /* the chance of a new bubble each step, from 0 to 1 */
    Float rate;
    /* a bubble spins at a random rate, in degrees a step, within spin / 2
     * either way */
    Float spin;
    Float growth; /* what each step adds to its size */
    /* the chance of a wobble each step, from 0 to 1. a wobble adds to the
     * bubble's speed along x and z. each gains at random within wobble / 2
     * either way */
    Float wobble_rate;
    Float wobble;
};

static void bubbles_destroy(struct task *task);
static void bubbles_step(struct task *task);
static void bubbles_draw(struct task *task);

/* PARAM_SETS sets for each of Tails, Sonic, Knuckles, Amy and Rouge */
/* clang-format off */
static struct bubble_params bubble_params[] = {
    {PLAYER_TAILS,    0, 2.0F, 0.0F, 2.0F, 0.1F,  0.4F, 0.2F,  0.3F, 0.961F, 0.015F, 0.05F, 0.1F, 8.0F, 0.08F, 0.3F, 0.12F},
    {PLAYER_TAILS,    0, 2.0F, 0.0F, 2.0F, 0.15F, 1.0F, 0.3F,  0.3F, 0.961F, 0.015F, 0.15F, 0.9F, 8.0F, 0.08F, 0.3F, 0.12F},
    {PLAYER_TAILS,    0, 2.0F, 0.0F, 2.0F, 0.15F, 0.4F, 0.2F,  0.3F, 0.961F, 0.015F, 0.15F, 0.4F, 8.0F, 0.08F, 0.3F, 0.12F},
    {PLAYER_SONIC,    0, 2.0F, 0.0F, 2.0F, 0.09F, 0.4F, 0.2F,  0.3F, 0.961F, 0.015F, 0.03F, 0.1F, 8.0F, 0.1F,  0.3F, 0.12F},
    {PLAYER_SONIC,    0, 2.0F, 0.0F, 2.0F, 0.15F, 1.0F, 0.3F,  0.3F, 0.961F, 0.015F, 0.15F, 0.9F, 8.0F, 0.1F,  0.3F, 0.12F},
    {PLAYER_SONIC,    0, 2.0F, 0.0F, 2.0F, 0.15F, 0.4F, 0.2F,  0.3F, 0.961F, 0.015F, 0.15F, 0.4F, 8.0F, 0.1F,  0.3F, 0.12F},
    {PLAYER_KNUCKLES, 0, 2.0F, 0.0F, 2.0F, 0.09F, 0.4F, 0.2F,  0.3F, 0.961F, 0.015F, 0.03F, 0.1F, 8.0F, 0.1F,  0.3F, 0.12F},
    {PLAYER_KNUCKLES, 0, 2.0F, 0.0F, 2.0F, 0.15F, 1.0F, 0.3F,  0.3F, 0.961F, 0.015F, 0.15F, 0.9F, 8.0F, 0.1F,  0.3F, 0.12F},
    {PLAYER_KNUCKLES, 0, 2.0F, 0.0F, 2.0F, 0.15F, 0.4F, 0.2F,  0.3F, 0.961F, 0.015F, 0.15F, 0.4F, 8.0F, 0.1F,  0.3F, 0.12F},
    /* @bug logic: no player takes these sets, which start at 0x264 of the
     * object's data. they are keyed by Amy's kind, PLAYER_KIND_AMY, but
     * player_character gives a record's character, and Amy's is
     * PLAYER_SONIC (MOV.B at 0x8c04ad9e) */
    {PLAYER_KIND_AMY, 0, 2.0F, 0.0F, 2.0F, 0.05F, 0.3F, 0.15F, 0.3F, 0.961F, 0.015F, 0.05F, 0.1F, 8.0F, 0.1F,  0.3F, 0.12F},
    {PLAYER_KIND_AMY, 0, 2.0F, 0.0F, 2.0F, 0.1F,  1.0F, 0.3F,  0.3F, 0.961F, 0.015F, 0.15F, 0.9F, 8.0F, 0.1F,  0.3F, 0.12F},
    {PLAYER_KIND_AMY, 0, 2.0F, 0.0F, 2.0F, 0.1F,  0.3F, 0.1F,  0.3F, 0.961F, 0.015F, 0.15F, 0.3F, 8.0F, 0.1F,  0.3F, 0.12F},
    {PLAYER_ROUGE,    0, 2.0F, 0.0F, 2.0F, 0.09F, 0.4F, 0.2F,  0.3F, 0.961F, 0.015F, 0.03F, 0.1F, 8.0F, 0.1F,  0.3F, 0.12F},
    {PLAYER_ROUGE,    0, 2.0F, 0.0F, 2.0F, 0.15F, 1.0F, 0.3F,  0.3F, 0.961F, 0.015F, 0.15F, 0.9F, 8.0F, 0.1F,  0.3F, 0.12F},
    {PLAYER_ROUGE,    0, 2.0F, 0.0F, 2.0F, 0.15F, 0.4F, 0.2F,  0.3F, 0.961F, 0.015F, 0.15F, 0.4F, 8.0F, 0.1F,  0.3F, 0.12F},
};
/* clang-format on */
#define BUBBLE_PARAMS ARRAY_COUNT(bubble_params)

/* a bubble's sprite: the whole of effect_texlist's bubble, 2 by 2 */
static NJS_TEXANIM bubble_texanim = {
#include "effect/bubbles/bubble_texanim.inc"
};

/* a new bubble at the head of the task's list, or null where the heap can't
 * give one */
static struct bubble *bubble_alloc(struct task *task)
{
    struct bubble *bubble = syMalloc(sizeof(struct bubble));

    if (bubble != NULL) {
        bubble->prev = NULL;
        bubble->next = BUBBLE_LIST(task);
        if (BUBBLE_LIST(task) != NULL) {
            BUBBLE_LIST(task)->prev = bubble;
        }
        task->motion.p = bubble;
    }
    return bubble;
}

/* take bubble off the task's list and free it */
static void bubble_free(struct task *task, struct bubble *bubble)
{
    struct bubble *prev = bubble->prev;
    struct bubble *next = bubble->next;

    if (prev != NULL) {
        prev->next = next;
    } else {
        task->motion.p = next;
    }
    if (next != NULL) {
        next->prev = prev;
    }
    syFree(bubble);
}

/* set pos to unk_224 of the player's record, where a new bubble starts. set
 * spd to its speed along unk_2c0. set head, unless it is null, to the head's
 * point, as reach says */
static void bubble_origin(struct task *task, NJS_POINT3 *pos, NJS_VECTOR *spd,
                          NJS_POINT3 *head)
{
    struct task_work *work = task->work;
    struct player *record = player_records[BUBBLES_PLAYER(work)];
    /* @hack redundant: unused, a local that only sizes the frame */
    NJS_VECTOR unused;
    NJS_VECTOR dir;
    Float reach;
    Float *speed;

    *spd = record->unk_2c0;
    njUnitVector(spd);
    /* @hack spelling: speed stepped through spd's x, y and z, where spd->x,
     * spd->y and spd->z would do */
    speed = &spd->x;
    *speed++ *= PARAM(task, work)->speed;
    *speed++ *= PARAM(task, work)->speed;
    *speed *= PARAM(task, work)->speed;
    *pos = record->unk_224;
    if (head != NULL) {
        reach = PARAM(task, work)->reach;
        dir = record->unk_2b4;
        njUnitVector(&dir);
        head->x = record->unk_218.x + (dir.x * reach);
        head->y = record->unk_218.y + (dir.y * reach);
        head->z = record->unk_218.z + (dir.z * reach);
    }
}

/* add a bubble where the player is under WORLD_FLAT_THIN water. unk_224 must
 * lie less than SURFACE_REACH below the surface. it returns the bubble, or
 * null where it adds none */
static struct bubble *bubble_spawn(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack redundant: player, which nothing reads */
    struct task_work
        *player = /* NOLINT(clang-diagnostic-unused-but-set-variable) */
        player_work[BUBBLES_PLAYER(work)];
    struct player *record = player_records[BUBBLES_PLAYER(work)];
    struct bubble *bubble = NULL;
    NJS_POINT3 pos;
    NJS_VECTOR dir;
    NJS_VECTOR spd;
    Float size;

    if (!(record->ground.last_above_flags & WORLD_FLAT_THIN)) {
        return NULL;
    }
    bubble_origin(task, &pos, &dir, NULL);
    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if (pos.y < record->ground.above_y &&
        pos.y + SURFACE_REACH > record->ground.above_y &&
        (bubble = bubble_alloc(task)) != NULL) {
        /* @hack redundant: player read again, which nothing reads */
        player = player_work[BUBBLES_PLAYER(work)];
        spd = ((struct player_motion *)player_tasks[BUBBLES_PLAYER(work)]
                   ->motion.p)
                  ->spd;
        size = ((PARAM(task, work)->size_max - PARAM(task, work)->size_min) *
                rand() * RAND_SCALE) +
               PARAM(task, work)->size_min;
        dir.x += (((rand() * RAND_SCALE) - 0.5F) * PARAM(task, work)->spread *
                  2.0F) +
                 (spd.x * SPEED_KEEP);
        dir.y += (((rand() * RAND_SCALE) - 0.5F) * PARAM(task, work)->spread *
                  2.0F) +
                 (spd.y * SPEED_KEEP);
        dir.z += (((rand() * RAND_SCALE) - 0.5F) * PARAM(task, work)->spread *
                  2.0F) +
                 (spd.z * SPEED_KEEP);
        bubble->pos = pos;
        bubble->spd = dir;
        bubble->ang = rand() * (START_TURNS * FULL_TURN * RAND_SCALE);
        bubble->ang_spd = ((rand() * RAND_SCALE) - 0.5F) *
                          PARAM(task, work)->spin * ANGLE_PER_DEGREE;
        bubble->surface = record->ground.above_y;
        bubble->aspect =
            1.0F - (rand() * PARAM(task, work)->squash * RAND_SCALE);
        bubble->size = size * GROW_START;
        bubble->max_size = size;
    }
    return bubble;
}

/* the task's first step, which finds the sets of player 0's character */
static void bubbles_init(struct task *task)
{
    struct task_work *work = task->work;
    int i;

    task->draw = bubbles_draw;
    task->step = bubbles_step;
    task->destructor = bubbles_destroy;
    task->motion.p = NULL;
    BREATHING(work) = 0;
    work->timer = 0;
    PARAM_SET(work) = SET_CALM;
    task->force.p = NULL;
    for (i = 0; (Uint32)i < BUBBLE_PARAMS; i += PARAM_SETS) {
        /* @bug logic: takes player 0's character's sets, not those of the
         * player whose bubbles these are. MOV #0,R4 at 0x8c0a1996 passes slot
         * 0, where a creator passes its own slot, as MOV.B @R13,R4 at
         * 0x8c0653c8 does */
        if (player_character(0) == bubble_params[i].character) {
            task->force.p = &bubble_params[i];
            break;
        }
    }
}

static void bubbles_destroy(struct task *task)
{
    while (BUBBLE_LIST(task) != NULL) {
        bubble_free(task, BUBBLE_LIST(task));
    }
    task->motion.p = NULL;
    task->force.p = NULL;
}

static void bubbles_step(struct task *task)
{
    struct task_work *work = task->work;
    struct bubble *bubble;
    struct bubble *next;
    NJS_POINT3 pos;
    NJS_VECTOR spd;
    NJS_POINT3 head;
    NJS_POINT3 center;
    Float radius;
    Float cx;
    Float cy;
    Float cz;
    Float drag;
    Float lift;
    Float growth;
    Float wobble_rate;
    Float wobble;
    Float dx;
    Float dy;
    Float dz;
    Float reach;
    Float distance;
    Float scale;
    Float size;
    Float depth;
    Float y;
    Float max_size;
    Float limit;
    Float old_size;
    Float surface;
#ifdef NONMATCHING
    /* the bubble this side's wobble and turn write in place of one the step
     * has freed */
    static struct bubble stand_in;
#endif

    if (player_work[BUBBLES_PLAYER(work)] == NULL ||
        PARAM(task, work) == NULL) {
        task_kill(task);
        return;
    }
    if (player_work[BUBBLES_PLAYER(work)]->submode == PLAYER_REQUEST_DROWN) {
        if (BREATHING(work)) {
            PARAM_SET(work) = SET_BURST_BREATHING;
        } else {
            PARAM_SET(work) = SET_BURST;
        }
    } else {
        PARAM_SET(work) = SET_CALM;
    }
    if (BREATHING(work)) {
        if (PARAM_SET(work) != SET_CALM ||
            rand() * RAND_SCALE < PARAM(task, work)->rate) {
            bubble_spawn(task);
        }
        if (work->timer-- <= 0) {
            BREATHING(work) = 0;
        }
    } else {
        if (PARAM_SET(work) != SET_CALM &&
            rand() * RAND_SCALE < PARAM(task, work)->rate) {
            bubble_spawn(task);
        }
        if (player_records[BUBBLES_PLAYER(work)]->anim.frame <= BREATH_FRAME) {
            BREATHING(work) = 1;
            work->timer = BREATH_STEPS;
        }
    }
    bubble_origin(task, &pos, &spd, &head);
    {
        /* @hack spelling: to, from and at stepped through the x, y and z of
         * center, head and pos, where center.x = head.x - pos.x and the like
         * would do */
        Float *to = &center.x;
        Float *from = &head.x;
        Float *at = &pos.x;

        *to = *from - *at;
        to++;
        from++;
        at++;
        *to = *from - *at;
        to++;
        from++;
        at++;
        *to = *from - *at;
    }
    {
        /* @hack spelling: to stepped through center's x, y and z, where
         * center.x *= 2.0F and the like would do */
        Float *to = &center.x;

        *to++ *= 2.0F;
        *to++ *= 2.0F;
        *to *= 2.0F;
    }
    radius = njScalor(&center);
    {
        /* @hack spelling: to and at stepped through the x, y and z of center
         * and pos, where center.x += pos.x and the like would do */
        Float *to = &center.x;
        Float *at = &pos.x;

        *to += *at;
        to++;
        at++;
        *to += *at;
        to++;
        at++;
        *to += *at;
    }
    /* @hack spelling: cx, cy and cz, copies of center's x, y and z, where
     * center.x, center.y and center.z would do */
    cx = center.x;
    cy = center.y;
    cz = center.z;
    drag = PARAM(task, work)->drag;
    lift = PARAM(task, work)->lift;
    growth = PARAM(task, work)->growth;
    wobble_rate = PARAM(task, work)->wobble_rate;
    wobble = PARAM(task, work)->wobble;
    for (bubble = BUBBLE_LIST(task); bubble != NULL; bubble = next) {
        {
            /* @hack spelling: the copy of bubble, where bubble would do */
            struct bubble *moved = bubble;
            /* @hack spelling: at and speed stepped through the bubble's pos
             * and spd and back, and the order of the steps, where
             * bubble->pos.x += bubble->spd.x and the like would do */
            Float *at = &moved->pos.x;
            Float *speed = &moved->spd.x;
            Float vx;
            Float vy;
            Float vz;
            Float px;
            Float py;
            Float pz;

            px = *at++;
            vx = *speed++;
            py = *at++;
            vy = *speed++;
            pz = *at;
            vz = *speed;
            py += vy;
            next = bubble->next;
            vy += lift;
            pz += vz;
            vz *= drag;
            vy *= drag;
            px += vx;
            vx *= drag;
            *speed = vz;
            *--speed = vy;
            *--speed = vx;
            *at = pz;
            *--at = py;
            *--at = px;
        }
        old_size = bubble->size;
        {
            /* @hack spelling: at stepped through the bubble's pos, and -cx
             * and -cy added to it, where bubble->pos.x - cx and the like
             * would do */
            Float *at = &bubble->pos.x;

            dx = -cx + *at++;
            dy = -cy + *at++;
            dz = *at - cz;
        }
        distance = (dx * dx) + (dy * dy) + (dz * dz);
        reach = radius + old_size;
        limit = reach * reach;
        if (limit > distance && distance > PUSH_MIN_SQUARED) {
            Float *at;

            scale = (reach + PUSH_CLEAR) / njSqrt(distance);
            dx *= scale;
            /* @hack spelling: at stepped through the bubble's pos, and at[1]
             * for its z, where bubble->pos.x and the like would do */
            at = &bubble->pos.x;
            dz *= scale;
            dy *= scale;
            *at++ = cx + dx;
            if (dy >= 0.0F) {
                *at++ = cy + dy;
                *at = cz + dz;
            } else {
                at[1] = cz + dz;
            }
        }
        y = bubble->pos.y;
        max_size = bubble->max_size;
        surface = bubble->surface;
        size = bubble->size + growth;
        if (max_size < size) {
            size = max_size;
        }
        /* @hack spelling: depth taken ahead of the test, where the else that
         * reads it would do */
        depth = surface - size - y;
        if (surface <= y) {
            bubble_free(task, bubble);
            /* @bug memory: writes the bubble just freed. BSR at 0x8c0a1d5e
             * frees it, and its wobble and turn below still write it, as
             * MOV.W at 0x8c0a1e00 writes its ang */
#ifdef NONMATCHING
            /* this side repairs the tag above: the wobble and turn write
             * stand_in instead. the wobble still calls rand as the image's
             * does */
            bubble = &stand_in;
#endif
        } else {
            if (depth < 0.0F) {
                size += depth;
            }
            bubble->size = size;
        }
        if (rand() * RAND_SCALE < wobble_rate) {
            /* @hack spelling: speed stepped through the bubble's spd, and
             * speed[1] for its z, where bubble->spd.x and the like would
             * do */
            Float *speed = &bubble->spd.x;

            *speed++ += ((rand() * RAND_SCALE) - 0.5F) * wobble;
            speed[1] += ((rand() * RAND_SCALE) - 0.5F) * wobble;
        }
        bubble->ang += bubble->ang_spd;
    }
    /* @bug logic: a breath out never ends, since this count undoes its count
     * down each step: ADD #-1 at 0x8c0a1ada, and ADD #1 at 0x8c0a1e0e on
     * every path that doesn't kill the task */
    work->timer++;
}

static void bubbles_draw(struct task *task)
{
    struct bubble *bubble;
    NJS_SPRITE sprite;

    sprite.tlist = &effect_texlist;
    sprite.tanim = &bubble_texanim;
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
    for (bubble = BUBBLE_LIST(task); bubble != NULL; bubble = bubble->next) {
        sprite.ang = bubble->ang;
        sprite.p = bubble->pos;
        sprite.sx = bubble->size;
        sprite.sy = bubble->size * bubble->aspect;
        njDrawSprite3D(&sprite, 0,
                       NJD_SPRITE_ANGLE | NJD_SPRITE_SCALE | NJD_SPRITE_ALPHA);
    }
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
    njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

void bubbles_create(int player)
{
    struct task *task;

    /* @hack spelling: the assignment inside the test, where a statement of
     * its own would do */
    if ((task = task_create(TASK_WORK, BUBBLES_LEVEL, bubbles_init,
                            "ObjectMizuiki")) != NULL) {
        task->work->scl.z = player;
    }
}
