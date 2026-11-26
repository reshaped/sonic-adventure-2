#include <ninja.h>
#include <nj_motion.h>
#include <sg_xpt.h>
#include <shinobi.h>
#include <stdio.h>

#include "camera/camera.h"
#include "event/areas.h"
#include "event/lighting.h"
#include "system/port.h"

#include "event/scene.h"

/* SHC places BSS in the order it first sees each name. the image holds these
 * three between scene.h's variables and playback.h's, so they are defined
 * between the two includes */
static NJS_CAMERA scene_camera;
static NJS_POINT3 cull_origin;
static Bool paused;

#include "event/playback.h"
#include "camera/helpers.h"
#include "draw/fade.h"
#include "draw/light.h"
#include "draw/texture_anim.h"
#include "effect/particle.h"
#include "event/afterimage.h"
#include "event/editor.h"
#include "event/effect_track.h"
#include "event/equipment.h"
#include "event/loader.h"
#include "event/messages.h"
#include "event/mirror.h"
#include "event/movie.h"
#include "event/scene_fog.h"
#include "event/script.h"
#include "event/tails.h"
#include "event/texture.h"
#include "event/typing.h"
#include "hud/message_window.h"
#include "main.h"
#include "save/card.h"
#include "save/save.h"
#include "shared/staff_roll.h"
#include "sound/adx.h"
#include "sound/sound.h"
#include "system/flags.h"
#include "system/frame.h"
#include "system/game_state.h"
#include "system/pad.h"
#include "system/sequence.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"
#include "system/vector.h"
#include "vmu/lcd.h"

#ifdef NONMATCHING
/* present_step, which this side's loop calls after each step */
#include "engine/dc/port.h"
#endif

#define SCENE_TASKS 16
/* the task level of the scene's tasks */
#define SCENE_LEVEL 1
/* each frame lasts FRAME_BLANKS vertical blanks and takes FRAME_STEPS steps,
 * until event_set_frame_interval paces the event */
#define FRAME_BLANKS 1
#define FRAME_STEPS 1
#define DISPLAY_SPEED_STEPS 100
/* a still entity is culled beyond CULL_RADIUS of the point CULL_AHEAD along
 * the camera's view */
#define CULL_RADIUS 1000.0F
#define CULL_AHEAD 500.0F
/* the hero story's last event, and the dark story's */
#define HERO_FINALE 28
#define DARK_FINALE 131
/* the events the last story plays on either side of EVENT_STAFF_ROLL */
#define BEFORE_STAFF_ROLL 208
#define AFTER_STAFF_ROLL 211
/* the event SEQUENCE_MENU_EVENT plays */
#define MENU_EVENT 350
/* under PAL's 50 frames a second, every PAL_STEPS steps take one more. the
 * event then runs as it does at NTSC's 60 */
#define PAL_STEPS 5
/* the foreground's alpha where its quad draws opaque */
#define OPAQUE_ALPHA 255
#define FADE_QUAD_VERTICES 4
/* the paused print of the texture memory. its labels stand at TEXMEM_LABEL_X
 * and its sizes at TEXMEM_VALUE_X. its rows run from TEXMEM_Y, TEXMEM_SPACING
 * apart */
#define TEXMEM_LABEL_X 5
#define TEXMEM_VALUE_X 20
#define TEXMEM_Y 24
#define TEXMEM_SPACING 2

struct event_scene *event_scenes;
struct message_list *event_messages;
Sint32 event_number;
Float event_frame;
Float event_scene_frame;
Float event_previous_frame;
Float event_frame_step;
Sint32 event_scene_index;
Float event_big_frame;
Sint32 event_big_motion;
Bool event_big_triggered;
struct event_fog_cue event_script_fog;
struct event_lighting event_lights[EVENT_LIGHTS];
struct event_fade event_foreground;
struct event_fade event_background;
Sint32 event_sequence_index;
Bool event_free_camera;
Bool event_reset_pressed;
enum playback_state { LOAD, RESTART, TYPE_LINE, PLAY, FINISH, IDLE };
static enum playback_state playback;
static Sint8 saved_unk_0b, saved_hud;
static struct camera_place view;
static Sint32 previous_scene;
static Sint32 camera_track;
static Sint32 display_speed;
static Uint8 unk_51a5dc[24];
static struct task *tasks[SCENE_TASKS];
static Sint32 task_count;
static Sint32 node_count;
static Uint8 unk_51a63c[0x22038];
static Sint32 pal_steps;

Bool event_loop = TRUE;
Bool event_debug_print = FALSE;
static Float unk_5143f4[] = {0.0F,   0.0F,   255.0F, 255.0F,
                             128.0F, 128.0F, 320.0F, 240.0F};
static Uint8 *unk_514414 = unk_51a63c;
static Sint32 unk_514418 = 1;
static NJS_POLYGON_VTX fade_quad[FADE_QUAD_VERTICES] = {
#include "event/scene/fade_quad.inc"
};
struct ending_movie {
    Sint32 event, movie;
};
KEPT static struct ending_movie ending_movies[2] = {
#include "event/scene/ending_movies.inc"
};
static Uint32 idle_colors[] = {0x80ff0000, 0x8000ff00, 0x800000ff};
struct event_scene_file *event_scene_file =
    (struct event_scene_file *)EVENT_SCENE_AREA;
#ifdef NONMATCHING
#else
static struct message_list *message_file =
    (struct message_list *)EVENT_MESSAGE_AREA;
#endif

#ifdef NONMATCHING
/* game_state.display by its own spelling: this side's objects don't define
 * event_display_mode, which only symbols.txt places */
#define PLAYBACK_DISPLAY game_state.display
#else
/* @hack spelling: game_state.display read through event_display_mode, a
 * symbol at its address, where game_state.display would do */
extern Sint8 event_display_mode;
#define PLAYBACK_DISPLAY event_display_mode
#endif

static void select_scene(void);

void event_set_frame_interval(int interval)
{
    frame_pacing_init(FRAME_BLANKS, FRAME_STEPS);
    frame_interval = interval;
    /* @hack spelling: frame_interval read back, where interval would do */
    frame_countdown = frame_interval;
    frame_step_count = interval;
    frame_step = 0;
}

static void count_nodes(NJS_CNK_OBJECT *object)
{
    node_count++;
    if (object->child != NULL) {
        count_nodes(object->child);
    }
    if (object->sibling != NULL) {
        count_nodes(object->sibling);
    }
}

static Float cull_distance(NJS_POINT3 *pos)
{
    NJS_VECTOR delta;
    delta.x = pos->x - cull_origin.x;
    delta.y = pos->y - cull_origin.y;
    delta.z = pos->z - cull_origin.z;
    return njScalor(&delta);
}

/* @hack spelling: an empty arm for EVENT_FORCE_SIMPLE, where one test of
 * both flags would do */
#define RETURN_IF_MULTI(kind)                                                  \
    if (entity->flags & EVENT_FORCE_SIMPLE) {                                  \
    } else if (entity->flags & EVENT_LIGHT_MASK) {                             \
        return kind;                                                           \
    }

static enum event_draw_kind draw_kind(struct event_entity *entity)
{
    NJS_POINT3 pos;
    Float distance;

    if (entity->object == NULL) {
        return EVENT_CULLED;
    }
    if (entity->motion != NULL) {
        if (event_motion_position(entity->motion, event_scene_frame, &pos) ==
            FALSE) {
            pos = entity->position;
        }
        if (entity->flags & EVENT_SHADOW_VOLUME) {
            return EVENT_MODIFIER_MOTION;
        }
        if (entity->shape != NULL) {
            RETURN_IF_MULTI(EVENT_MULTI_SHAPE)
            return EVENT_SHAPE;
        } else {
            RETURN_IF_MULTI(EVENT_MULTI_MOTION)
            return EVENT_MOTION;
        }
    }
    if (entity->flags & EVENT_SKIP_FOG) {
        return EVENT_UNFOGGED;
    }
    distance = cull_distance(&entity->position);
    if (distance < CULL_RADIUS) {
        if (entity->flags & EVENT_SHADOW_VOLUME) {
            return EVENT_MODIFIER;
        }
        RETURN_IF_MULTI(EVENT_MULTI_OBJECT)
        return EVENT_OBJECT;
    }
    return EVENT_CULLED;
}

/* @hack spelling: the entity indexed afresh at each use, where a pointer to it
 * would do */
#define ENTITY event_scenes[scene].entities[i]
#define MIRROR(kind) event_mirrors_draw(kind, scene, i)

static void draw_reflections(Sint32 scene)
{
    Sint32 i;
    draw_control_set(DRAW_CONTROL_SHADOW);
    event_mirrors_init();
    for (i = 0; i < event_scenes[scene].entity_count; i++) {
        switch (draw_kind(&ENTITY)) {
        case EVENT_OBJECT:
            MIRROR(EVENT_OBJECT);
            break;
        case EVENT_MULTI_OBJECT:
            event_select_lights(ENTITY.flags);
            MIRROR(EVENT_MULTI_OBJECT);
            break;
        case EVENT_MOTION:
            MIRROR(EVENT_MOTION);
            break;
        case EVENT_MULTI_MOTION:
            event_select_lights(ENTITY.flags);
            MIRROR(EVENT_MULTI_MOTION);
            break;
        case EVENT_SHAPE:
            MIRROR(EVENT_SHAPE);
            break;
        case EVENT_MULTI_SHAPE:
            event_select_lights(ENTITY.flags);
            MIRROR(EVENT_MULTI_SHAPE);
            break;
        case EVENT_MODIFIER:
            MIRROR(EVENT_MODIFIER);
            break;
        case EVENT_MODIFIER_MOTION:
            MIRROR(EVENT_MODIFIER_MOTION);
            break;
        default:
            break;
        }
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void animate_uv(Sint32 scene, Sint32 index)
{
    struct event_scene *current = &event_scenes[scene];
    if (current->entities[index].texture_animation != NULL) {
        texture_anim_apply(current->entities[index].object->model,
                           current->entities[index].texture_animation,
                           event_frame);
    }
}

#define PREPARE_MODEL()                                                        \
    njPushMatrix(NULL);                                                        \
    animate_uv(scene, i)
#define SIMPLE_DRAW                                                            \
    ((ENTITY.flags & EVENT_SIMPLE_LIGHT) || (ENTITY.flags & EVENT_FORCE_SIMPLE))
#define BIG event_scenes[event_scene_index].big
#define BIG_MOTION BIG->motions[event_big_motion]

static void draw_models(Sint32 scene)
{
    Sint32 i;
    event_attachments_reset();
    draw_control_set(DRAW_CONTROL_SHADOW);
    for (i = 0; i < event_scenes[scene].entity_count; i++) {
        event_attachments_watch(ENTITY.object);
        switch (draw_kind(&ENTITY)) {
        case EVENT_OBJECT:
            PREPARE_MODEL();
            if (SIMPLE_DRAW) {
                njCnkSimpleDrawObject(ENTITY.object);
            } else {
                njCnkEasyDrawObject(ENTITY.object);
            }
            njPopMatrix(1);
            break;
        case EVENT_MULTI_OBJECT:
            event_select_lights(ENTITY.flags);
            PREPARE_MODEL();
            if (ENTITY.flags & EVENT_SIMPLE_LIGHT) {
                njCnkSimpleMultiDrawObject(ENTITY.object);
            } else {
                njCnkEasyMultiDrawObject(ENTITY.object);
            }
            njPopMatrix(1);
            break;
        case EVENT_MOTION:
            PREPARE_MODEL();
            if (SIMPLE_DRAW) {
                njCnkSimpleDrawMotion(ENTITY.object, ENTITY.motion,
                                      event_scene_frame);
            } else {
                njCnkEasyDrawMotion(ENTITY.object, ENTITY.motion,
                                    event_scene_frame);
            }
            njPopMatrix(1);
            break;
        case EVENT_MULTI_MOTION:
            event_select_lights(ENTITY.flags);
            PREPARE_MODEL();
            if (ENTITY.flags & EVENT_SIMPLE_LIGHT) {
                njCnkSimpleMultiDrawMotion(ENTITY.object, ENTITY.motion,
                                           event_scene_frame);
            } else {
                njCnkEasyMultiDrawMotion(ENTITY.object, ENTITY.motion,
                                         event_scene_frame);
            }
            njPopMatrix(1);
            break;
        case EVENT_SHAPE:
            PREPARE_MODEL();
            if (SIMPLE_DRAW) {
                njCnkSimpleDrawShapeMotion(ENTITY.object, ENTITY.motion,
                                           ENTITY.shape, event_scene_frame);
            } else {
                njCnkEasyDrawShapeMotion(ENTITY.object, ENTITY.motion,
                                         ENTITY.shape, event_scene_frame);
            }
            njPopMatrix(1);
            break;
        case EVENT_MULTI_SHAPE:
            event_select_lights(ENTITY.flags);
            PREPARE_MODEL();
            if (ENTITY.flags & EVENT_SIMPLE_LIGHT) {
                njCnkSimpleMultiDrawShapeMotion(ENTITY.object, ENTITY.motion,
                                                ENTITY.shape,
                                                event_scene_frame);
            } else {
                njCnkEasyMultiDrawShapeMotion(ENTITY.object, ENTITY.motion,
                                              ENTITY.shape, event_scene_frame);
            }
            njPopMatrix(1);
            break;
        case EVENT_UNFOGGED:
            njPushMatrix(NULL);
            njFogDisable();
            animate_uv(scene, i);
            njCnkEasyDrawObject(ENTITY.object);
            njFogEnable();
            njPopMatrix(1);
            break;
        case EVENT_MODIFIER:
            PREPARE_MODEL();
            njCnkModDrawObject(ENTITY.object);
            njPopMatrix(1);
            break;
        case EVENT_MODIFIER_MOTION:
            PREPARE_MODEL();
            event_draw_modifier_motion(ENTITY.object, ENTITY.motion,
                                       event_scene_frame);
            njPopMatrix(1);
            break;
        default:
            break;
        }
        event_attachments_unwatch();
    }
    event_attachments_draw();
    /* @bug logic: BIG is the current scene's Big, whatever scene is, so
     * draw_scenes' calls at 0x8c500fb6 and 0x8c500fbe draw him twice a
     * frame */
    if (BIG->count != 0) {
        /* @bug memory: once a scene that doesn't loop has played Big's last
         * motion, event_big_motion is -1, and 0x8c500836 reads motions[-1]
         * to draw him */
#ifdef NONMATCHING
        /* this side repairs the bug tagged above: once the motions end, Big
         * draws no more */
        if (event_big_motion < 0) {
            draw_control_clear(DRAW_CONTROL_SHADOW);
            return;
        }
#endif
        njPushMatrix(NULL);
        if (BIG_MOTION.shape != NULL) {
            njCnkSimpleDrawShapeMotion(BIG->object, BIG_MOTION.motion,
                                       BIG_MOTION.shape, event_big_frame);
        } else {
            njCnkSimpleDrawMotion(BIG->object, BIG_MOTION.motion,
                                  event_big_frame);
        }
        njPopMatrix(1);
    }
    draw_control_clear(DRAW_CONTROL_SHADOW);
}

static void emit_afterimages(Sint32 scene)
{
    Sint32 i;
    Sint32 j;
    struct event_afterimage pose;
    for (i = 0; i < event_scenes[scene].entity_count; i++) {
        for (j = 0; j < EVENT_SLOTS; j++) {
            /* @hack spelling: active compared with TRUE, where the flag alone
             * would do */
            if (event_repeats[j].active == TRUE &&
                ENTITY.object == event_scene_file->slot_objects[j]) {
                pose.object = ENTITY.object;
                pose.motion = ENTITY.motion;
                pose.shape = ENTITY.shape;
                pose.frame = event_scene_frame;
                pose.kind = draw_kind(&ENTITY);
                pose.flags = ENTITY.flags;
                pose.lifetime = event_repeats[j].pose_frames;
                event_afterimage_start(&pose);
                event_repeats[j].age++;
                if (event_repeats[j].age > event_repeats[j].emission_frames) {
                    event_repeats[j].active = FALSE;
                }
            }
        }
    }
}

static void set_camera(void)
{
    if (paused) {
        njSetCamera(&_nj_motion_camera_);
    } else {
        if (camera_track >= event_scenes[event_scene_index].camera_count) {
            camera_track = 0;
        }
        njCameraMotion(&scene_camera,
                       event_scenes[event_scene_index].cameras[camera_track],
                       event_scene_frame);
        if (event_free_camera) {
            event_editor_set_camera();
        }
    }
    cull_origin.x =
        _nj_motion_camera_.px + (_nj_motion_camera_.vx * CULL_AHEAD);
    cull_origin.y =
        _nj_motion_camera_.py + (_nj_motion_camera_.vy * CULL_AHEAD);
    cull_origin.z =
        _nj_motion_camera_.pz + (_nj_motion_camera_.vz * CULL_AHEAD);
    view.pos.x = _nj_motion_camera_.px;
    view.pos.y = _nj_motion_camera_.py;
    view.pos.z = _nj_motion_camera_.pz;
    view.dir.x = _nj_motion_camera_.vx;
    view.dir.y = _nj_motion_camera_.vy;
    view.dir.z = _nj_motion_camera_.vz;
    vector_sum(&view.pos, &view.dir, &view.target);
    view.dist = 1.0F;
    look_angles(&view.dir, &view.ang.x, &view.ang.y);
}

#define RGB(array) (array)[0], (array)[1], (array)[2]
#define XYZ(point) (point).x, (point).y, (point).z
static void set_lighting(void)
{
    Sint32 i;
    NJS_VECTOR direction;
    njCnkSetEasyMultiLight(EVENT_LIGHTS);
    njCnkSetSimpleMultiLight(EVENT_LIGHTS);
    for (i = 0; i < EVENT_LIGHTS; i++) {
        njUnitVector(&event_lights[i].direction);
        njCnkSetEasyMultiLightVectorEx(i + 1, XYZ(event_lights[i].direction));
        njCnkSetEasyMultiLightColor(i + 1, RGB(event_lights[i].color));
        njCnkSetSimpleMultiLightVectorEx(i + 1, XYZ(event_lights[i].direction));
        njCnkSetSimpleMultiLightColor(i + 1, RGB(event_lights[i].color));
    }
    njCnkSetEasyMultiAmbient(RGB(event_lights[0].ambient));
    njCnkSetSimpleMultiAmbient(RGB(event_lights[0].ambient));
    njCnkSetEasyMultiLightMatrices();
    njCnkSetSimpleMultiLightMatrices();
    njCalcVector(NULL, &event_lights[0].direction, &direction);
    njUnitVector(&direction);
    easy_light_set_direction(XYZ(direction));
    njCnkSetEasyLightIntensity(1.0F, event_lights[0].ambient_intensity);
    njCnkSetEasyLightColor(RGB(event_lights[0].color));
    simple_light_set_direction(XYZ(direction));
    njCnkSetSimpleLightIntensity(1.0F, event_lights[0].ambient_intensity);
    njCnkSetSimpleLightColor(RGB(event_lights[0].color));
    njCnkSetAmbient(RGB(event_lights[0].ambient));
}
#define LIGHT_SWITCH(index)                                                    \
    if (flags & EVENT_LIGHT(index)) {                                          \
        njCnkSetEasyMultiLightSwitch(index, TRUE);                             \
        njCnkSetSimpleMultiLightSwitch(index, TRUE);                           \
    } else {                                                                   \
        njCnkSetEasyMultiLightSwitch(index, FALSE);                            \
        njCnkSetSimpleMultiLightSwitch(index, FALSE);                          \
    }
void event_select_lights(Uint32 flags)
{
    LIGHT_SWITCH(1)
    LIGHT_SWITCH(2)
    LIGHT_SWITCH(3)
    LIGHT_SWITCH(4)
}

static void draw_scenes(void)
{
    njSetTexture(event_scene_file->texlist);
    set_lighting();
    draw_reflections(0);
    draw_reflections(event_scene_index);
    njSetTexture(event_scene_file->texlist);
    draw_models(0);
    draw_models(event_scene_index);
}

static void draw_editor(void)
{
    if (event_editor.mode == EDIT_SPRITES) {
        event_editor_draw_sprite();
    }
    if (event_editor.mode == EDIT_BURSTS) {
        event_editor_draw_burst();
    }
}

static void draw_fade(void)
{
    Uint32 back = event_background.current.color;
    Bool translucent;
    njSetBackColor(back, back, back);
    /* @hack spelling: the draw in an else after the return, where it could
     * follow the if */
    if (event_foreground.current.argb.a == 0) {
        return;
    } else {
        if (event_foreground.current.argb.a == OPAQUE_ALPHA) {
            translucent = FALSE;
        } else {
            translucent = TRUE;
        }
        fade_quad[0].col = event_foreground.current.color;
        fade_quad[1].col = event_foreground.current.color;
        fade_quad[2].col = event_foreground.current.color;
        fade_quad[3].col = event_foreground.current.color;
        njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
        njColorBlendingMode(NJD_DESTINATION_COLOR,
                            NJD_COLOR_BLENDING_INVSRCALPHA);
        njDrawPolygon(fade_quad, FADE_QUAD_VERTICES, translucent);
    }
}

static void rewind_event(void)
{
    event_frame_step = EVENT_FRAME_STEP;
    event_frame = EVENT_FRAME_STEP;
    event_previous_frame = 0.0F;
    event_big_triggered = FALSE;
    event_big_motion = 0;
    event_big_frame = 0.0F;
    previous_scene = 0;
    event_scene_index = 0;
    camera_track = 0;
    select_scene();
}

#ifdef NONMATCHING
#else
static void select_messages(Sint32 event)
{
    /* @hack redundant: unused, a local that only sizes the frame */
    char unused[48];
    Sint32 i;
    (void)unused;
    for (i = 0; message_file[i].event != event; i++) {
        if (message_file[i].event < 0) {
            event_messages = NULL;
            return;
        }
    }
    event_messages = &message_file[i];
}
#endif

static void debug_step(struct task *task)
{
    if (player_pads[EVENT_PLAYER_PAD]->on & PDD_DGT_TR) {
        event_frame_step += 0.01F;
        display_speed = DISPLAY_SPEED_STEPS;
    }
    if (player_pads[EVENT_PLAYER_PAD]->on & PDD_DGT_TL) {
        event_frame_step -= 0.01F;
        display_speed = DISPLAY_SPEED_STEPS;
    }
    if (player_pads[EVENT_PLAYER_PAD]->on & PDD_DGT_TB) {
        event_frame_step = 0.0F;
    }
    if ((player_pads[EVENT_PLAYER_PAD]->on & PDD_DGT_TB) &&
        (pad_press_repeat[EVENT_PLAYER_PAD] & PDD_DGT_KU)) {
        event_frame_step = 1.0F;
    } else if ((player_pads[EVENT_PLAYER_PAD]->on & PDD_DGT_TB) &&
               (pad_press_repeat[EVENT_PLAYER_PAD] & PDD_DGT_KD)) {
        event_frame_step = -1.0F;
    } else if ((player_pads[EVENT_PLAYER_PAD]->on & PDD_DGT_TB) &&
               (pad_press_repeat[EVENT_PLAYER_PAD] & PDD_DGT_KR)) {
        event_frame_step = 10.0F;
    } else if ((player_pads[EVENT_PLAYER_PAD]->on & PDD_DGT_TB) &&
               (pad_press_repeat[EVENT_PLAYER_PAD] & PDD_DGT_KL)) {
        event_frame_step = -10.0F;
    }
    if ((player_pads[EVENT_PLAYER_PAD]->on & PDD_DGT_TX) &&
        (player_pads[EVENT_PLAYER_PAD]->press & PDD_DGT_TY)) {
        rewind_event();
    }
    if (display_speed != 0) {
        display_speed--;
    }
    if (player_pads[EVENT_EDIT_PAD]->press & PDD_DGT_TY) {
        camera_track++;
    }
}

static void debug_draw(struct task *task)
{
    if (display_speed != 0 && event_debug_print) {
        njPrintF(NJM_LOCATION(1, 10), event_frame_step, 3);
    }
}

static void debug_destroy(struct task *task)
{
}

static void select_scene(void)
{
    Float start;
    event_scene_index = 1;
    start = 0.0F;
    while (start < event_frame) {
        if (event_scene_index >= event_scene_file->scene_count) {
            break;
        }
        if (!(event_frame >
              start + event_scenes[event_scene_index].frames - 1.0F)) {
            break;
        }
        start += event_scenes[event_scene_index].frames;
        event_scene_index++;
    }
    event_scene_frame = event_frame - start;
    if (event_scene_frame < 0.0F) {
        event_scene_frame = 0.0F;
    }
    if (previous_scene != event_scene_index) {
        event_big_triggered = FALSE;
        event_big_motion = 0;
        event_big_frame = 0.0F;
    }
    previous_scene = event_scene_index;
    if (event_big_triggered) {
        event_big_frame += event_frame_step;
        if (event_big_frame >= (Float)BIG_MOTION.motion->nbFrame) {
            event_big_motion++;
            event_big_triggered = FALSE;
            event_big_frame = 0.0F;
            if (event_big_motion >= BIG->count) {
                if (BIG->loop) {
                    event_big_motion = 0;
                } else {
                    event_big_motion = -1;
                }
            }
        }
    }
}

static void play_ending_movie(void)
{
    Sint32 i;
    for (i = 0; (Uint32)i < sizeof(ending_movies) / sizeof(ending_movies[0]);
         i++) {
        if (ending_movies[i].event == event_number) {
            struct event_movie_request movie = {NULL, 0, EVENT_MOVIE_FADE_OUT};
            char filename[32];
            sprintf(filename, "e%04d.sfd", (int)ending_movies[i].movie);
            movie.name = filename;
            event_movie_draw_plane = FALSE;
            event_movie_play(&movie);
            break;
        }
    }
}

static void scene_step(struct task *task)
{
    event_previous_frame = event_frame;
    event_frame += event_frame_step;
    if (event_loop) {
        if (event_frame >= event_scenes->frames - 1) {
            playback = RESTART;
        } else if (event_frame < 1.0F) {
            event_frame = event_scenes->frames - 1.0F;
            event_previous_frame = event_scenes->frames;
        }
    } else {
        if (event_frame >= event_scenes->frames - 1) {
            playback = FINISH;
            event_frame = event_scenes->frames - 1;
        } else if (event_frame < 1.0F) {
            event_frame = 1.0F;
        }
    }
    select_scene();
    emit_afterimages(0);
    emit_afterimages(event_scene_index);
    event_fog_select(event_scene_index);
    if (event_debug_print) {
        njPrintD(NJM_LOCATION(4, 8), event_scene_index, 2);
    }
    if (event_scene_file->animation != NULL) {
        event_texture_frame(
            event_scene_file->animation->sets,
            event_scene_file->animation->texture,
            ((Sint32)event_frame % event_scene_file->animation->frames) +
                event_scene_file->animation->texture);
    }
}

static void scene_draw(struct task *task)
{
    if (event_frame != 0.0F) {
        if (event_debug_print) {
            njPrintF(NJM_LOCATION(1, 4), event_frame, 4);
        }
        /* @hack redundant: the second test of event_debug_print, where one
         * test around both prints would do */
        if (event_debug_print) {
            njPrintF(NJM_LOCATION(1, 6), event_scene_frame, 4);
        }
        njSetBackColor(0, 0, 0);
        set_camera();
        draw_scenes();
        draw_fade();
        event_effects_draw();
        draw_editor();
    }
}
static void scene_destroy(struct task *task)
{
}

#define ADVANCE_FRAME()                                                        \
    task_step_all();                                                           \
    event_script_step(event_number);                                           \
    event_tails_step();                                                        \
    event_movie_step()

#ifdef NONMATCHING
/* draw the picture a frame of the event draws after its steps, for
 * present_step */
static void event_draw_step(void)
{
    event_movie_draw();
    task_draw_all();
}

/* TRUE where Start asks to skip the event, by the test SEGA's PLAY makes
 * below. the endings and the staff roll take no skip */
static Bool skip_asked(void)
{
    return !event_free_camera &&
           (game_bits[event_number] || game_state.debug) &&
           event_number != HERO_FINALE && event_number != BEFORE_STAFF_ROLL &&
           event_number != EVENT_STAFF_ROLL &&
           event_number != AFTER_STAFF_ROLL &&
           (player_pads[EVENT_PLAYER_PAD]->press & PDD_DGT_ST) &&
           event_editor.mode == EDIT_OFF;
}
/* TRUE once Start has asked for the skip while the event's line types */
static Bool typed_skip;
#endif

int event_step(void)
{
    int result = OVERLAY_RUNNING;
    int i;
    save_count_time();
    pad_scan();
    switch (playback) {
    case LOAD:
        pad_enable();
        /* @hack spelling: the assignment inside the test, where a statement of
         * its own would do */
        if ((event_number = event_load_step()) < 0) {
#ifdef NONMATCHING
            /* this side resets the overlay where the event failed to load */
            if (event_number == EVENT_LOAD_FAILED) {
                return OVERLAY_RESET;
            }
#endif
            break;
        } else if (event_number >= EVENT_FIRST_MOVIE) {
            goto finish;
        }
#ifdef NONMATCHING
#else
        select_messages(event_number);
#endif
        event_tails_start();
        fade_set_color(0);
        playback = RESTART;
        break;
    case RESTART:
        particle_core_start();
        rewind_event();
        event_hide_unequipped();
        event_script_reset();
        playback = PLAY;
        pad_enable();
        /* @hack redundant: the constant test around the call, where the call
         * alone would do */
        1.0F ? sound_stop_all() : (void)0;
        pal_steps = 0;
#ifdef NONMATCHING
        /* this side's skip of the typed line starts unasked */
        typed_skip = FALSE;
#endif
        if (event_typing_start()) {
            playback = TYPE_LINE;
            event_set_frame_interval(EVENT_EVERY_BLANK);
        }
        break;
    case TYPE_LINE:
#ifdef NONMATCHING
        /* this side takes Start while the line types too, where SEGA's
         * takes it in PLAY alone. the line ends where it stands, and the
         * event finishes once the line's sprites are gone */
        if (skip_asked()) {
            typed_skip = TRUE;
            event_typing_end();
        }
#endif
        if (event_typing_step()) {
#ifdef NONMATCHING
            /* this side finishes the event where Start asked the skip
             * while the line typed */
            if (typed_skip) {
                playback = FINISH;
                break;
            }
#endif
            playback = PLAY;
            if (event_tracks->sounds[0].mode == EVENT_HALF_RATE) {
                event_set_frame_interval(EVENT_EVERY_OTHER_BLANK);
            } else {
                event_set_frame_interval(EVENT_EVERY_BLANK);
            }
        }
        break;
    case PLAY:
        if (paused) {
            if (event_debug_print) {
                njPrint(NJM_LOCATION(24, 20), "PAUSE");
                njPrint(NJM_LOCATION(TEXMEM_LABEL_X, TEXMEM_Y), "FREESIZE ->");
                /* @hack redundant: a test of event_debug_print, which the
                 * test around it makes true */
                if (event_debug_print) {
                    njPrintH(NJM_LOCATION(TEXMEM_VALUE_X, TEXMEM_Y),
                             njCalcTexture(NJD_TEXMEM_FREESIZE), 8);
                }
                njPrint(NJM_LOCATION(TEXMEM_LABEL_X, TEXMEM_Y + TEXMEM_SPACING),
                        "BLOCKSIZE  ->");
                /* @hack redundant: a test of event_debug_print, which the
                 * test around it makes true */
                if (event_debug_print) {
                    njPrintH(
                        NJM_LOCATION(TEXMEM_VALUE_X, TEXMEM_Y + TEXMEM_SPACING),
                        njCalcTexture(NJD_TEXMEM_MAXBLOCK), 8);
                }
                njPrint(NJM_LOCATION(TEXMEM_LABEL_X,
                                     TEXMEM_Y + (2 * TEXMEM_SPACING)),
                        "MAXSIZE  ->");
                /* @hack redundant: a test of event_debug_print, which the
                 * test around it makes true */
                if (event_debug_print) {
                    njPrintH(NJM_LOCATION(TEXMEM_VALUE_X,
                                          TEXMEM_Y + (2 * TEXMEM_SPACING)),
                             njCalcTexture(NJD_TEXMEM_MAXSIZE), 8);
                }
            }
        } else {
            i = 0;
            while (i++ < frame_interval) {
                ADVANCE_FRAME();
                pal_steps++;
                if (PLAYBACK_DISPLAY == DISPLAY_PAL && pal_steps == PAL_STEPS) {
                    pal_steps = 0;
                    ADVANCE_FRAME();
                }
#ifdef NONMATCHING
                /* this side shows the step's picture where the port's 60 fps
                 * splits a half-rate event's frame (engine/dc/port.h) */
                present_step(i, event_draw_step);
#endif
            }
        }
        event_movie_draw();
        task_draw_all();
        if (!event_free_camera) {
            if (game_bits[event_number] || game_state.debug) {
                if (event_number != HERO_FINALE &&
                    event_number != BEFORE_STAFF_ROLL &&
                    event_number != EVENT_STAFF_ROLL &&
                    event_number != AFTER_STAFF_ROLL) {
                    if ((player_pads[EVENT_PLAYER_PAD]->press & PDD_DGT_ST) &&
                        event_editor.mode == EDIT_OFF) {
                        playback = FINISH;
                    }
                }
            }
        }
        if (pad_reset_pressed(0)) {
            paused = FALSE;
            playback = FINISH;
            event_reset_pressed = TRUE;
            if (event_number == MENU_EVENT) {
                game_state.demo = DEMO_ON;
            }
        } else {
            event_reset_pressed = FALSE;
        }
        break;
    case FINISH:
        if (event_number == EVENT_STAFF_ROLL) {
            staff_roll_release();
        }
        if (message_window_current() != NULL) {
            task_destroy(message_window_current());
        }
        sound_stop_all();
        voices_stop();
        event_tails_stop();
        pad_disable();
        njReleaseTexture(*EVENT_TEXLIST_AREA);
        njReleaseTexture(&event_effect_texlist);
        texture_release_shared(event_pvms, event_texture_groups);
        particle_core_stop();
        event_movie_stop_scenes();
        if (!event_reset_pressed) {
            play_ending_movie();
        }
        music_stop();
    finish:
        if (event_reset_pressed) {
            result = OVERLAY_RESET;
        } else if (game_flags[FLAG_SEQUENCE]) {
            if (event_sequence_index >= SEQUENCE_EVENTS ||
                sequence_events[event_sequence_index] < 0) {
                result = OVERLAY_DONE;
                switch (sequence_events[0]) {
                case MENU_EVENT:
                    break;
                case HERO_FINALE:
                case DARK_FINALE:
                    save_award();
                    break;
                default:
                    save_write();
                    break;
                }
                sequence_advance();
                event_set_frame_interval(EVENT_EVERY_BLANK);
            } else {
                playback = LOAD;
                event_set_frame_interval(EVENT_EVERY_BLANK);
            }
        } else {
            playback = LOAD;
            event_set_frame_interval(EVENT_EVERY_BLANK);
        }
        break;
    case IDLE:
    default:
        fade_set_color(0);
        njSetBackColor(idle_colors[0], idle_colors[1], idle_colors[2]);
        task_step_all();
        task_draw_all();
        break;
    }
    return result;
}

void event_init(void)
{
    struct task **slot;
    struct task *task;
    Sint32 i;
    (void)unk_51a5dc;
    (void)unk_5143f4;
    (void)unk_514414;
    (void)unk_514418;
    (void)count_nodes;
    frame_pacing_init(FRAME_BLANKS, FRAME_STEPS);
    task_init();
    /* @hack spelling: the slots cleared through a pointer, counting down,
     * where tasks[i] = NULL for each i would do */
    slot = tasks;
    for (i = SCENE_TASKS; i != 0; i--) {
        *slot++ = NULL;
    }
    task_count = 0;
    paused = FALSE;
    njInitCamera(&scene_camera);
    sound_task_create(NULL);
    task = task_create(TASK_WORK, SCENE_LEVEL, scene_step, "eventExec");
    tasks[task_count++] = task;
    task->draw = scene_draw;
    task->destructor = scene_destroy;
    if (game_flags[FLAG_SEQUENCE] == SEQUENCE_NONE) {
        task = task_create(TASK_WORK, SCENE_LEVEL, debug_step, "debugExec");
        tasks[task_count++] = task;
        task->draw = debug_draw;
        task->destructor = debug_destroy;
        display_speed = 0;
    }
    playback = LOAD;
    saved_unk_0b = game_state.unk_0b;
    saved_hud = game_state.hud_on;
    game_state.unk_0b = 0;
    game_state.hud_on = 0;
    camera_views[0] = &view;
    event_fog_start();
    event_sequence_index = 0;
    if (game_flags[FLAG_SEQUENCE]) {
        event_loop = FALSE;
    }
    draw_control_save();
    draw_control_clear(NJD_CONTROL_3D_MODEL_CLIP);
    card_init();
    lcd_start();
}

void event_exit(void)
{
    Sint32 i;
    for (i = 0; i < task_count; i++) {
        task_destroy(tasks[i]);
    }
    game_state.unk_0b = saved_unk_0b;
    game_state.hud_on = saved_hud;
    game_state.idle_dim = TRUE;
    camera_views[0] = NULL;
    event_fog_stop();
    music_stop();
    fun_8c059fb2();
    sound_stop_all_and_unpause();
    sound_task_kill();
    draw_control_restore();
    card_exit();
    lcd_stop();
    task_destroy_all();
    if (event_number == MENU_EVENT) {
        game_state.emblems = save_data.emblems;
        fade_set_color(event_foreground.current.color);
    }
}
