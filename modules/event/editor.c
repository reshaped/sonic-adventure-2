#include <mathf.h>
#include <ninja.h>
#include <nj_motion.h>
#include <sg_xpt.h>
#include <shinobi.h>

#include "camera/camera.h"
#include "camera/helpers.h"
#include "draw/lines.h"
#include "event/playback.h"
#include "event/editor.h"
#include "event/model_burst.h"
#include "event/movie.h"
#include "event/scene.h"
#include "event/scene_fog.h"
#include "event/screen_sprite.h"
#include "event/script.h"
#include "system/angle.h"
#include "system/flags.h"
#include "system/pad.h"
#include "system/sequence.h"

#define HELD(button) (player_pads[EVENT_EDIT_PAD]->on & PDD_DGT_##button)
#define PRESS(button) (player_pads[EVENT_EDIT_PAD]->press & PDD_DGT_##button)
#define REPEAT(button) (pad_press_repeat[EVENT_EDIT_PAD] & PDD_DGT_##button)
/* open_bursts puts the burst's launch point LAUNCH_AHEAD along the camera's
 * view */
#define LAUNCH_AHEAD 50.0F
/* the size of the mark at the burst's position */
#define MARK_SIZE 5.0F
/* the change a frame of up or down makes to the free camera's vy */
#define TILT_STEP 0.02F
/* the sprite editor's steps. a position steps by 1, or by FAST_STEP with A
 * held, and a scale by SCALE_STEP times that. a texture jumps by
 * TEXTURE_JUMP */
#define FAST_STEP 10.0F
#define TEXTURE_JUMP 10
#define SCALE_STEP 0.1F
/* the sprite editor prints its lines from (INFO_X, INFO_Y) down */
#define INFO_X 5
#define INFO_Y 20

/* the burst editor's fields, in the order it prints them */
enum burst_field {
    FIELD_POSITION_X,
    FIELD_POSITION_Y,
    FIELD_POSITION_Z,
    FIELD_ANGLE_X,
    FIELD_ANGLE_Y,
    FIELD_ANGLE_Z,
    FIELD_GRAVITY_X,
    FIELD_GRAVITY_Y,
    FIELD_GRAVITY_Z,
    FIELD_SPREAD,
    FIELD_SPEED,
    FIELD_SCALE,
    FIELD_COUNT,
    FIELD_MODEL,
    FIELD_TRAIL,
    FIELD_TRAIL_INTERVAL
};
#define LAST_FIELD FIELD_TRAIL_INTERVAL
/* X held makes a field's step COARSER times as large, and B FINER times */
#define COARSER 10
#define FINER 0.1F
/* the bits of a hex digit. X held steps a hex field two digits up, and Y one
 * digit up */
#define HEX_DIGIT_BITS 4
#define ACCELERATION_STEP 0.1F
#define SPREAD_STEP 0x10 /* in Angle units */

struct event_editor event_editor;
static NJS_CAMERA free_camera;
static void open_sprites(void);
static void step_sprite(void);
static void open_bursts(void);
static void step_burst(void);
static void move_launch(NJS_POINT3 *pos, Float speed, Bool stick, int port);

void event_editor_close(void)
{
    event_editor.mode = EDIT_OFF;
    event_free_camera = FALSE;
    if (game_flags[FLAG_SEQUENCE]) {
        event_debug_print = FALSE;
    }
}

void event_editor_step(void)
{
    struct event_model_burst burst;

    if (game_flags[FLAG_SEQUENCE]) {
        return;
    }
    switch (event_editor.mode) {
    case EDIT_SPRITES:
        step_sprite();
        if (HELD(TA) && PRESS(TX)) {
            event_editor_close();
        }
        break;
    case EDIT_BURSTS:
        step_burst();
        if (HELD(TA) && PRESS(TX)) {
            event_editor_close();
        } else if (PRESS(TA)) {
            burst.direction.x =
                event_script_work->degrees[0] * ANGLE_PER_DEGREE;
            burst.direction.y =
                event_script_work->degrees[1] * ANGLE_PER_DEGREE;
            /* @bug uninitialized: the third angle overwrites x at 0x8c5057be,
             * so the burst's pieces take z unset */
#ifdef NONMATCHING
            /* this side repairs the bug tagged above: the third angle goes to
             * z */
            burst.direction.z =
                event_script_work->degrees[2] * ANGLE_PER_DEGREE;
#else
            burst.direction.x =
                event_script_work->degrees[2] * ANGLE_PER_DEGREE;
#endif
            burst.position.x = event_script_work->position.x;
            burst.position.y = event_script_work->position.y;
            burst.position.z = event_script_work->position.z;
            burst.acceleration.x = event_script_work->acceleration.x;
            burst.acceleration.y = event_script_work->acceleration.y;
            burst.acceleration.z = event_script_work->acceleration.z;
            burst.spread = event_script_work->spread;
            burst.speed = event_script_work->speed;
            burst.scale = event_script_work->scale;
            burst.count = event_script_work->count;
            burst.model = event_script_work->model;
            burst.trail = event_script_work->trail;
            burst.trail_interval = event_script_work->trail_interval;
            if (burst.trail > 0) {
                event_model_burst_start(&burst);
            }
        }
        break;
    default:
        if (HELD(TX) && PRESS(TY)) {
            open_sprites();
        }
        if (HELD(TX) && PRESS(TB)) {
            open_bursts();
        }
        if (HELD(TA) && PRESS(TY)) {
            event_debug_print = !event_debug_print;
        }
        if ((player_pads[EVENT_PLAYER_PAD]->on & PDD_DGT_TY) &&
            (player_pads[EVENT_PLAYER_PAD]->press & PDD_DGT_TX)) {
            event_frame_step = EVENT_FRAME_STEP;
        }
        if (HELD(TY) && PRESS(TB)) {
            event_loop = !event_loop;
            if (event_debug_print) {
                njPrintC(NJM_LOCATION(10, 10), "CHANGE!!!!!!!");
            }
        }
        if (HELD(TA) && PRESS(TB)) {
            event_editing_fog = !event_editing_fog;
        }
        if (HELD(TX) && PRESS(TA)) {
            event_debug_print = !event_debug_print;
        }
        if (HELD(TB) && PRESS(TY)) {
            event_free_camera = !event_free_camera;
            if (event_free_camera) {
                free_camera = _nj_motion_camera_;
            }
        }
        break;
    }
}

static void open_sprites(void)
{
    event_editor.mode = EDIT_SPRITES;
    event_script_work->texture_count = event_scene_file->texlist->nbTexture;
    event_script_work->texture = 0;
    event_script_work->sprite_position.x = 0;
    event_script_work->sprite_position.y = 0;
    event_script_work->sprite_scale.x = 1.0F;
    event_script_work->sprite_scale.y = 1.0F;
}

static void step_sprite(void)
{
    Float step;

    if (HELD(TA)) {
        step = FAST_STEP;
    } else {
        step = 1.0F;
    }
    if (HELD(TB)) {
        if (REPEAT(KL)) {
            event_script_work->texture--;
        }
        if (REPEAT(KR)) {
            event_script_work->texture++;
        }
        if (REPEAT(KD)) {
            event_script_work->texture -= TEXTURE_JUMP;
        }
        if (REPEAT(KU)) {
            event_script_work->texture += TEXTURE_JUMP;
        }
        if (event_script_work->texture < 0) {
            event_script_work->texture =
                event_scene_file->texlist->nbTexture - 1;
        }
        if ((Uint32)event_script_work->texture >=
            event_scene_file->texlist->nbTexture) {
            event_script_work->texture = 0;
        }
    } else if (HELD(TX)) {
        if (REPEAT(KL)) {
            event_script_work->sprite_scale.x -= step * SCALE_STEP;
        }
        if (REPEAT(KR)) {
            event_script_work->sprite_scale.x += step * SCALE_STEP;
        }
        if (REPEAT(KU)) {
            event_script_work->sprite_scale.y -= step * SCALE_STEP;
        }
        if (REPEAT(KD)) {
            event_script_work->sprite_scale.y += step * SCALE_STEP;
        }
    } else {
        if (REPEAT(KL)) {
            event_script_work->sprite_position.x -= step;
        }
        if (REPEAT(KR)) {
            event_script_work->sprite_position.x += step;
        }
        if (REPEAT(KU)) {
            event_script_work->sprite_position.y -= step;
        }
        if (REPEAT(KD)) {
            event_script_work->sprite_position.y += step;
        }
    }
}

#define SET_BLEND(destination)                                                 \
    njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);        \
    njColorBlendingMode(NJD_DESTINATION_COLOR, (destination))

void event_editor_draw_sprite(void)
{
    NJS_TEXANIM anim;
    NJS_SPRITE sprite;
    anim.sx = event_scene_file->texture_sizes[event_script_work->texture].width;
    anim.sy =
        event_scene_file->texture_sizes[event_script_work->texture].height;
    anim.cx =
        event_scene_file->texture_sizes[event_script_work->texture].width / 2;
    anim.cy =
        event_scene_file->texture_sizes[event_script_work->texture].height / 2;
    anim.u1 = 0;
    anim.v1 = 0;
    anim.u2 = EVENT_UV_LAST;
    anim.v2 = EVENT_UV_LAST;
    anim.texid = event_script_work->texture;
    sprite.tlist = event_scene_file->texlist;
    sprite.tanim = &anim;
    sprite.ang = 0;
    sprite.sx = event_script_work->sprite_scale.x;
    sprite.sy = event_script_work->sprite_scale.y;
    sprite.p.x = event_script_work->sprite_position.x + EVENT_SCREEN_CENTER_X;
    sprite.p.y = event_script_work->sprite_position.y + EVENT_SCREEN_CENTER_Y;
    constant_material_set(0.0F, 0.0F, 0.0F, 0.0F);
    SET_BLEND(NJD_COLOR_BLENDING_INVSRCALPHA);
    if (HELD(ST)) {
        SET_BLEND(NJD_COLOR_BLENDING_ONE);
        njDrawSprite2D(&sprite, 0, EVENT_SPRITE_PRIORITY, NJD_SPRITE_ALPHA);
        SET_BLEND(NJD_COLOR_BLENDING_INVSRCALPHA);
    } else {
        njDrawSprite2D(&sprite, 0, EVENT_SPRITE_PRIORITY, 0);
    }
    njPrint(NJM_LOCATION(INFO_X, INFO_Y), "SPRITE NUMBER : %d",
            (int)event_script_work->texture);
    njPrint(NJM_LOCATION(INFO_X, INFO_Y + 1), "ORIGINAL SIZE : %03d X %03d",
            event_scene_file->texture_sizes[event_script_work->texture].width,
            event_scene_file->texture_sizes[event_script_work->texture].height);
    njPrint(NJM_LOCATION(INFO_X, INFO_Y + 2), "POSITION      : %3d , %3d",
            event_script_work->sprite_position.x,
            event_script_work->sprite_position.y);
    njPrint(NJM_LOCATION(INFO_X, INFO_Y + 3), "SCALE         : %2.1f X %2.1f",
            event_script_work->sprite_scale.x,
            event_script_work->sprite_scale.y);
    njPrint(
        NJM_LOCATION(INFO_X, INFO_Y + 4), "NAME          : %s",
        (char *)event_scene_file->texlist->textures[event_script_work->texture]
            .filename);
    njPrint(NJM_LOCATION(INFO_X, INFO_Y + 5), "2P START -> ALPHA DRAW");
}

static void open_bursts(void)
{
    int i;
    Uint8 *field;
    event_editor.mode = EDIT_BURSTS;
    event_editor.cursor = 0;
    field = event_script_work->unk_320;
    /* @hack redundant: field and a loop through it that runs no times */
    for (i = 0; i != 0; i--) {
        *field++ = 64;
    }
    event_script_work->position.x =
        _nj_motion_camera_.px + (_nj_motion_camera_.vx * LAUNCH_AHEAD);
    event_script_work->position.y =
        _nj_motion_camera_.py + (_nj_motion_camera_.vy * LAUNCH_AHEAD);
    event_script_work->position.z =
        _nj_motion_camera_.pz + (_nj_motion_camera_.vz * LAUNCH_AHEAD);
}

#define STEP_VALUE()                                                           \
    if (HELD(TX)) {                                                            \
        step *= COARSER;                                                       \
    }                                                                          \
    if (HELD(TB)) {                                                            \
        step *= FINER;                                                         \
    }                                                                          \
    if (REPEAT(KU)) {                                                          \
        *value += step;                                                        \
    }                                                                          \
    if (REPEAT(KD)) {                                                          \
        *value -= step;                                                        \
    }

static void step_float(Float *value, Float step)
{
    STEP_VALUE()
}
static void step_long(Sint32 *value, Sint32 step)
{
    STEP_VALUE()
}
static void step_short(Sint16 *value, Sint32 step)
{
    STEP_VALUE()
}

static void step_hex(Sint32 *value, Sint32 step)
{
    if (HELD(TX)) {
        step <<= 2 * HEX_DIGIT_BITS;
    }
    if (HELD(TY)) {
        step <<= HEX_DIGIT_BITS;
    }
    if (REPEAT(KU)) {
        *value += step;
    }
    if (REPEAT(KD)) {
        *value -= step;
    }
}

static void step_burst(void)
{
    if (HELD(ST)) {
        move_launch(&event_script_work->position, 1.0F, TRUE, EVENT_EDIT_PAD);
    } else if (!(player_pads[EVENT_EDIT_PAD]->on &
                 (PDD_DGT_TX | PDD_DGT_TY | PDD_DGT_TA | PDD_DGT_TB))) {
        if (REPEAT(KU)) {
            event_editor.cursor--;
            if (event_editor.cursor < 0) {
                event_editor.cursor = LAST_FIELD;
            }
        }
        if (REPEAT(KD)) {
            event_editor.cursor++;
            if (event_editor.cursor > LAST_FIELD) {
                event_editor.cursor = 0;
            }
        }
    } else {
        switch (event_editor.cursor) {
        case FIELD_POSITION_X:
            step_float(&event_script_work->position.x, 1.0F);
            break;
        case FIELD_POSITION_Y:
            step_float(&event_script_work->position.y, 1.0F);
            break;
        case FIELD_POSITION_Z:
            step_float(&event_script_work->position.z, 1.0F);
            break;
        case FIELD_ANGLE_X:
            step_short(&event_script_work->degrees[0], 1);
            break;
        case FIELD_ANGLE_Y:
            step_short(&event_script_work->degrees[1], 1);
            break;
        case FIELD_ANGLE_Z:
            step_short(&event_script_work->degrees[2], 1);
            break;
        case FIELD_GRAVITY_X:
            step_float(&event_script_work->acceleration.x, ACCELERATION_STEP);
            break;
        case FIELD_GRAVITY_Y:
            step_float(&event_script_work->acceleration.y, ACCELERATION_STEP);
            break;
        case FIELD_GRAVITY_Z:
            step_float(&event_script_work->acceleration.z, ACCELERATION_STEP);
            break;
        case FIELD_SPREAD:
            step_hex(&event_script_work->spread, SPREAD_STEP);
            event_script_work->spread &= ANGLE_MASK;
            break;
        case FIELD_SPEED:
            step_float(&event_script_work->speed, 1.0F);
            break;
        case FIELD_SCALE:
            step_float(&event_script_work->scale, 1.0F);
            break;
        case FIELD_COUNT:
            step_long(&event_script_work->count, 1);
            break;
        case FIELD_MODEL:
            step_long(&event_script_work->model, 1);
            break;
        case FIELD_TRAIL:
            step_long(&event_script_work->trail, 1);
            break;
        case FIELD_TRAIL_INTERVAL:
            step_long(&event_script_work->trail_interval, 1);
            break;
        }
    }
}

/* the burst editor prints field n at (FIELD_X, FIRST_FIELD_ROW + n). its
 * cursor stands at CURSOR_X on its field's row */
#define FIRST_FIELD_ROW 12
#define FIELD_X 5
#define CURSOR_X 3
#define PRINT_FIELD(field, fmt, value)                                         \
    njPrint(NJM_LOCATION(FIELD_X, FIRST_FIELD_ROW + (field)), fmt, value)
void event_editor_draw_burst(void)
{
    njPrint(NJM_LOCATION(CURSOR_X, event_editor.cursor + FIRST_FIELD_ROW), "*");
    PRINT_FIELD(FIELD_POSITION_X, "POSITION(X)    : %5.3f",
                event_script_work->position.x);
    PRINT_FIELD(FIELD_POSITION_Y, "POSITION(Y)    : %5.3f",
                event_script_work->position.y);
    PRINT_FIELD(FIELD_POSITION_Z, "POSITION(Z)    : %5.3f",
                event_script_work->position.z);
    PRINT_FIELD(FIELD_ANGLE_X, "LAUNCHANGLE(X) : %2d",
                event_script_work->degrees[0]);
    PRINT_FIELD(FIELD_ANGLE_Y, "LAUNCHANGLE(Y) : %2d",
                event_script_work->degrees[1]);
    PRINT_FIELD(FIELD_ANGLE_Z, "LAUNCHANGLE(Z) : %2d",
                event_script_work->degrees[2]);
    PRINT_FIELD(FIELD_GRAVITY_X, "GRAVITY(X)     : %3.2f",
                event_script_work->acceleration.x);
    PRINT_FIELD(FIELD_GRAVITY_Y, "GRAVITY(Y)     : %3.2f",
                event_script_work->acceleration.y);
    PRINT_FIELD(FIELD_GRAVITY_Z, "GRAVITY(Z)     : %3.2f",
                event_script_work->acceleration.z);
    PRINT_FIELD(FIELD_SPREAD, "DIFF           : 0x%04x",
                (unsigned)event_script_work->spread);
    PRINT_FIELD(FIELD_SPEED, "LAUNCH SPD     : %3.2f",
                event_script_work->speed);
    PRINT_FIELD(FIELD_SCALE, "SCALE          : %3.2f",
                event_script_work->scale);
    PRINT_FIELD(FIELD_COUNT, "NUM            : %2d",
                (int)event_script_work->count);
    PRINT_FIELD(FIELD_MODEL, "MDL NUMBER     : %2d",
                (int)event_script_work->model);
    PRINT_FIELD(FIELD_TRAIL, "EFFECT NO      : %2d",
                (int)event_script_work->trail);
    PRINT_FIELD(FIELD_TRAIL_INTERVAL, "EFFECT RATE    : %2d",
                (int)event_script_work->trail_interval);
    debug_draw_mark(&event_script_work->position, MARK_SIZE);
}

/* @bug logic: up with right reads right from the first controller, not
 * port's, at 0x8c5064dc and 0x8c5066f8. the second controller's up with right
 * then moves as up alone, unless the first holds right */
#ifdef NONMATCHING
/* this side repairs the bug tagged above: up with right reads port's own right.
 * it scales the stick by * 256, where << 8 of a negative axis is undefined C */
#define UP_RIGHT_PORT(port) (port)
#define EDIT_AXIS(axis) ((axis) * 256)
#else
#define UP_RIGHT_PORT(port) 0
#define EDIT_AXIS(axis) CAMERA_STICK_READING(axis)
#endif
/* move pos by speed the way player_pads[port] points, across the ground as
 * camera_view faces it. while stick is FALSE, the d-pad moves pos in eight
 * directions. otherwise the stick moves it the way it leans, read as the
 * camera reads a stick. L raises pos.y by speed, and R lowers it while L is
 * up */
/* @hack spelling: a macro that defines a mover for each camera, where one
 * function taking the camera would do */
#define DEFINE_MOVE(name, camera_view)                                         \
    static void name(NJS_POINT3 *pos, Float speed, Bool stick, int port)       \
    {                                                                          \
        Angle pitch;                                                           \
        Angle yaw;                                                             \
        Sint16 x;                                                              \
        Sint16 y;                                                              \
        /* @hack type: the camera's vx read as an NJS_VECTOR, where a copy of  \
         * its vx, vy and vz would do */                                       \
        look_angles((NJS_VECTOR *)&(camera_view).vx, &pitch, &yaw);            \
        if (!stick) {                                                          \
            if (player_pads[port]->on & PAD_DIRECTIONS) {                      \
                if (player_pads[port]->on & PDD_DGT_KU) {                      \
                    if (player_pads[port]->on & PDD_DGT_KL) {                  \
                        yaw += 5 * EIGHTH_TURN;                                \
                    } else if (player_pads[UP_RIGHT_PORT(port)]->on &          \
                               PDD_DGT_KR) {                                   \
                        yaw += 3 * EIGHTH_TURN;                                \
                    } else {                                                   \
                        yaw += HALF_TURN;                                      \
                    }                                                          \
                } else if (player_pads[port]->on & PDD_DGT_KD) {               \
                    if (player_pads[port]->on & PDD_DGT_KL) {                  \
                        yaw += 7 * EIGHTH_TURN;                                \
                    } else if (player_pads[port]->on & PDD_DGT_KR) {           \
                        yaw += EIGHTH_TURN;                                    \
                    } else {                                                   \
                        /* @hack redundant: yaw += 0 for down alone */         \
                        yaw += 0;                                              \
                    }                                                          \
                } else if (player_pads[port]->on & PDD_DGT_KL) {               \
                    yaw += 3 * QUARTER_TURN;                                   \
                } else if (player_pads[port]->on & PDD_DGT_KR) {               \
                    yaw += QUARTER_TURN;                                       \
                }                                                              \
                pos->z += speed * njCos(yaw);                                  \
                pos->x += speed * njSin(yaw);                                  \
            }                                                                  \
        } else {                                                               \
            x = EDIT_AXIS(player_pads[port]->x1);                              \
            y = EDIT_AXIS(player_pads[port]->y1);                              \
            if (x > -CAMERA_STICK_DEAD_ZONE && x < CAMERA_STICK_DEAD_ZONE) {   \
                x = 0;                                                         \
            }                                                                  \
            if (y > -CAMERA_STICK_DEAD_ZONE && y < CAMERA_STICK_DEAD_ZONE) {   \
                y = 0;                                                         \
            }                                                                  \
            if (x != 0 || y != 0) {                                            \
                yaw -=                                                         \
                    (Angle)(atan2f(y, x) * ANGLE_PER_RADIAN) - QUARTER_TURN;   \
                pos->z += speed * njCos(yaw);                                  \
                pos->x += speed * njSin(yaw);                                  \
            }                                                                  \
        }                                                                      \
        if (player_pads[port]->on & (PDD_DGT_TR | PDD_DGT_TL)) {               \
            if (player_pads[port]->on & PDD_DGT_TL) {                          \
                pos->y += speed;                                               \
            } else if (player_pads[port]->on & PDD_DGT_TR) {                   \
                pos->y -= speed;                                               \
            }                                                                  \
        }                                                                      \
    }

DEFINE_MOVE(move_launch, _nj_motion_camera_)
DEFINE_MOVE(move_camera, free_camera)

void event_editor_set_camera(void)
{
    Float radius;
    Float square;
    Angle yaw;
    if (event_debug_print) {
        njPrintC(NJM_LOCATION(1, 7), "Debug Camera Mode");
    }
    /* @hack type: the camera's vx read as an NJS_VECTOR, where a copy of its
     * vx, vy and vz would do */
    njUnitVector((NJS_VECTOR *)&free_camera.vx);
    /* @hack type: the camera's px read as an NJS_POINT3, where a copy of its
     * px, py and pz would do */
    move_camera((NJS_POINT3 *)&free_camera.px, 1.0F, TRUE, EVENT_EDIT_PAD);
    if (HELD(KU)) {
        free_camera.vy += TILT_STEP;
    }
    if (HELD(KD)) {
        free_camera.vy -= TILT_STEP;
    }
    /* @hack type: the camera's vx read as an NJS_VECTOR, where a copy of its
     * vx, vy and vz would do */
    njUnitVector((NJS_VECTOR *)&free_camera.vx);
    square =
        (free_camera.vx * free_camera.vx) + (free_camera.vz * free_camera.vz);
    radius = njSqrt(square);
    yaw = (Angle)(atan2f(free_camera.vz, free_camera.vx) * ANGLE_PER_RADIAN);
    if (HELD(KL)) {
        yaw -= (Angle)ANGLE_PER_DEGREE;
    }
    if (HELD(KR)) {
        yaw += (Angle)ANGLE_PER_DEGREE;
    }
    free_camera.vx = radius * njCos(yaw);
    free_camera.vz = radius * njSin(yaw);
    /* @hack type: the camera's vx read as an NJS_VECTOR, where a copy of its
     * vx, vy and vz would do */
    njUnitVector((NJS_VECTOR *)&free_camera.vx);
    njSetCamera(&free_camera);
}
