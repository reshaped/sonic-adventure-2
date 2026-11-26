/* the switch, SWITCH in a stage's object list: a button that turns a switch
 * id on or off. other objects test the ids. players push its cap, and so do
 * the pressers, points other objects add. SSS in an object list is a falling
 * cylinder whose pos is among the pressers */
#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/chunk.h"
#include "draw/lines.h"
#include "effect/particle.h"
#include "object/fall.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/common_object.h"
#include "shared/switch.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/util.h"
#include "shared/switch/text.inc"

enum switch_mode { MODE_INIT, MODE_READY };

/* fields of the work. the switch is pressed while PRESS_STEPS_LEFT isn't 0.
 * LIT is 1 while the lamp is lit, and 0 otherwise. TIME_LEFT is a
 * SWITCH_TIMED switch's steps until it turns its id off */
#define PRESS_STEPS_LEFT(work) ((work)->tick)
#define LIT(work) ((work)->submode)
#define TIME_LEFT(work) ((work)->timer)

/* the cap's rest point lies CAP_HEIGHT above the switch's pos. a step holds
 * the cap within CAP_TRAVEL of that point, and holds cap.y at CAP_LOWEST or
 * more. the cap's pushers then move it, and can carry it past either bound */
#define CAP_HEIGHT 5.0F
#define CAP_TRAVEL 8.0F
#define CAP_LOWEST (-5.0F)
/* the spring pulls the cap toward its rest point. each step, the cap's speed
 * loses SPRING times its offset from that point. the spring acts only while
 * the offset is longer than SPRING_SLACK */
#define SPRING 0.01875F
#define SPRING_SLACK 0.001F
/* each step keeps CAP_DAMPING of the cap's speed. the speed then gains
 * CAP_FOLLOW times the push. the push is the mean place the cap's pushers
 * push it to, less its place before them. the speed's length is then held to
 * CAP_SPEED_MAX */
#define CAP_DAMPING 0.95F
#define CAP_FOLLOW 0.2F
#define CAP_SPEED_MAX 25.0F
/* the cap rests while it lies within REST_DISTANCE of its rest point and
 * moves slower than REST_SPEED */
#define REST_DISTANCE 1.0F
#define REST_SPEED 0.5F
/* a step that finds the cap farther than PRESS_DISTANCE from its rest point
 * sets PRESS_STEPS_LEFT to PRESS_STEPS. a step that finds the cap resting
 * takes 1 from PRESS_STEPS_LEFT, down to 0 */
#define PRESS_DISTANCE 3.2F
#define PRESS_STEPS 10
/* the players of the slots below PUSHING_PLAYERS push the cap, up to the
 * first empty slot */
#define PUSHING_PLAYERS 2
/* how far a player and a presser keep the cap's center from them, as
 * switch_push_cap measures it */
#define PLAYER_REACH 8.0F
#define PRESSER_REACH 12.0F
/* a pusher's distance from the cap's center counts only the depth below the
 * center past DEPTH_SLACK */
#define DEPTH_SLACK 20.0F
/* the cap leans as if it topped a stalk STALK_LENGTH long. away is how far
 * the stalk's top lies from upright, across x and z. along is the top's
 * offset on x alone, or on z alone. STALK_LEAN(along, away) is the stalk's
 * angle from upright toward that axis */
#define STALK_LENGTH 8.1F
#define STALK_LEAN(along, away)                                                \
    ((Angle)(atan2f((along), njSqrt((STALK_LENGTH * STALK_LENGTH) -            \
                                    ((away) * (away)))) *                      \
             ANGLE_PER_RADIAN))

/* a SWITCH_TIMED switch's time is scl.z, truncated, in steps. an scl.z not
 * strictly between TIME_MIN and TIME_MAX gives TIME_MIN */
#define TIME_MIN 5
#define TIME_MAX 30000.0F
/* while the time runs, the lamp blinks. a blink's slack is the time left
 * less BLINK_FAST. the lamp stays lit for 2 steps plus BLINK_LIT of the
 * slack, truncated. it stays dark for 2 steps plus BLINK_DARK of the slack,
 * truncated. a slack below 0 counts as 0 */
#define BLINK_FAST 40
#define BLINK_LIT 0.06F
#define BLINK_DARK 0.04F

/* a lit lamp's glow is GLOW_FULL. an unlit lamp's glow falls by GLOW_FADE a
 * step while it is GLOW_FADE_LEAST or more. below that it drops to
 * GLOW_UNLIT, where it stays */
#define GLOW_FULL 255
#define GLOW_FADE 30
#define GLOW_FADE_LEAST 50
#define GLOW_UNLIT 20
/* switch_draw's light is the glow over GLOW_SCALE, under 1 even at
 * GLOW_FULL. a SWITCH_TIMED switch's light also flickers, multiplied by
 * blink_level(FLICKER_PERIOD, FLICKER_REST, FLICKER_FADE) */
#define GLOW_SCALE 256.0F
#define FLICKER_PERIOD 30
#define FLICKER_REST 15
#define FLICKER_FADE 6
/* while the light is above LIGHT_UNSHADED, the pad and the cap draw unshaded
 * and without fog */
#define LIGHT_UNSHADED 0.5F
/* the cap's scale wobbles with game_state.steps. its x and z swing by
 * WOBBLE_XZ, at WOBBLE_XZ_RATE degrees a step. z runs WOBBLE_Z_LAG steps
 * ahead of x. its y swings by WOBBLE_Y, at WOBBLE_Y_RATE degrees a step.
 * while the id is on, the cap spins SPIN_RATE degrees a step */
#define WOBBLE_XZ 0.1F
#define WOBBLE_XZ_RATE 5
#define WOBBLE_Z_LAG 30
#define WOBBLE_Y 0.04F
#define WOBBLE_Y_RATE 3
#define SPIN_RATE 2
/* the byte of a packed color at bit shift, scaled to run from 0 to 1 */
#define COLOR_PART(color, shift)                                               \
    ((((color) >> (shift)) & 0xff) * (1.0F / 255.0F))

#define ON_SOUND 0x800e
#define OFF_SOUND 0x1002

/* the cylinder SSS draws as, which stands on its pos */
#define SSS_RADIUS 5.0F
#define SSS_HEIGHT 10.0F

/* a switch's record, in task->extra */
struct switch_state {
    /* 1 if the switch was pressed at its last step, and 0 if not. only
     * switch_toggle_step uses it */
    Sint8 was_pressed;
    Uint8 glow; /* the lamp's glow, up to GLOW_FULL */
    /* a SWITCH_TIMED switch's blink countdown. the step that finds it 0
     * turns the lamp on or off */
    Sint16 blink;
    Uint16 kind; /* a switch_kind */
    Uint16 id;
    Sint32 unk_08;
    Angle tilt_x; /* the cap's lean about x */
    Sint32 unk_10;
    Angle tilt_z; /* the cap's lean about z */
    /* 1 where the step found the cap past PRESS_DISTANCE, else 0. nothing
     * reads it */
    Sint32 pressed;
    /* 1 from a press until the cap rests. nothing reads it */
    Sint32 settling;
    NJS_POINT3 cap; /* the cap's place, from its rest point */
    NJS_VECTOR cap_spd;
};

#define STATE(task) ((struct switch_state *)(task)->extra.p)

static void switch_init(struct task *task);
static void switch_state_alloc(struct task *task);
static void switch_ready_step(struct task *task);
static void switch_draw(struct task *task);
static void switch_destroy(struct task *task);
static void switch_toggle_step(struct task *task);
static void switch_push_step(struct task *task);
static void switch_timed_step(struct task *task);
static void switch_start_timer(struct task_work *work);
static void switch_start_blink(struct task *task);
static void switch_on_only_step(struct task *task);
static void switch_light(struct task *task, Bool on);
static void switch_fade(struct task *task);
static void switch_turn(struct task *task, Bool on);
static Bool switch_own_id_is_on(struct task *task);
static Bool switch_was_written(Uint32 id);
static void sss_step(struct task *task);
static void sss_destroy(struct task *task);
static void sss_draw(struct task *task);

static NJS_TEXNAME switch_texname[1] = {
#include "shared/switch/switch_texname.inc"
};
NJS_TEXLIST switch_texlist = {
#include "shared/switch/switch_texlist.inc"
};
/* the switch's models: the pad, an octagon that lies on the base, the cap,
 * and the base */
static Sint16 switch_pad_polygons[95] = {
#include "shared/switch/switch_pad_polygons.inc"
};

#define PAD_POINTS 9
static Sint32 switch_pad_vertices[57] = {
#include "shared/switch/switch_pad_vertices.inc"
};

static NJS_CNK_MODEL switch_pad_model = {
#include "shared/switch/switch_pad_model.inc"
};
/* the models' nodes, in which the base's child is the cap, and the cap's
 * sibling the pad. switch_draw draws each model alone */
static NJS_CNK_OBJECT switch_pad_object = {
#include "shared/switch/switch_pad_object.inc"
};
static Sint16 switch_cap_polygons[615] = {
#include "shared/switch/switch_cap_polygons.inc"
};

#define CAP_POINTS 50
static Sint32 switch_cap_vertices[303] = {
#include "shared/switch/switch_cap_vertices.inc"
};

static NJS_CNK_MODEL switch_cap_model = {
#include "shared/switch/switch_cap_model.inc"
};
static NJS_CNK_OBJECT switch_cap_object = {
#include "shared/switch/switch_cap_object.inc"
};
static Sint16 switch_base_polygons[223] = {
#include "shared/switch/switch_base_polygons.inc"
};

#define BASE_POINTS 24
static Sint32 switch_base_vertices[147] = {
#include "shared/switch/switch_base_vertices.inc"
};

static NJS_CNK_MODEL switch_base_model = {
#include "shared/switch/switch_base_model.inc"
};
static NJS_CNK_OBJECT switch_base_object = {
#include "shared/switch/switch_base_object.inc"
};

/* the kinds' names, for switch_step's block that no path reaches */
static const char *switch_kind_names[5] = {
#include "shared/switch/switch_kind_names.inc"
};
static NJS_POINT3 *switch_pressers[SWITCH_PRESSERS] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
};
/* switch_draw blends the cap's color from switch_cap_unlit_color toward
 * switch_lit_color by the light. it blends the pad's from
 * switch_pad_unlit_color by the glow over GLOW_SCALE */
static Uint32 switch_cap_unlit_color = 0xffb0b0bf;
static Uint32 switch_pad_unlit_color = 0xffffffff;
static Uint32 switch_lit_color = 0xffeeeedd;
#define SSS_SHAPES 2
/* clang-format off */
static struct collision_shape sss_shapes[SSS_SHAPES] = {
    {0, SHAPE_CYLINDER, PUSH_FIRM(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 5.0F, 0.0F}, 10.2F, 6.0F, 0.0F, 0, {0, 0, 0}},
    {0, SHAPE_CYLINDER, PUSH_SOLID(PUSH_ALL_BUT_PICKUPS),
     0, 0, {0.0F, 4.5F, 0.0F}, 2.6F, 6.0F, 0.0F, 0, {0, 0, 0}},
};
/* clang-format on */

static int switch_pressed(struct task *task)
{
    /* @hack spelling: PRESS_STEPS_LEFT's truth as a conditional of 1 and 0,
     * where PRESS_STEPS_LEFT != 0 would do */
    return PRESS_STEPS_LEFT(task->work) ? 1 : 0;
}

NJS_POINT3 **switch_presser_add(NJS_POINT3 *pos)
{
    NJS_POINT3 **slot;
    int i;

    for (i = 0, slot = switch_pressers; i < SWITCH_PRESSERS; i++, slot++) {
        if (*slot == NULL) {
            *slot = pos;
            return slot;
        }
    }
    return NULL;
}

NJS_POINT3 **switch_presser_remove(NJS_POINT3 *pos)
{
    NJS_POINT3 **slot;
    int i;

    for (i = 0, slot = switch_pressers; i < SWITCH_PRESSERS; i++, slot++) {
        if (*slot == pos) {
            *slot = NULL;
            return slot;
        }
    }
    return NULL;
}

void switch_step(struct task *task)
{
    struct task_work *work = task->work;
    char text[256];

    if (placement_try_leave(task)) {
        return;
    }
    /* @hack redundant: a block no path reaches, kept for the strings SHC
     * places in the object, with its cases' code and its frame */
    if (0) {
        switch (work->mode) {
        case MODE_INIT:
            switch_init(task);
            break;
        case MODE_READY:
            switch_destroy(task);
            switch_state_alloc(task);
            break;
        /* @hack redundant: an empty default */
        default:
            break;
        }
        njPrintC(NJM_LOCATION(17, 16), "<- ANGLE X");
        njPrintC(NJM_LOCATION(17, 17), "<- ANGLE Y");
        njPrintC(NJM_LOCATION(17, 18), "<- ANGLE Z");
        sprintf(text, "<- KIND:%s", switch_kind_names[STATE(task)->kind]);
        njPrintC(NJM_LOCATION(17, 19), text);
        njPrintC(NJM_LOCATION(17, 20), "<- ID");
        if (STATE(task)->kind == SWITCH_TIMED) {
            njPrintC(NJM_LOCATION(17, 21), "<- TIMER");
        }
        return;
    }
    switch (work->mode) {
    case MODE_INIT:
        switch_init(task);
        break;
    case MODE_READY:
        switch_ready_step(task);
        break;
    }
}

static void switch_init(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;

    task->destructor = switch_destroy;
    task->draw = switch_draw;
    switch_state_alloc(task);
    work->mode = MODE_READY;
}

static void switch_state_alloc(struct task *task)
{
    struct task_work *work = task->work;
    Uint32 kind;
    Uint16 id;

    task->extra.p = syCalloc(1, sizeof(struct switch_state));
    kind = (Uint32)work->scl.x;
    if (kind > SWITCH_UNDEFINED) {
        kind = SWITCH_UNDEFINED;
    }
    STATE(task)->kind = kind;
    id = (Uint32)work->scl.y;
    if (id >= SWITCH_IDS) {
        id = SWITCH_IDS - 1;
    }
    STATE(task)->id = id;
    PRESS_STEPS_LEFT(work) = 0;
    TIME_LEFT(work) = 0;
    /* @hack redundant: the record's fields set to 0, though syCalloc cleared
     * the record */
    STATE(task)->was_pressed = 0;
    LIT(work) = 0;
    STATE(task)->glow = 0;
    STATE(task)->tilt_x = 0;
    STATE(task)->unk_10 = 0;
    STATE(task)->tilt_z = 0;
    STATE(task)->pressed = 0;
    STATE(task)->settling = 0;
    STATE(task)->cap.x = 0.0F;
    STATE(task)->cap.y = 0.0F;
    STATE(task)->cap.z = 0.0F;
    STATE(task)->cap_spd.x = 0.0F;
    STATE(task)->cap_spd.y = 0.0F;
    STATE(task)->cap_spd.z = 0.0F;
}

/* set *out to the place pusher pushes center to, and return 1. a pusher
 * reach or more from center doesn't push it, and *out is center. a pusher
 * above center pushes it out to reach from the pusher, along the line between
 * them. one no higher than center pushes it across x and z alone */
static int switch_push_cap(NJS_POINT3 *center, Float reach, NJS_POINT3 *pusher,
                           NJS_POINT3 *out)
{
    Float x;
    Float y;
    Float z;
    Float px;
    Float py;
    Float pz;
    Float dx;
    Float dy;
    Float dz;
    Float dx2;
    Float dz2;
    Float reach2;
    Float distance2;
    Float distance;
    Float scale;

    reach2 = reach * reach;
    x = center->x;
    y = center->y;
    z = center->z;
    px = pusher->x;
    py = pusher->y;
    pz = pusher->z;
    dx = px - x;
    dx2 = dx * dx;
    dy = py - y;
    dz = pz - z;
    dz2 = dz * dz;
    if (dy > 0.0F) {
        distance2 = dx2 + (dy * dy) + dz2;
        /* @hack redundant: a test of distance2 that no path reaches, which
         * leaves no code */
        if (0) {
            if (distance2) {
            }
        }
        distance = njSqrt(distance2);
        if (distance2 >= reach2 && distance2 != 0.0F) {
            out->x = x;
            out->y = y;
            out->z = z;
        } else {
            scale = reach / distance;
            /* @hack redundant: a test of y and three of scale that no path
             * reaches, which leave no code */
            if (0) {
                if (y) {
                }
                if (scale) {
                }
                if (scale) {
                }
                if (scale) {
                }
            }
            dx *= scale;
            dy *= scale;
            out->x = px - dx;
            out->y = py - dy;
            /* @hack spelling: dz scaled where it is read, where dz *= scale
             * beside dx's and dy's would do */
            out->z = pz - (dz * scale);
        }
    } else {
        dy = _builtin_fabsf(dy);
        if (dy >= DEPTH_SLACK) {
            /* @hack spelling: the add of a negated DEPTH_SLACK, where its
             * subtraction would do */
            dy += -DEPTH_SLACK;
            dx2 += dy * dy;
        }
        distance2 = dx2 + dz2;
        distance = njSqrt(distance2);
        /* @bug arithmetic: divides reach by a distance of 0 where pusher lies
         * at center, or straight below it by DEPTH_SLACK or less. this test
         * sends a distance2 of 0 to the division, and out.x and out.z are then
         * not a number. FCMP/EQ at 0x356 of the object's code makes the test,
         * and FDIV at 0x370 the division */
        if (distance2 >= reach2 && distance2 != 0.0F) {
            out->x = x;
            out->y = y;
            out->z = z;
        } else {
            scale = reach / distance;
            dx *= scale;
            dz *= scale;
            out->x = px - dx;
            out->y = y;
            out->z = pz - dz;
        }
    }
    return 1;
}

/* move the cap by its speed. then move it to the mean of the places its
 * pushers push it to, if any. its pushers are the players and the pressers.
 * it also updates the cap's speed, its lean and PRESS_STEPS_LEFT */
static void switch_move_cap(struct task *task)
{
    struct task_work *work = task->work;
    Float vx = STATE(task)->cap_spd.x;
    Float vy = STATE(task)->cap_spd.y;
    Float vz = STATE(task)->cap_spd.z;
    Float x;
    Float y;
    Float z;
    Float length;
    Float scale;
    Float across2;
    NJS_POINT3 center;
    NJS_POINT3 sum;
    NJS_POINT3 relative;
    NJS_POINT3 pushed;
    NJS_POINT3 *point;
    NJS_POINT3 **slot;
    int count;
    int i;

    x = STATE(task)->cap.x;
    y = STATE(task)->cap.y;
    z = STATE(task)->cap.z;
    x += vx;
    y += vy;
    z += vz;
    /* @hack spelling: the squared length set in a statement of its own,
     * where njSqrt of the sum would do */
    length = (x * x) + (y * y) + (z * z);
    length = njSqrt(length);
    if (length > SPRING_SLACK) {
        scale = (length * SPRING) / length;
        /* @hack spelling: each product negated and added, where its
         * subtraction would do */
        /* @hack spelling: z's product before y's, where the order x, y, z
         * would do */
        vx += -(x * scale);
        vz += -(z * scale);
        vy += -(y * scale);
    }
    STATE(task)->pressed = 0;
    if (length < REST_DISTANCE) {
        if ((vx * vx) + (vy * vy) + (vz * vz) < REST_SPEED * REST_SPEED) {
            STATE(task)->settling = 0;
            if (PRESS_STEPS_LEFT(work) > 0) {
                PRESS_STEPS_LEFT(work)--;
            }
        }
    }
    if (length > PRESS_DISTANCE) {
        STATE(task)->settling = 1;
        STATE(task)->pressed = 1;
        PRESS_STEPS_LEFT(work) = PRESS_STEPS;
    }
    if (length > CAP_TRAVEL) {
        scale = CAP_TRAVEL / length;
        /* @hack spelling: x = x * scale, where x *= scale as y and z have
         * would do */
        x = x * scale;
        y *= scale;
        z *= scale;
    }
    if (y < CAP_LOWEST) {
        y = CAP_LOWEST;
    }
    vx *= CAP_DAMPING;
    vy *= CAP_DAMPING;
    vz *= CAP_DAMPING;
    center.x = x;
    center.y = y + CAP_HEIGHT;
    center.z = z;
    sum.x = 0.0F;
    sum.y = 0.0F;
    sum.z = 0.0F;
    count = 0;
    for (i = 0; i < PUSHING_PLAYERS && player_work[i] != NULL; i++) {
        point = &player_work[i]->pos;
        relative.x = point->x - work->pos.x;
        relative.y = point->y - work->pos.y;
        relative.z = point->z - work->pos.z;
        if (switch_push_cap(&center, PLAYER_REACH, &relative, &pushed)) {
            sum.x += pushed.x;
            sum.y += pushed.y;
            sum.z += pushed.z;
            count++;
        }
    }
    for (i = 0, slot = switch_pressers; i < SWITCH_PRESSERS; i++, slot++) {
        point = *slot;
        if (point != NULL) {
            relative.x = point->x - work->pos.x;
            relative.y = point->y - work->pos.y;
            relative.z = point->z - work->pos.z;
            if (switch_push_cap(&center, PRESSER_REACH, &relative, &pushed)) {
                sum.x += pushed.x;
                sum.y += pushed.y;
                sum.z += pushed.z;
                count++;
            }
        }
    }
    if (count > 1) {
        scale = 1.0F / count;
        sum.x *= scale;
        sum.y *= scale;
        sum.z *= scale;
    } else if (count == 0) {
        sum = center;
    }
    x = sum.x;
    y = sum.y - CAP_HEIGHT;
    z = sum.z;
    vx += (x - center.x) * CAP_FOLLOW;
    vy += (sum.y - center.y) * CAP_FOLLOW;
    vz += (z - center.z) * CAP_FOLLOW;
    length = njSqrt((vx * vx) + (vy * vy) + (vz * vz));
    if (length > CAP_SPEED_MAX) {
        scale = CAP_SPEED_MAX / length;
        /* @hack spelling: vz scaled before vy, where the order vx, vy, vz
         * would do */
        vx *= scale;
        vz *= scale;
        vy *= scale;
    }
    STATE(task)->cap.x = x;
    STATE(task)->cap.y = y;
    STATE(task)->cap.z = z;
    STATE(task)->cap_spd.x = vx;
    STATE(task)->cap_spd.y = vy;
    STATE(task)->cap_spd.z = vz;
    /* the cap leans toward its place across x and z, held within
     * CAP_TRAVEL of rest. tilt_z leans it by x as if z were 0, and tilt_x
     * leans it by z. where x and z are both 0, tilt_z keeps its value */
    across2 = (x * x) + (z * z);
    length = njSqrt(across2);
    if (length >= CAP_TRAVEL) {
        scale = CAP_TRAVEL / length;
        length = CAP_TRAVEL;
        x *= scale;
        z *= scale;
    }
    if (across2 != 0.0F) {
        STATE(task)->tilt_z = -STALK_LEAN(x, x);
    }
    STATE(task)->tilt_x = STALK_LEAN(z, length);
}

static void switch_ready_step(struct task *task)
{
    switch_move_cap(task);
    switch (STATE(task)->kind) {
    /* @hack spelling: SWITCH_TOGGLE's case apart from default, with a copy of
     * its body, where one case for both would do */
    case SWITCH_TOGGLE:
        switch_toggle_step(task);
        break;
    case SWITCH_PUSH:
        switch_push_step(task);
        break;
    case SWITCH_TIMED:
        switch_timed_step(task);
        break;
    case SWITCH_ON_ONLY:
        switch_on_only_step(task);
        break;
    default:
        switch_toggle_step(task);
        break;
    }
    switch_fade(task);
}

static void switch_draw(struct task *task)
{
    struct task_work *work = task->work;
    Float light;
    Uint32 color;

    njSetTexture(&switch_texlist);
    njPushMatrix(NULL);
    njTranslateV(NULL, &work->pos);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, work->ang.y);
    njRotateZ(NULL, work->ang.z);
    njCnkEasyDrawModel(&switch_base_model);
    draw_control_save();
    draw_attr_save();
    draw_control_clear(DRAW_CONTROL_MATERIAL_EDITS);
    draw_control_set(NJD_CONTROL_3D_CONSTANT_MATERIAL);
    draw_control_set(NJD_CONTROL_3D_CNK_CONSTANT_ATTR);
    light = STATE(task)->glow * (1.0F / GLOW_SCALE);
    if (STATE(task)->kind == SWITCH_TIMED) {
        light = blink_level(FLICKER_PERIOD, FLICKER_REST, FLICKER_FADE) * light;
    }
    if (light > LIGHT_UNSHADED) {
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_IL | NJD_FST_IS);
        njFogDisable();
    } else {
        njSetConstantAttr(CHUNK_TYPE_BITS, NJD_FST_IS);
    }
    color = color_blend(switch_pad_unlit_color, switch_lit_color,
                        STATE(task)->glow * (1.0F / GLOW_SCALE));
    constant_material_set(COLOR_PART(color, 24), COLOR_PART(color, 16),
                          COLOR_PART(color, 8), COLOR_PART(color, 0));
    njCnkSimpleDrawModel(&switch_pad_model);
    njTranslate(NULL, 0.0F, STATE(task)->cap.y, 0.0F);
    njRotateY(NULL, -work->ang.y);
    njRotateX(NULL, STATE(task)->tilt_x);
    njRotateZ(NULL, STATE(task)->tilt_z);
    njScale(NULL,
            1.0F + (njSin((Angle)(game_state.steps * ANGLE_PER_DEGREE) *
                          WOBBLE_XZ_RATE) *
                    WOBBLE_XZ),
            1.0F + (njCos((Angle)(game_state.steps * ANGLE_PER_DEGREE) *
                          WOBBLE_Y_RATE) *
                    WOBBLE_Y),
            1.0F + (njSin((Angle)((game_state.steps + WOBBLE_Z_LAG) *
                                  ANGLE_PER_DEGREE) *
                          WOBBLE_XZ_RATE) *
                    WOBBLE_XZ));
    /* @hack redundant: an empty else */
    if (switch_own_id_is_on(task)) {
        njRotateY(NULL,
                  (Angle)(game_state.steps * ANGLE_PER_DEGREE) * SPIN_RATE);
    } else {
    }
    color = color_blend(switch_cap_unlit_color, switch_lit_color, light);
    constant_material_set(COLOR_PART(color, 24), COLOR_PART(color, 16),
                          COLOR_PART(color, 8), COLOR_PART(color, 0));
    njCnkSimpleDrawModel(&switch_cap_model);
    if (light > LIGHT_UNSHADED) {
        njFogEnable();
    }
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    draw_attr_restore();
    draw_control_restore();
    njPopMatrix(1);
}

static void switch_destroy(struct task *task)
{
    /* @hack spelling: copy, a local for task, where task read in place would
     * do */
    struct task *copy = task;

    /* @hack redundant: a do loop that runs once */
    do {
        syFree(copy->extra.p);
        copy->extra.p = NULL;
    } while (0);
}

static void switch_toggle_step(struct task *task)
{
    if (switch_pressed(task)) {
        if (STATE(task)->was_pressed == 0) {
            switch_turn(task, 1 - switch_own_id_is_on(task));
        }
        STATE(task)->was_pressed = 1;
    } else {
        STATE(task)->was_pressed = 0;
    }
    switch_light(task, switch_own_id_is_on(task));
}

static void switch_push_step(struct task *task)
{
    if (switch_pressed(task)) {
        switch_turn(task, TRUE);
        switch_light(task, TRUE);
    } else {
        switch_turn(task, FALSE);
        switch_light(task, FALSE);
    }
}

static void switch_timed_step(struct task *task)
{
    struct task_work *work = task->work;

    if (switch_was_written(STATE(task)->id)) {
        stage_switch_flags[STATE(task)->id] &= ~SWITCH_WRITTEN;
        /* @hack spelling: the result compared with 1, where its truth would
         * do */
        if (switch_own_id_is_on(task) == 1) {
            switch_light(task, TRUE);
            switch_start_timer(work);
            switch_start_blink(task);
        } else {
            switch_light(task, FALSE);
            STATE(task)->blink = 0;
        }
    }
    if (switch_pressed(task)) {
        switch_turn(task, TRUE);
        switch_light(task, TRUE);
        switch_start_timer(work);
        switch_start_blink(task);
        /* @hack spelling: the result compared with 1, where its truth would
         * do */
    } else if (switch_own_id_is_on(task) == 1) {
        if (STATE(task)->blink != 0) {
            STATE(task)->blink--;
        } else {
            switch_light(task, 1 - LIT(work));
            switch_start_blink(task);
        }
        if (TIME_LEFT(work) > 0) {
            TIME_LEFT(work)--;
            if (TIME_LEFT(work) == 0) {
                switch_turn(task, FALSE);
                switch_light(task, FALSE);
            }
        }
    } else {
        switch_light(task, FALSE);
    }
}

static void switch_start_timer(struct task_work *work)
{
    if (work->scl.z > TIME_MIN && work->scl.z < TIME_MAX) {
        TIME_LEFT(work) = (int)work->scl.z;
    } else {
        TIME_LEFT(work) = TIME_MIN;
    }
}

static void switch_start_blink(struct task *task)
{
    struct task_work *work = task->work;
    int slack = TIME_LEFT(work) - BLINK_FAST;

    if (slack > 0) {
        if (LIT(work)) {
            STATE(task)->blink = (int)(slack * BLINK_LIT) + 1;
        } else {
            STATE(task)->blink = (int)(slack * BLINK_DARK) + 1;
        }
    } else {
        STATE(task)->blink = 1;
    }
}

static void switch_on_only_step(struct task *task)
{
    if (switch_pressed(task)) {
        switch_turn(task, TRUE);
    }
    switch_light(task, switch_own_id_is_on(task));
}

static void switch_light(struct task *task, Bool on)
{
    struct task_work *work = task->work;

    if (on) {
        LIT(work) = 1;
        STATE(task)->glow = GLOW_FULL;
    } else {
        LIT(work) = 0;
    }
}

static void switch_fade(struct task *task)
{
    if (!LIT(task->work)) {
        if (STATE(task)->glow >= GLOW_FADE_LEAST) {
            STATE(task)->glow -= GLOW_FADE;
        } else {
            STATE(task)->glow = GLOW_UNLIT;
        }
    }
}

/* turn the switch's id on, or off where on is FALSE. a change of the id
 * plays a sound */
static void switch_turn(struct task *task, Bool on)
{
    if (on) {
        if (!switch_is_on(STATE(task)->id)) {
            sound_play(ON_SOUND, NULL, 0, 0);
        }
        switch_set(STATE(task)->id, TRUE);
    } else {
        if (switch_is_on(STATE(task)->id)) {
            sound_play(OFF_SOUND, NULL, 0, 0);
        }
        switch_set(STATE(task)->id, FALSE);
    }
}

static Bool switch_own_id_is_on(struct task *task)
{
    /* @hack redundant: a call of the function itself that no path
     * reaches */
    if (0) {
        switch_own_id_is_on(task);
    }
    return switch_is_on(STATE(task)->id);
}

void switch_set(Uint32 id, Bool on)
{
    if (on) {
        stage_switch_flags[id] |= SWITCH_ON;
    } else {
        stage_switch_flags[id] &= ~SWITCH_ON;
    }
    stage_switch_flags[id] |= SWITCH_WRITTEN;
}

Bool switch_is_on(Uint32 id)
{
    /* @hack redundant: a do loop that runs once */
    do {
        if (stage_switch_flags[id] & SWITCH_ON) {
            return TRUE;
        }
    } while (0);
    return FALSE;
}

static Bool switch_was_written(Uint32 id)
{
    /* @hack redundant: a do loop that runs once */
    do {
        if (stage_switch_flags[id] & SWITCH_WRITTEN) {
            return TRUE;
        }
    } while (0);
    return FALSE;
}

void switch_clear_all(void)
{
    int i;

    for (i = 0; i < SWITCH_IDS; i++) {
        stage_switch_flags[i] = 0;
    }
}

void sss_init(struct task *task)
{
    struct task_work *work = task->work;
    /* @hack spelling: motion, a local for task->motion, where task->motion
     * read in the call would do */
    struct task_motion *motion = task->motion.p;

    collision_init(task, sss_shapes, SSS_SHAPES, COLLISION_OBJECT);
    fall_set_kind(work, motion, FALL_KIND_11);
    switch_presser_add(&work->pos);
    task->destructor = sss_destroy;
    task->step = sss_step;
    task->draw = sss_draw;
}

static void sss_step(struct task *task)
{
    /* @hack spelling: copy, a local for task, where task read in place would
     * do */
    struct task *copy = task;

    if (!placement_try_leave(copy)) {
        fall_step_medium(copy);
    }
}

static void sss_destroy(struct task *task)
{
    /* @hack spelling: work, a local for task->work, where task->work read in
     * place would do */
    struct task_work *work = task->work;

    switch_presser_remove(&work->pos);
}

static void sss_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    struct angles ang;
    NJS_VECTOR scl;

    pos.x = work->pos.x;
    pos.y = work->pos.y + (SSS_HEIGHT / 2);
    pos.z = work->pos.z;
    ang.x = 0;
    ang.y = work->ang.y;
    ang.z = 0;
    scl.x = SSS_RADIUS;
    scl.y = SSS_HEIGHT;
    scl.z = SSS_RADIUS;
    debug_draw_model(&pos, &ang, &scl, DEBUG_MODEL_CYLINDER);
}
