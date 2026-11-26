/* the save point, SAVEPOINT in a stage's object list: a gate between two
 * lamps. the first player to pass the gate restarts there, with the clock at
 * the time of the pass */
#include <ninja.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "collision/collision.h"
#include "draw/screen.h"
#include "draw/texture_anim.h"
#include "hud/hud.h"
#include "object/set.h"
#include "player/player.h"
#include "shared/item_box.h"
#include "shared/messenger.h"
#include "shared/object_builds.h"
#include "shared/save_point.h"
#include "sound/sound.h"
#include "stage/stage.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/snapshot.h"
#include "system/task.h"

#include "shared/save_point/text.inc"

/* a save point's modes, in its work's mode. nothing in this source sets
 * MODE_HOLD or MODE_KILL */
enum save_point_mode {
    MODE_INIT,
    MODE_ARMED, /* waiting for a player's touch */
    MODE_SPIN, /* the lamps spin down after a touch */
    MODE_PASSED, /* the lamps stand still */
    MODE_HOLD, /* the save point only draws */
    MODE_KILL /* the step calls placement_kill */
};

/* the bits of ang.y that hold the gate's heading. its low byte holds the
 * gate's order, and its bits from 16 up count for nothing */
#define HEADING_BITS 0xff00
#define HEADING(work) ((work)->ang.y & HEADING_BITS)
/* the order a pass gives restart_point_keep. the first step sets it to the
 * gate's order, or to 0 where the order doesn't count */
#define ORDER(work) ((work)->tick)

/* a pass shows its time for TIME_STEPS steps. for its first 12 spans of
 * BLINK_STEPS steps the readout blinks, hidden in each odd span */
#define TIME_STEPS 300
#define BLINK_STEPS 10
/* the minutes draw READOUT_RIGHT pixels left of the current camera's right
 * edge, and READOUT_Y pixels down. the draw takes each camera's part of the
 * screen as an equal width, side by side */
#define READOUT_RIGHT 135.0F
#define READOUT_Y 430.0F
/* the readout holds minutes to MINUTE_MAX and seconds to SECOND_MAX, two
 * digits each */
#define MINUTE_MAX 99
#define SECOND_MAX 60

/* a pass sets the lamps' spin, ang_spd.y, to SPIN_PER_SPEED times the
 * player's speed through the gate. each step keeps SPIN_KEEP of the spin */
#define SPIN_PER_SPEED (10 * ANGLE_PER_DEGREE)
#define SPIN_KEEP 0.995F
/* once spin, the size of ang_spd.y, is below SLOW_SPIN, the lamps tilt back
 * to an ang.x of 0. each step then sets ang_spd.x to
 * -(SLOW_SPIN - spin) * distance * TILT_PULL. distance is ang.x up to an
 * eighth turn, and TILT_FOLD less ang.x above it. TILT_FOLD lies past
 * QUARTER_TURN, the lamps' first ang.x, so the first tilt isn't 0 */
#define SLOW_SPIN ((int)(20 * ANGLE_PER_DEGREE))
#define TILT_PULL (0.8F / FULL_TURN)
#define TILT_FOLD (QUARTER_TURN + (int)(11 * ANGLE_PER_DEGREE))

/* a pass gives the player an item by its rings. it gives a barrier from
 * BARRIER_AT rings, and a speed up from SPEED_UP_AT. below that, it gives the
 * ring items from RINGS_20_AT, RINGS_10_AT and RINGS_5_AT */
#define BARRIER_AT 90
#define SPEED_UP_AT 80
#define RINGS_20_AT 60
#define RINGS_10_AT 40
#define RINGS_5_AT 20

#define SAVE_SOUND 0x1001
#define SAVE_POINT_SHAPES 1
#define LAMPS 2

/* task's record, whose address the first word of task's extra holds */
#define SAVE_POINT(task) (*(struct save_point **)(task)->extra.p)

/* a save point's record, which its destructor frees */
struct save_point {
    Sint32 unk_00;
    /* the lamps' turn in the gate's axes, with ang.y negated for lamp 0.
     * ang.z stays 0 */
    struct angles ang;
    /* what each step of MODE_SPIN adds to ang.x and ang.y */
    struct angles ang_spd;
    /* the lamps' tasks, children of the save point's. lamp 0 stands toward
     * -x, and lamp 1 toward +x. each task's work holds its lamp's place and
     * angles, which nothing in this source reads */
    struct task *lamps[LAMPS];
    /* the frame of the lamps' texture animations: 0 until a step in
     * MODE_SPIN or MODE_PASSED makes it 1 */
    Float frame;
    /* by player: the steps left to show the pass's time */
    Sint32 timer[GAME_PLAYERS];
    Sint16 unk_30[2];
    /* by player: the clock's minute and second at the player's pass */
    Sint16 minute[GAME_PLAYERS];
    Sint16 second[GAME_PLAYERS];
};

/* the record of the save point that last stepped or drew. the step's helpers
 * read it in place of their task's */
static struct save_point *save_point_current;

#define SAVE_POINT_TEXTURES 1
static NJS_TEXNAME save_point_texname[SAVE_POINT_TEXTURES] = {
#include "shared/save_point/save_point_texname.inc"
};
NJS_TEXLIST save_point_texlist = {
#include "shared/save_point/save_point_texlist.inc"
};
/* each lamp's model: a chunk model of LAMP_POINTS points, each with its
 * normal */
#define LAMP_POINTS 44
static Sint16 save_point_lamp0_polygons[395] = {
#include "shared/save_point/save_point_lamp0_polygons.inc"
};
static Sint32 save_point_lamp0_vertices[267] = {
#include "shared/save_point/save_point_lamp0_vertices.inc"
};
static NJS_CNK_MODEL save_point_lamp0_model = {
#include "shared/save_point/save_point_lamp0_model.inc"
};
/* lamp 0's node. the draw reads only its pos, the lamp's place in the
 * gate's axes */
/* @hack type: an array of one object, where a single object would do */
static NJS_CNK_OBJECT save_point_lamp0_object[1] = {
#include "shared/save_point/save_point_lamp0_object.inc"
};
static Sint16 save_point_lamp1_polygons[395] = {
#include "shared/save_point/save_point_lamp1_polygons.inc"
};
static Sint32 save_point_lamp1_vertices[267] = {
#include "shared/save_point/save_point_lamp1_vertices.inc"
};
static NJS_CNK_MODEL save_point_lamp1_model = {
#include "shared/save_point/save_point_lamp1_model.inc"
};
/* lamp 1's node. the draw reads only its pos, the lamp's place in the
 * gate's axes */
/* @hack type: an array of one object, where a single object would do */
static NJS_CNK_OBJECT save_point_lamp1_object[1] = {
#include "shared/save_point/save_point_lamp1_object.inc"
};
/* the gate's model: a chunk model of GATE_POINTS points, each with its
 * normal */
#define GATE_POINTS 82
static Sint16 save_point_polygons[845] = {
#include "shared/save_point/save_point_polygons.inc"
};
static Sint32 save_point_vertices[495] = {
#include "shared/save_point/save_point_vertices.inc"
};
static NJS_CNK_MODEL save_point_model = {
#include "shared/save_point/save_point_model.inc"
};
/* the save point's tree: the gate, with lamp 1 and lamp 0 as its children.
 * the draw takes each model by itself */
static NJS_CNK_OBJECT save_point_object = {
#include "shared/save_point/save_point_object.inc"
};
static Sint16 save_point_lamp1_uvs[308] = {
#include "shared/save_point/save_point_lamp1_uvs.inc"
};
static struct texture_anim save_point_lamp1_anim = {
#include "shared/save_point/save_point_lamp1_anim.inc"
};
static Sint16 save_point_lamp0_uvs[308] = {
#include "shared/save_point/save_point_lamp0_uvs.inc"
};
static struct texture_anim save_point_lamp0_anim = {
#include "shared/save_point/save_point_lamp0_anim.inc"
};
/* the save point's collision object. its model has COLLISION_POINTS points,
 * each with its normal, in one NJD_MESHSET_TRIMESH meshset. SHC refuses
 * braces around the material's colors, with error 2129 */
#define COLLISION_POINTS 18
/* NOLINTBEGIN(clang-diagnostic-missing-braces) */
static NJS_MATERIAL save_point_collision_material = {
#include "shared/save_point/save_point_collision_material.inc"
};
/* NOLINTEND(clang-diagnostic-missing-braces) */
struct save_point_collision_strips {
    Uint16 mesh[11];
    Uint16 mesh_2[6];
    Uint16 mesh_3[4];
    Uint16 mesh_4[4];
    Uint16 mesh_5[6];
    Uint16 mesh_6[11];
};
static struct save_point_collision_strips save_point_collision_meshes = {
#include "shared/save_point/save_point_collision_meshes.inc"
};
static NJS_MESHSET save_point_collision_meshset = {
#include "shared/save_point/save_point_collision_meshset.inc"
};
static NJS_POINT3 save_point_collision_points[COLLISION_POINTS] = {
#include "shared/save_point/save_point_collision_points.inc"
};
static NJS_VECTOR save_point_collision_normals[COLLISION_POINTS] = {
#include "shared/save_point/save_point_collision_normals.inc"
};
static NJS_MODEL save_point_collision_model = {
#include "shared/save_point/save_point_collision_model.inc"
};
static NJS_OBJECT save_point_collision = {
#include "shared/save_point/save_point_collision.inc"
};

/* the gate's shape, a box 24 across, 8 high and 1 deep */
/* clang-format off */
static struct collision_shape save_point_shapes[SAVE_POINT_SHAPES] = {
    {0, SHAPE_BOX, PUSH_FIRM(PUSH_PLAYERS | PUSH_ENEMIES | PUSH_PICKUPS),
     SHAPE_ATTACK_LEVEL(0) | SHAPE_HURTS_ALL, 0, {0.0F, 6.0F, 1.0F}, 12.0F,
     4.0F, 0.5F, 0, {0, 0, 0}},
};
/* clang-format on */
static Float readout_scale = 0.7F;
/* the readout's gaps in pixels: from the minutes to the colon, and from the
 * colon to the seconds */
static Float readout_colon_gap = 32.0F;
static Float readout_second_gap = 32.0F;
static Uint32 readout_color = 0xffffffff;
/* each lamp task's place, from the gate's pos, in the gate's axes */
static NJS_VECTOR lamp0_offset = {-15.0F, 9.8F, 0.0F};
static NJS_VECTOR lamp1_offset = {15.0F, 9.8F, 0.0F};

static void save_point_destroy(struct task *task);
static void save_point_draw(struct task *task);
static struct save_point *save_point_alloc(void);
static void save_point_lamp_step(struct task *task);
static void save_point_place_lamps(struct task_work *work);
static void save_point_collide(struct task *task, struct task_work *work);
static void save_point_spin(struct task *task, struct task_work *work);
static void save_point_count_down(struct task *task, struct task_work *work);
static Float save_point_pass_speed(struct task *task, struct task_work *work,
                                   int player);

void save_point_step(struct task *task)
{
    struct task_work *work = task->work;
    int character;

    if (placement_try_leave(task)) {
        return;
    }
    save_point_current = SAVE_POINT(task);
    /* @hack redundant: a block no path reaches, for its string in the
     * object's constants and its store of 0 to tick */
    if (0) {
        (void)SAVE_POINT_ID_LABEL;
        work->tick = 0;
    }
    switch (work->mode) {
    case MODE_INIT:
        /* @hack spelling: the test inverted, with a goto past the call, where
         * the call inside the if would do */
        if (game_state.mission != MISSION_TIMED) {
            goto init;
        }
        placement_kill(task);
        return;
    init:
        work->mode = MODE_ARMED;
        SAVE_POINT(task) = save_point_alloc();
#ifdef NONMATCHING
        /* where save_point_alloc gave no record, this side stays in
         * MODE_INIT, to try again on the next step */
        if (SAVE_POINT(task) == NULL) {
            work->mode = MODE_INIT;
            return;
        }
#endif
        save_point_current = SAVE_POINT(task);
        task->destructor = save_point_destroy;
        task->draw = save_point_draw;
        save_point_current->lamps[0] =
            task_create_child(TASK_WORK, save_point_lamp_step, task);
        save_point_current->lamps[1] =
            task_create_child(TASK_WORK, save_point_lamp_step, task);
        collision_init(task, save_point_shapes, SAVE_POINT_SHAPES,
                       COLLISION_OBJECT);
        save_point_place_lamps(work);
        save_point_current->ang.x = QUARTER_TURN;
        save_point_current->ang.y = -QUARTER_TURN;
        character = stage_character(0);
        if ((character == PLAYER_SONIC || character == PLAYER_SHADOW) &&
            game_state.two_players) {
            ORDER(work) = work->ang.y;
        } else {
            ORDER(work) = 0;
        }
        if (task->placement != NULL) {
            placement_keep(task);
            if (placement_touched(task)) {
                save_point_current->ang.x = 0;
                work->mode = MODE_PASSED;
            }
        }
        break;
    case MODE_ARMED:
        save_point_collide(task, work);
        break;
    case MODE_SPIN:
        save_point_collide(task, work);
        save_point_spin(task, work);
        save_point_current->frame = 1.0F;
        save_point_count_down(task, work);
        break;
    case MODE_PASSED:
        save_point_current->frame = 1.0F;
        save_point_collide(task, work);
        save_point_count_down(task, work);
        break;
    case MODE_HOLD:
        break;
    case MODE_KILL:
    default:
        placement_kill(task);
        break;
    }
}

/* draw minute and second as the readout, with the minutes at pos. the call
 * leaves the fog enabled, whatever its state before */
static void save_point_draw_time(int minute, int second, NJS_POINT3 *pos)
{
    struct hud_number number;

    number.flags = HUD_NUMBER_SCALED | HUD_NUMBER_CLAMPED | HUD_NUMBER_ZEROS;
    number.place = 0;
    number.unk_02 = 0;
    number.max = MINUTE_MAX;
    number.value = minute;
    number.pos.x = pos->x;
    number.pos.y = pos->y;
    number.pos.z = 0.0F;
    number.scale = readout_scale;
    number.color = readout_color;
    njFogDisable();
    hud_number_draw(&number);
    number.pos.x += readout_colon_gap;
    number.max = 0;
    number.value = 0;
    number.flags = HUD_NUMBER_SCALED | HUD_NUMBER_COLONS;
    hud_number_draw(&number);
    number.pos.x += readout_second_gap;
    number.flags = HUD_NUMBER_SCALED | HUD_NUMBER_CLAMPED | HUD_NUMBER_ZEROS;
    number.max = SECOND_MAX;
    number.value = second;
    hud_number_draw(&number);
    njFogEnable();
}

static void save_point_destroy(struct task *task)
{
    if (SAVE_POINT(task) != NULL) {
#ifdef NONMATCHING
        /* the next gate to draw would read the freed record, unless a
         * gate's step replaced it first */
        if (save_point_current == SAVE_POINT(task)) {
            save_point_current = NULL;
        }
#endif
        syFree(SAVE_POINT(task));
        SAVE_POINT(task) = NULL;
    }
}

static void save_point_draw(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 pos;
    /* @hack spelling: rotate_x, rotate_y and translate, locals that hold
     * njRotateX, njRotateY and njTranslateV, where each called by name would
     * do */
    void (*rotate_x)(NJS_MATRIX *matrix, Angle ang);
    void (*rotate_y)(NJS_MATRIX *matrix, Angle ang);
    void (*translate)(NJS_MATRIX *matrix, NJS_VECTOR *vec);

    /* @bug logic: draws the readout from the record of the save point that
     * last stepped or drew, which can be another gate's. the draw reads
     * save_point_current before it stores this gate's record there, at 0x2f6
     * and 0x3b2 of the object's code */
    if (save_point_current != NULL &&
        (work->mode == MODE_SPIN || work->mode == MODE_PASSED) &&
        save_point_current->timer[camera_current_index] > 0 &&
        camera_active_count > 0) {
        pos.x = (SCREEN_WIDTH / camera_active_count *
                 (camera_current_index + 1.0F)) -
                READOUT_RIGHT;
        pos.y = READOUT_Y;
        pos.z = 0.0F;
        switch ((TIME_STEPS - save_point_current->timer[camera_current_index]) /
                BLINK_STEPS) {
        case 1:
        case 3:
        case 5:
        case 7:
        case 9:
        case 11:
            break;
        default:
            save_point_draw_time(
                save_point_current->minute[camera_current_index],
                save_point_current->second[camera_current_index], &pos);
            break;
        }
    }
    save_point_current = SAVE_POINT(task);
    njPushMatrix(NULL);
    translate = njTranslateV;
    translate(NULL, &work->pos);
    njRotateZ(NULL, work->ang.z);
    rotate_x = njRotateX;
    rotate_x(NULL, work->ang.x);
    rotate_y = njRotateY;
    rotate_y(NULL, HEADING(work));
    njSetTexture(&save_point_texlist);
    njCnkEasyDrawModel(&save_point_model);
    if (objects_fog_off) {
        njFogDisable();
    }
    njPushMatrix(NULL);
    translate(NULL, (NJS_VECTOR *)save_point_lamp1_object->pos);
    rotate_y(NULL, save_point_current->ang.y);
    rotate_x(NULL, save_point_current->ang.x);
    texture_anim_blend(&save_point_lamp1_model, &save_point_lamp1_anim,
                       save_point_current->frame);
    njCnkEasyDrawModel(&save_point_lamp1_model);
    njPopMatrix(1);
    translate(NULL, (NJS_VECTOR *)save_point_lamp0_object->pos);
    rotate_y(NULL, -save_point_current->ang.y);
    rotate_x(NULL, save_point_current->ang.x);
    texture_anim_blend(&save_point_lamp0_model, &save_point_lamp0_anim,
                       save_point_current->frame);
    njCnkEasyDrawModel(&save_point_lamp0_model);
    if (objects_fog_off) {
        njFogEnable();
    }
    njPopMatrix(1);
    /* @hack redundant: a store to the work's mode that no path reaches */
    if (0) {
        work->mode = MODE_INIT;
    }
}

static struct save_point *save_point_alloc(void)
{
    struct save_point *record;
    /* @hack spelling: byte, a local for the record's bytes, where record
     * cast in place would do */
    char *byte;
    int i;

    /* @hack redundant: a test of syMalloc's result that does nothing */
    if ((record = syMalloc(sizeof(struct save_point))) == NULL) {
    }
    /* @bug memory: zeroes a record's bytes from address 0 where syMalloc
     * fails. the loop runs whatever the test of syMalloc's result finds (TST
     * R0,R0 at 0x49e, MOV.B R5,@R4 at 0x4a6 of the object's code) */
#ifdef NONMATCHING
    /* this side repairs the tag above: it returns the null, and
     * save_point_step waits in MODE_INIT for a record */
    if (record == NULL) {
        return NULL;
    }
#endif
    byte = (char *)record;
    for (i = 0; i < (int)sizeof(struct save_point); i++) {
        byte[i] = 0;
    }
    return record;
}

static void save_point_lamp_step(struct task *task)
{
}

static void save_point_place_lamps(struct task_work *work)
{
    /* @hack spelling: the two offsets in one array, where a local for each
     * would do */
    NJS_VECTOR offset[LAMPS];

    njPushMatrixEx();
    njUnitMatrix(NULL);
    njRotateZ(NULL, work->ang.z);
    njRotateX(NULL, work->ang.x);
    njRotateY(NULL, HEADING(work));
    njCalcVector(NULL, &lamp0_offset, &offset[0]);
    njCalcVector(NULL, &lamp1_offset, &offset[1]);
    njPopMatrixEx();
    save_point_current->lamps[0]->work->pos.x += offset[0].x;
    save_point_current->lamps[0]->work->pos.y += offset[0].y;
    save_point_current->lamps[0]->work->pos.z += offset[0].z;
    save_point_current->lamps[1]->work->pos.x += offset[1].x;
    save_point_current->lamps[1]->work->pos.y += offset[1].y;
    save_point_current->lamps[1]->work->pos.z += offset[1].z;
}

static void save_point_collide(struct task *task, struct task_work *work)
{
    struct task *other;
    int player;
    int rings;
    Sint8 minute;
    Sint8 second;
    Sint8 frame;

    save_point_current->lamps[0]->work->ang.x =
        save_point_current->ang.x + work->ang.x;
    save_point_current->lamps[0]->work->ang.y =
        -save_point_current->ang.y + HEADING(work);
    save_point_current->lamps[0]->work->ang.z =
        save_point_current->ang.z + work->ang.z;
    save_point_current->lamps[1]->work->ang.x =
        save_point_current->ang.x + work->ang.x;
    save_point_current->lamps[1]->work->ang.y =
        save_point_current->ang.y + HEADING(work);
    save_point_current->lamps[1]->work->ang.z =
        save_point_current->ang.z + work->ang.z;
    if (work->mode == MODE_ARMED) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((other = collision_next_player(task)) != NULL &&
            player_slot_of(other) != -1 && other->parent == NULL) {
            /* @hack redundant: player_slot_of called again for the slot its
             * test found, where a local set in the test would do */
            player = player_slot_of(other);
            work->mode = MODE_SPIN;
            save_point_current->ang_spd.y =
                save_point_pass_speed(task, work, player) * SPIN_PER_SPEED;
            save_point_current->timer[player] = TIME_STEPS;
            restart_point_keep(player, &player_work[player]->pos,
                               &player_work[player]->ang, ORDER(work));
            /* @bug memory: placement_set_touched writes through null where
             * the task has no placement. the first step tests
             * task->placement, but no test guards this call (0xea and 0x720
             * of the object's code) */
            placement_set_touched(task);
            sound_play(SAVE_SOUND, NULL, 0, 0);
            stage_clock_get(&minute, &second, &frame);
            save_point_current->minute[player] = minute;
            save_point_current->second[player] = second;
            messenger_set_event(player, MESSENGER_EVENT_SAVE_POINT);
            rings = ring_count(player);
            if (rings >= BARRIER_AT) {
                if (player_records[player]->flags & PLAYER_BARRIER ||
                    player_records[player]->flags & PLAYER_MAGNETIC_BARRIER) {
                    item_give(player, ITEM_MAGNETIC_BARRIER);
                } else {
                    item_give(player, ITEM_BARRIER);
                }
            } else if (rings >= SPEED_UP_AT) {
                item_give(player, ITEM_SPEED_UP);
            } else if (rings >= RINGS_20_AT) {
                item_give(player, ITEM_20_RINGS);
                game_state.ring_total += 20;
                snapshot_current->ring_total += 20;
            } else if (rings >= RINGS_10_AT) {
                item_give(player, ITEM_10_RINGS);
                game_state.ring_total += 10;
                snapshot_current->ring_total += 10;
            } else if (rings >= RINGS_5_AT) {
                item_give(player, ITEM_5_RINGS);
                game_state.ring_total += 5;
                snapshot_current->ring_total += 5;
            }
        }
    }
    collision_register(task);
}

/* @hack redundant: the parameter task, which the function doesn't read */
static void save_point_spin(struct task *task, struct task_work *work)
{
    /* @hack spelling: spd declared ahead of ang, not in the order of their
     * first use */
    /* @hack redundant: a third element in spd and in ang, which nothing
     * uses */
    Sint16 spd[3];
    Sint16 ang[3];
    NJS_VECTOR unused_spd;
    NJS_VECTOR unused_pos;
    /* @hack spelling: spin reused for the tilt's new speed, where a local of
     * its own would do */
    Sint16 spin;
    Sint16 distance;
    int i;

    ang[0] = save_point_current->ang.x;
    ang[1] = save_point_current->ang.y;
    spd[0] = save_point_current->ang_spd.x;
    spd[1] = save_point_current->ang_spd.y;
    ang[0] += spd[0];
    ang[1] += spd[1];
    /* @hack spelling: spd[1] times SPIN_KEEP stored back, where *= would
     * do */
    spd[1] = spd[1] * SPIN_KEEP;
    spin = njAbs(spd[1]);
    if (spin < SLOW_SPIN) {
        if (ang[0] > EIGHTH_TURN) {
            distance = TILT_FOLD - ang[0];
        } else {
            distance = ang[0];
        }
        /* @hack type: the product cut to an int before its negation, where
         * the negated product would do */
        /* @hack type: SLOW_SPIN less spin converted to a Float, where its int
         * product with distance would do */
        spin = -(int)((Float)(SLOW_SPIN - spin) * distance * TILT_PULL);
        spd[0] = spin;
    }
    save_point_current->ang.x = ang[0];
    save_point_current->ang.y = ang[1];
    save_point_current->ang_spd.x = spd[0];
    save_point_current->ang_spd.y = spd[1];
    if (spd[0] == 0 && spd[1] == 0) {
        work->mode = MODE_PASSED;
        return;
    }
    for (i = 0; i < 2; i++) {
        if (save_point_current->unk_30[i] < 3) {
            /* @hack redundant: the stores of unused_spd and unused_pos,
             * which nothing reads */
            unused_spd.x = 0.0F;
            unused_spd.y = -0.3F;
            unused_spd.z = 0.0F;
            unused_pos = player_work[0]->pos;
            unused_pos.y += 4.0F;
            save_point_current->unk_30[i]++;
        }
    }
}

/* @hack redundant: the parameters task and work, which the function doesn't
 * read */
static void save_point_count_down(struct task *task, struct task_work *work)
{
    int player;

    for (player = 0; player < GAME_PLAYERS; player++) {
        if (save_point_current->timer[player] > 0) {
            save_point_current->timer[player]--;
        }
    }
}

/* return player's speed, negated where player faces more than a quarter
 * turn away from the gate's heading */
/* @hack redundant: the parameter task, which the function doesn't read */
static Float save_point_pass_speed(struct task *task, struct task_work *work,
                                   int player)
{
    Float speed;
    Angle turn;

    speed = njScalor(&player_records[player]->spd);
    turn = angle_difference(QUARTER_TURN - player_work[player]->ang.y,
                            HEADING(work));
    /* @hack type: njAbs(turn) cut to an int, where comparing the Float itself
     * would do */
    if ((int)njAbs(turn) > QUARTER_TURN) {
        return -speed;
    }
    return speed;
}
