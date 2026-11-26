/* CASHBOX in Security Hall's object list: the safes */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <string.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "collision/world.h"
#include "draw/color.h"
#include "hud/hud.h"
#include "object/fall.h"
#include "object/placement.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/dust.h"
#include "sound/sound.h"
#include "stg08/cash_box.h"
#include "stg08/security_hall.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"

#define CORNERS 4
/* far_safes holds the fronts of up to FAR_SAFES placements out of range, and
 * object/fall.h's far_safe_count counts them */
#define FAR_SAFES 120
/* a closed safe does nothing while no player is within ACTIVE_RADIUS of its
 * pos. players meet its copy of security_safe_collision only while the safe
 * is closed and a player is near. near is within the copy's radius +
 * SIZE_MARGIN of the copy's center. world_add classes the copy by that sum
 * too */
#define ACTIVE_RADIUS 65.0F
#define SIZE_MARGIN 10.0F
/* an open safe leaves once no player is within its copy's radius +
 * DISMISS_MARGIN of the copy's center */
#define DISMISS_MARGIN 20.0F
#define OPEN_POINTS 20
/* the sounds, ids in the stage's sound list */
#define UNLOCK_SOUND 2
#define OPEN_SOUND 3
#define SOUND_PRIORITY 0
/* a touch puffs dust at each corner of a square, and a larger puff at its
 * middle. the square stands upright across the safe's z, with its foot on the
 * safe's pos. its side is SQUARE_SIDE. each puff flies along +z, and each
 * corner's out to its side too */
#define SQUARE_SIDE 40.0F
#define SQUARE_HALF 20.0F
#define CORNER_DRIFT 0.1F
#define CORNER_SPEED 0.8F
#define CORNER_SIZE 5.0F
#define MIDDLE_SPEED 0.9F
#define MIDDLE_SIZE 7.0F
/* a touched safe turns its dial, the work's ang.z, by DIAL_SPIN a step for
 * DIAL_STEPS + 1 steps. it then comes ADVANCE_SPEED forward a step for
 * ADVANCE_STEPS + 1 steps. its halves then part, each moving SEPARATE_SPEED a
 * step, for SEPARATE_STEPS + 1 */
#define DIAL_SPIN 0xccc
#define DIAL_STEPS 40
#define ADVANCE_SPEED 0.2F
#define ADVANCE_STEPS 25
#define SEPARATE_SPEED 1.0F
#define SEPARATE_STEPS 50
/* a safe's submode is OPEN_AT_INIT where its area was security_open_area at
 * its init, and 0 otherwise */
#define OPEN_AT_INIT 1
/* a closed safe of OPEN_AT_INIT shows its light while game_state.steps mod
 * BLINK_STEPS is above BLINK_DARK */
#define BLINK_STEPS 48
#define BLINK_DARK 20
/* the work's tick counts the flash's steps from 1, and is 0 while there is
 * none. the flash is white. its depth grows to RISE_DEPTH of flash_depth as
 * it rises. as it fades, its alpha falls from FLASH_ALPHA to 0, and its depth
 * grows by FADE_DEPTH more */
#define FLASH_ALPHA 254.0F
#define WHITE_RGB 0xffffff
#define RISE_DEPTH 0.5F
#define FADE_DEPTH 0.5F

enum safe_mode {
    SAFE_CLOSED,
    SAFE_TURN_DIAL,
    SAFE_ADVANCE,
    SAFE_SEPARATE,
    SAFE_OPEN
};

/* a safe's record, in task->motion */
struct safe {
    /* the copy of security_safe_collision that the collision world holds,
     * placed where the safe lies */
    NJS_OBJECT object;
    NJS_POINT3 center; /* the copy's center in the world */
    Float separation; /* how far each half has moved from its closed place */
    NJS_TEXTURE_VTX front[CORNERS]; /* the front's quad in the world */
};

/* a slot of far_safes: the front of a placement out of range */
struct far_safe {
    int texture; /* SAFE_TEXTURE(work) + 1, or 0 for a free slot */
    NJS_TEXTURE_VTX front[CORNERS];
    struct set_entry *entry; /* the entry of the placement that keeps it */
};

#define SAFE(task) ((struct safe *)(task)->motion.p)

#include "stg08/cash_box/text.inc"

/* each header defines one of the safe's models. the object's data holds them
 * in the order of these includes */
/* clang-format off */
#include "stg08/safe/safe_solid.h"
#include "stg08/safe/safe_right_b.h"
#include "stg08/safe/safe_right_a.h"
#include "stg08/safe/safe_right_c.h"
#include "stg08/safe/safe_left_b.h"
#include "stg08/safe/safe_left_a.h"
#include "stg08/safe/safe_left_c.h"
#include "stg08/safe/safe_front_b.h"
#include "stg08/safe/safe_front_a.h"
#include "stg08/safe/safe_front_c.h"
#include "stg08/safe/safe_closed_a.h"
#include "stg08/safe/safe_closed_b.h"
#include "stg08/safe/safe_closed_c.h"
#include "stg08/safe/safe_flash.h"
/* clang-format on */

NJS_TEXLIST *security_safe_textures[14] = {
#include "stg08/cash_box/security_safe_textures.inc"
};
static struct collision_shape shape = {
#include "stg08/cash_box/shape.inc"
};
static NJS_TEXLIST *right_texlists[SAFE_AREAS] = {
#include "stg08/cash_box/right_texlists.inc"
};
static NJS_TEXLIST *left_texlists[SAFE_AREAS] = {
#include "stg08/cash_box/left_texlists.inc"
};
static NJS_CNK_OBJECT *left_objects[SAFE_AREAS] = {
#include "stg08/cash_box/left_objects.inc"
};
static NJS_CNK_OBJECT *right_objects[SAFE_AREAS] = {
#include "stg08/cash_box/right_objects.inc"
};
static NJS_TEXLIST *front_texlists[SAFE_AREAS] = {
#include "stg08/cash_box/front_texlists.inc"
};
static NJS_TEXLIST *closed_texlists[SAFE_AREAS] = {
#include "stg08/cash_box/closed_texlists.inc"
};
static NJS_CNK_OBJECT *front_objects[SAFE_AREAS] = {
#include "stg08/cash_box/front_objects.inc"
};
static NJS_CNK_OBJECT *closed_objects[SAFE_AREAS] = {
#include "stg08/cash_box/closed_objects.inc"
};
/* a safe within model_distance of the camera draws its models, if its
 * textures are its own area's. any other draws only its front */
static Float model_distance = {
#include "stg08/cash_box/model_distance.inc"
};
/* the flash rises over flash_rise steps, and fades over flash_fade more */
static int flash_rise = {
#include "stg08/cash_box/flash_rise.inc"
};
static int flash_fade = {
#include "stg08/cash_box/flash_fade.inc"
};
static Float flash_depth = {
#include "stg08/cash_box/flash_depth.inc"
};
static NJS_TEXTURE_VTX front_vertices[SAFE_AREAS * CORNERS] = {
#include "stg08/cash_box/front_vertices.inc"
};
static struct far_safe far_safes[FAR_SAFES];

static void safe_destroy(struct task *task);
static void safe_step(struct task *task);
static void safe_draw(struct task *task);

static void place_front(struct task_work *work, NJS_TEXTURE_VTX *vertices,
                        int area)
{
    NJS_TEXTURE_VTX *model = &front_vertices[area * CORNERS];
    NJS_POINT3 local;
    NJS_POINT3 world;
    int i;

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateEx(&work->pos);
    njRotateY(NULL, work->ang.y);
    for (i = 0; i < CORNERS; i++, model++, vertices++) {
        local.x = model->x;
        local.y = model->y;
        local.z = model->z;
        njCalcPoint(NULL, &local, &world);
        vertices->x = world.x;
        vertices->y = world.y;
        vertices->z = world.z;
        vertices->col = model->col;
        vertices->u = model->u;
        vertices->v = model->v;
    }
    njPopMatrixEx();
}

/* keep the front of task's placement in a free slot of far_safes. a
 * placement that holds a slot keeps it. returns the slot's index + 1, which
 * the placement's state holds. returns 0 for a task without a placement
 * entry, or where no slot is free */
static Sint8 register_far(struct task *task)
{
    struct task_work *work = task->work;
    struct set_entry *entry;
    struct far_safe *safe;
    struct far_safe *end;
    Sint8 state;
    int index;
    int slot;
    Uint32 saved_slot;

    if (!far_safe_count) {
        memset(far_safes, 0, sizeof(far_safes));
    }
    /* @hack spelling: task->placement tested again inside the test of its
     * entry, where !task->placement->entry would do */
    if (!task->placement ||
        (task->placement ? task->placement->entry : NULL) == NULL) {
        return 0;
    } else {
        entry = task->placement->entry;
    }
    state = placement_get_state(task);
    /* @hack redundant: a second test of entry, which the test above makes
     * needless */
    if (!entry) {
        return 0;
    }
    if (state) {
        return state;
    } else {
        /* @hack spelling: saved_slot, a Uint32 that counts beside slot, where
         * slot alone would do */
        slot = 1;
        saved_slot = 1;
        safe = far_safes;
        end = far_safes + FAR_SAFES;
        for (; safe < end; safe++) {
            if (safe->texture && safe->entry == entry) {
                placement_set_state(task, saved_slot);
                return slot;
            }
            slot = slot + 1;
            saved_slot = saved_slot + 1;
        }
    }
    /* @hack spelling: slot, which counts beside index, where index + 1 would
     * do */
    slot = 1;
    index = 0;
    while (index < FAR_SAFES) {
        if (!far_safes[index].texture) {
            state = slot;
            far_safes[index].texture = SAFE_TEXTURE(work) + 1;
            far_safes[index].entry = entry;
            place_front(work, far_safes[index].front, SAFE_AREA(work));
            far_safe_count++;
            placement_set_state(task, state);
            return state;
        }
        index++;
        slot = slot + 1;
    }
    return 0;
}

/* free the slot of far_safes that task's placement holds, if it holds one */
static void release_far(struct task *task)
{
    Sint8 slot;

    if (far_safe_count && task->placement) {
        slot = placement_get_state(task);
        if (slot) {
            far_safes[slot - 1].texture = 0;
            placement_set_state(task, 0);
            far_safe_count--;
        }
    }
}

void security_cash_boxes_draw(void)
{
    int i;

    if (far_safe_count) {
        for (i = 0; i < FAR_SAFES; i++) {
            if (far_safes[i].texture) {
                njSetTexture(front_texlists[far_safes[i].texture - 1]);
                njSetTextureNum(0);
                njDrawTexture3DEx(far_safes[i].front, CORNERS, 0);
            }
        }
    }
}

void security_cash_box_init(struct task *task)
{
    struct task_work *work = task->work;
    struct safe *safe;
    Uint32 flags;
    Float radius;

    if (placement_try_leave(task)) {
        register_far(task);
        return;
    }
    task->motion.p = syCalloc(1, sizeof(struct safe));
    if (!task->motion.p) {
        return;
    }
    task->draw = safe_draw;
    task->step = safe_step;
    task->destructor = safe_destroy;
    work->mode = SAFE_CLOSED;
    if (security_open_area == SAFE_AREA(work)) {
        work->submode = OPEN_AT_INIT;
    } else {
        work->submode = 0;
    }
    safe = SAFE(task);
    safe->object = security_safe_collision;
    safe->object.evalflags &= ~(NJD_EVAL_UNIT_POS | NJD_EVAL_UNIT_ANG);
    safe->object.ang[1] = work->ang.y;
    safe->object.ang[2] = 0;
    safe->object.ang[0] = 0;
    safe->object.pos[0] = work->pos.x;
    safe->object.pos[1] = work->pos.y;
    safe->object.pos[2] = work->pos.z;
    safe->object.child = NULL;
    safe->object.sibling = NULL;
    flags = WORLD_SOLID | (work->submode == OPEN_AT_INIT ? WORLD_DUSTY : 0);
    radius = safe->object.model->r + SIZE_MARGIN;
    if (radius <= WORLD_SIZE_CLASS_1_MAX) {
        if (radius > WORLD_SIZE_CLASS_2_MAX) {
            flags |= WORLD_SIZE_CLASS(1);
        } else if (radius > WORLD_SIZE_CLASS_3_MAX) {
            flags |= WORLD_SIZE_CLASS(2);
        } else {
            flags |= WORLD_SIZE_CLASS(3);
        }
    }
    world_add(flags, task, &safe->object);
    njPushMatrixEx();
    njUnitMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateY(NULL, work->ang.y);
    /* @hack spelling: center set through SAFE(task), where safe would do */
    njCalcPoint(NULL, &safe->object.model->center, &SAFE(task)->center);
    njPopMatrix(1);
    collision_init(task, &shape, 1, COLLISION_OBJECT);
    work->ang.z = 0;
    /* @hack spelling: separation and front set through SAFE(task), where
     * safe would do */
    SAFE(task)->separation = 0.0F;
    place_front(work, SAFE(task)->front, SAFE_AREA(work));
    release_far(task);
}

static void safe_destroy(struct task *task)
{
    if (SAFE(task)->object.model) {
        world_remove(task, &SAFE(task)->object);
    }
    syFree(task->motion.p);
    task->motion.p = NULL;
    task->extra.p = NULL;
    /* @hack redundant: a second clear of task->motion */
    task->motion.p = NULL;
}

/* make a dust_short_create puff of size scale at (at_x, at_y, 0) in the
 * current matrix's frame. the puff moves at (dx, 0, dz) in that frame. the
 * caller declares local and point as NJS_POINT3s, and direction and velocity
 * as NJS_VECTORs, which the macro writes */
#define DUST(at_x, at_y, dx, dz, scale)                                        \
    local.x = (at_x);                                                          \
    local.y = (at_y);                                                          \
    local.z = 0.0F;                                                            \
    direction.x = (dx);                                                        \
    direction.y = 0.0F;                                                        \
    direction.z = (dz);                                                        \
    njCalcPoint(NULL, &local, &point);                                         \
    njCalcVector(NULL, &direction, &velocity);                                 \
    dust_short_create(&point, &velocity, (scale))

#pragma inline(add_position)
static void add_position(NJS_VECTOR *step, NJS_POINT3 *pos)
{
    Float *to = ((union vector_floats *)pos)->component;
    Float *from = ((union vector_floats *)step)->component;
    *to++ += *from++;
    *to++ += *from++;
    *to += *from;
}

static void safe_step(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 local;
    NJS_VECTOR direction;
    NJS_VECTOR velocity;
    NJS_POINT3 point;
    int near;

    if (placement_try_leave(task)) {
        register_far(task);
        return;
    }
    /* @hack redundant: a block no path reaches, which tests ang.x against
     * 0xf0 twice and the area once, each with an empty body */
    if (0) {
        if (work->ang.x == 0xf0) {
        }
        if (work->ang.x == 0xf0) {
        }
        if (SAFE_AREA(work)) {
        }
    }
    near = player_near(&work->pos, ACTIVE_RADIUS);
    if (!near && work->mode == SAFE_CLOSED) {
        work->flags &= ~WORLD_ADDED;
        return;
    }
    /* @hack redundant: a block no path reaches, which tests near with an
     * empty body */
    if (0) {
        if (near) {
        }
    }
    if (SAFE(task)->object.model) {
        if (work->mode == SAFE_CLOSED &&
            player_near(&SAFE(task)->center,
                        SAFE(task)->object.model->r + SIZE_MARGIN)) {
            work->flags |= WORLD_ADDED;
        } else {
            work->flags &= ~WORLD_ADDED;
        }
    }
    switch (work->mode) {
    case SAFE_CLOSED:
        if (collision_next_player(task)) {
            work->timer = DIAL_STEPS;
            work->mode = SAFE_TURN_DIAL;
            sound_play(UNLOCK_SOUND, NULL, SOUND_PRIORITY, 0);
            njPushMatrixEx();
            njUnitMatrix(NULL);
            njTranslateV(NULL, &work->pos);
            njRotateY(NULL, work->ang.y);
            DUST(SQUARE_HALF, 0.0F, CORNER_DRIFT, CORNER_SPEED, CORNER_SIZE);
            DUST(-SQUARE_HALF, 0.0F, -CORNER_DRIFT, CORNER_SPEED, CORNER_SIZE);
            DUST(-SQUARE_HALF, SQUARE_SIDE, -CORNER_DRIFT, CORNER_SPEED,
                 CORNER_SIZE);
            DUST(SQUARE_HALF, SQUARE_SIDE, CORNER_DRIFT, CORNER_SPEED,
                 CORNER_SIZE);
            DUST(0.0F, SQUARE_HALF, 0.0F, MIDDLE_SPEED, MIDDLE_SIZE);
            njPopMatrix(1);
        } else {
            /* a closed safe kills its task once its area has opened or
             * closed since its init. the submode says whether it was open
             * then */
            if (security_open_area == SAFE_AREA(work)) {
                if (work->submode == 0) {
                    task_kill(task);
                    break;
                }
            } else if (work->submode) {
                task_kill(task);
                break;
            }
            collision_register(task);
        }
        break;
    case SAFE_TURN_DIAL:
        work->ang.z += DIAL_SPIN;
        if (work->timer-- <= 0) {
            work->mode = SAFE_ADVANCE;
            work->timer = ADVANCE_STEPS;
            work->tick = 1;
            sound_play(OPEN_SOUND, NULL, SOUND_PRIORITY, 0);
        }
        break;
    case SAFE_ADVANCE:
        if (work->tick) {
            work->tick++;
            if (work->tick > flash_rise + flash_fade) {
                work->tick = 0;
            }
        }
        local.x = 0.0F;
        local.y = 0.0F;
        local.z = ADVANCE_SPEED;
        njPushMatrixEx();
        njUnitMatrix(NULL);
        njRotateY(NULL, work->ang.y);
        njCalcVector(NULL, &local, &local);
        njPopMatrix(1);
        /* @hack spelling: the step added by add_position, a Float at a time
         * through pointers, where a sum of each coordinate would do */
        add_position(&local, &work->pos);
        if (work->timer-- > 0) {
            break;
        }
        work->mode = SAFE_SEPARATE;
        work->timer = SEPARATE_STEPS;
        break;
    case SAFE_SEPARATE:
        if (work->tick) {
            work->tick++;
            if (work->tick > flash_rise + flash_fade) {
                work->tick = 0;
            }
        }
        SAFE(task)->separation += SEPARATE_SPEED;
        if (work->timer-- <= 0) {
            work->mode = SAFE_OPEN;
            work->timer = 0;
            work->tick = 0;
        }
        break;
    case SAFE_OPEN:
        if (!player_near(&SAFE(task)->center,
                         SAFE(task)->object.model->r + DISMISS_MARGIN)) {
            if (task->placement) {
                placement_kill(task);
            } else {
                task_kill(task);
            }
            score_add(OPEN_POINTS);
        }
        break;
    }
}

static void safe_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float scale;
    Float alpha;
    Float fade;
    Uint32 color;
    NJS_CNK_OBJECT *door;
    NJS_CNK_OBJECT *dial;

    /* @hack redundant: statements kept for their strings, the set editor's
     * labels for ang.x and ang.y, which SHC places in this source's
     * constants */
    (void)SAFE_EDITOR_MODEL_NUMBER;
    (void)SAFE_EDITOR_ANGLE_Y;

    if (SAFE_AREAS_ON(work) == 0 &&
        njDistanceP2P(&camera_views[camera_current_index]->pos, &work->pos) <
            model_distance) {
        njPushMatrixEx();
        njTranslateEx(&work->pos);
        njRotateY(NULL, work->ang.y);
        if (work->tick > 0) {
            if (work->tick < flash_rise) {
                scale = (Float)work->tick / (Float)flash_rise;
                alpha = 1.0F;
                scale *= RISE_DEPTH;
            } else {
                fade = 1.0F - (((Float)work->tick - (Float)flash_rise) /
                               (Float)flash_fade);
                alpha = fade;
                scale = ((1.0F - fade) * FADE_DEPTH) + RISE_DEPTH;
            }
            if (scale > 1.0F) {
                scale = 1.0F;
            }
            if (scale < 0.0F) {
                scale = 0.0F;
            }
            if (alpha > 1.0F) {
                alpha = 1.0F;
            }
            if (alpha < 0.0F) {
                alpha = 0.0F;
            }
            alpha *= FLASH_ALPHA;
            /* @hack type: alpha converted to an int, then to a Uint32, where
             * (Uint32)alpha would do */
            color = ((Uint32)(int)alpha << ALPHA_SHIFT) | WHITE_RGB;
            njSetTexture(&security_safe_flash_texlist);
            njPushMatrixEx();
            njScale(NULL, 1.0F, 1.0F, flash_depth * scale);
            njFogDisable();
            model_draw_tinted(security_safe_flash->model, color);
            njFogEnable();
            njPopMatrix(1);
        }
        if (SAFE(task)->separation == 0.0F && work->mode < SAFE_ADVANCE) {
            door = closed_objects[SAFE_AREA(work)];
            njSetTexture(closed_texlists[SAFE_TEXTURE(work)]);
            draw_control_set(DRAW_CONTROL_SHADOW);
            njCnkEasyDrawModel(door->model);
            draw_control_clear(DRAW_CONTROL_SHADOW);
            /* the door's child is the dial, and the dial's sibling the light */
            dial = door->child;
            if (game_state.steps % BLINK_STEPS > BLINK_DARK &&
                work->mode == SAFE_CLOSED && work->submode == OPEN_AT_INIT) {
                njCnkEasyDrawModel(dial->sibling->model);
            }
            if (work->ang.z) {
                njTranslateV(NULL, &dial->model->center);
                njRotateZ(NULL, work->ang.z);
                njTranslate(NULL, -dial->model->center.x,
                            -dial->model->center.y, -dial->model->center.z);
            }
            njCnkEasyDrawModel(dial->model);
        } else {
            draw_control_set(DRAW_CONTROL_SHADOW);
            njTranslate(NULL, SAFE(task)->separation, 0.0F, 0.0F);
            njSetTexture(right_texlists[SAFE_TEXTURE(work)]);
            njCnkEasyDrawModel(right_objects[SAFE_AREA(work)]->model);
            njTranslate(NULL, -(SAFE(task)->separation * 2.0F), 0.0F, 0.0F);
            njSetTexture(left_texlists[SAFE_TEXTURE(work)]);
            njCnkEasyDrawModel(left_objects[SAFE_AREA(work)]->model);
            draw_control_clear(DRAW_CONTROL_SHADOW);
        }
        njPopMatrix(1);
    } else {
        njSetTexture(front_texlists[SAFE_TEXTURE(work)]);
        njSetTextureNum(0);
        njDrawTexture3DEx(SAFE(task)->front, CORNERS, 0);
    }
}
