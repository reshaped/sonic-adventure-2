#include <ninja.h>
#include <nt_utl.h>
#include <sg_xpt.h>

#include "camera/stack.h"
#include "chao/garden.h"
#include "chao/garden_scene.h"
#include "chao/residents.h"
#include "chao/work.h"
#include "chaostgkinder/camera.h"
#include "chaostgkinder/land_data.h"
#include "chaostgkinder/messages.h"
#include "chaostgkinder/network.h"
#include "chaostgkinder/objects.h"
#include "chaostgkinder/rooms.h"
#include "chaostgkinder/services.h"
#include "chaostgkinder/widget.h"
#include "collision/world.h"
#include "draw/color.h"
#include "draw/light.h"
#include "player/player.h"
#include "sound/adx.h"
#include "system/angle.h"
#include "system/pad.h"
#include "system/task.h"
#include "system/texture.h"
#include "system/util.h"

/* a way into a room. pos and yaw are the player's place and heading. view is
 * the camera's angle of view, and the rest its place and angles */
struct kinder_entrance {
    NJS_POINT3 pos;
    Angle yaw, view;
    NJS_POINT3 camera_pos;
    Angle camera_pitch, camera_yaw;
};

struct kinder_object {
    void (*step)(struct task *task);
    struct angles ang;
    NJS_POINT3 pos, scl;
};

struct kinder_room {
    Sint32 id;
    Bool player_active;
    struct land_table *land;
    const struct kinder_entrance *entrances;
    Sint32 entrance_count;
    const struct kinder_object *objects;
    Sint32 object_count;
    const char *name;
};

/* every room's entrances. kinder_rooms gives each room its first and its
 * count. entrances 9, 13 and 14 belong to no room */
static const struct kinder_entrance entrances[17] = {
#include "chaostgkinder/rooms/entrances.inc"
};

static const struct kinder_object house_objects[2] = {
#include "chaostgkinder/rooms/house_objects.inc"
};
static const struct kinder_object corridor_objects[5] = {
#include "chaostgkinder/rooms/corridor_objects.inc"
};
static const struct kinder_object classroom_objects[] = {
    {kinder_classroom_init, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}};
static const struct kinder_object health_center_objects[2] = {
#include "chaostgkinder/rooms/health_center_objects.inc"
};
static const struct kinder_object principal_objects[3] = {
#include "chaostgkinder/rooms/principal_objects.inc"
};
static const struct kinder_object editor_objects[] = {
    {kinder_editor_init, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}};
static const struct kinder_object account_objects[] = {
    {kinder_account_init, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}};

#include "chaostgkinder/rooms/text.inc"

/* the rows of the black market, the library and the playground hold no land,
 * entrance or object. a request for one of those rooms loads nothing. no door
 * of the corridor leads to them. the account room's exits request the black
 * market. the book editor's exits request the library. nothing requests the
 * account room. only kinder_book_prompt_step requests the editor, and no task
 * runs that step. the image holds the library's land, kinder_library_land,
 * which no row names. the disc holds the black market's textures and the
 * library's */
static const struct kinder_room kinder_rooms[KINDER_ROOMS] = {
    {KINDER_HOUSE, TRUE, &kinder_house_land, &entrances[0], 2, house_objects,
     ARRAY_COUNT(house_objects), KINDER_ROOM_NAME_HOUSE},
    {KINDER_CORRIDOR, TRUE, &kinder_corridor_land, &entrances[2], 7,
     corridor_objects, ARRAY_COUNT(corridor_objects),
     KINDER_ROOM_NAME_CORRIDOR},
    {KINDER_BLACK_MARKET, FALSE, NULL, NULL, 0, NULL, 0,
     KINDER_ROOM_NAME_BLACK_MARKET},
    {KINDER_CLASSROOM, FALSE, &kinder_classroom_land, &entrances[10], 1,
     classroom_objects, ARRAY_COUNT(classroom_objects),
     KINDER_ROOM_NAME_CLASSROOM},
    {KINDER_HEALTH_CENTER, FALSE, &kinder_health_center_land, &entrances[11], 1,
     health_center_objects, ARRAY_COUNT(health_center_objects),
     KINDER_ROOM_NAME_HEALTH_CENTER},
    {KINDER_LIBRARY, FALSE, NULL, NULL, 0, NULL, 0, KINDER_ROOM_NAME_LIBRARY},
    {KINDER_PRINCIPAL, FALSE, &kinder_principal_land, &entrances[12], 1,
     principal_objects, ARRAY_COUNT(principal_objects),
     KINDER_ROOM_NAME_PRINCIPAL_ROOM},
    {KINDER_PLAYGROUND, FALSE, NULL, NULL, 0, NULL, 0,
     KINDER_ROOM_NAME_PLAYGROUND},
    {KINDER_CONNECT, FALSE, NULL, NULL, 0, NULL, 0, KINDER_ROOM_NAME_CONNECT},
    {KINDER_EDITOR, FALSE, NULL, &entrances[15], 1, editor_objects,
     ARRAY_COUNT(editor_objects), KINDER_ROOM_NAME_EDITOR},
    {KINDER_ACCOUNT, FALSE, NULL, &entrances[16], 1, account_objects,
     ARRAY_COUNT(account_objects), KINDER_ROOM_NAME_ACCOUNT}};

#ifdef NONMATCHING
/* the land's buffer from the end of the SDK code's BSS, where this side's
 * linker puts it. this side's objects don't define kinder_land_buffer, which
 * only symbols.txt places */
#include "chaostgkinder/network_layout.h"
#define LAND_BUILD_AREA kinder_sg_bss_end
#else
extern Uint8 kinder_land_buffer[];
#define LAND_BUILD_AREA kinder_land_buffer
#endif

/* room_flags' bits */
#define ROOM_REQUESTED 1
#define LOBBY_REQUESTED 0x10
#define PLAYER_PAUSED 0x20
#define CAMERA_PUSHED 0x40
#define FADE_OUT 0x100

/* the fade's alpha, which moves FADE_STEP a step between 0 and OPAQUE */
#define FADE_STEP 9
#define OPAQUE 255
/* the color behind the rooms, and the black a lobby request leaves */
#define ROOM_BACK_COLOR 0xff001a9c
#define BLACK 0xff000000

/* the kindergarten task's work->mode: the steps it opens with, in order,
 * then ROOM_OPEN, where it stays */
enum room_mode {
    ROOM_START,
    ROOM_NETWORK_INFO,
    ROOM_LOAD_HOUSE,
    ROOM_CARRIED,
    ROOM_OPEN
};
/* the most steps ROOM_NETWORK_INFO waits while ntInfIsNetworkInfo returns
 * NTD_ERR_NOTINIT */
#define NETWORK_INFO_STEPS 30

/* @hack type: main_light declared as an array of one, where a struct light
 * would do */
static struct light main_light[1] = {
#include "chaostgkinder/rooms/main_light.inc"
};
static struct light fill_light = {
#include "chaostgkinder/rooms/fill_light.inc"
};
static Sint32 room_flags = 0;
static Sint32 current_room = KINDER_HOUSE;
static Sint32 current_entrance = 0;
static Sint32 next_room = 0;
static Sint32 next_entrance = 0;
static Sint32 fade_alpha = 0;
static struct task *garden_camera = NULL;
struct task *kinder_carried_chao = NULL;
static NJS_POLYGON_VTX fade_quad[4] = {
#include "chaostgkinder/rooms/fade_quad.inc"
};
Sint32 kinder_network_info;

#ifdef NONMATCHING
/* every entrance on the disc has a view of ROOM_VIEW. one whose view is 0,
 * as in the build without the disc, whose includes hold zeros, takes it */
#define ROOM_VIEW ((Angle)(70 * ANGLE_PER_DEGREE))
#define ENTRANCE_VIEW(view) ((view) == 0 ? ROOM_VIEW : (view))
#else
#define ENTRANCE_VIEW(view) (view)
#endif

static void load_room(struct task *task)
{
#ifdef NONMATCHING
    /* the ang.z of each corridor door after corridor_objects[0] on the disc:
     * the room its model is for. the door's ang.x, its slot in doors, is one
     * less. a door whose ang.z is 0, as in the build without the disc, takes
     * both */
    static const Uint8 corridor_door_models[] = {
        KINDER_CORRIDOR, KINDER_CLASSROOM, KINDER_HEALTH_CENTER,
        KINDER_PRINCIPAL};
#endif
    const struct kinder_room *room;
    const struct kinder_entrance *entrance;
    const char *messages;
    struct land_table *land;
    struct task *child;
    struct task_work *player;
    const struct kinder_object *object;
    struct task_work *work;

#ifdef NONMATCHING
    /* this side loads no room for a negative entrance, which indexes the room's
     * entrances below */
    if (next_entrance < 0) {
        return;
    }
#endif
    for (room = kinder_rooms;
         room < kinder_rooms + KINDER_ROOMS && room->id != next_room; room++) {
    }
    if (room >= kinder_rooms + KINDER_ROOMS ||
        room->entrance_count <= next_entrance) {
        return;
    }
    entrance = &room->entrances[next_entrance];
    messages = NULL;
    switch (room->id) {
    case KINDER_HEALTH_CENTER:
        messages = "msgChaoKinDoctor";
        break;
    case KINDER_PRINCIPAL:
        messages = "msgChaoKinPrincipal";
        break;
    }
    if (messages) {
        kinder_messages_load(messages, KINDER_SERVICE_BANK);
    }
    world_land_stop();
    land = room->land;
    if (land) {
        world_land_start(land);
        world_build_land(land, (Uint32)LAND_BUILD_AREA);
    }
    if (room->player_active) {
        if (room_flags & PLAYER_PAUSED) {
            player_resume(0);
            room_flags &= ~PLAYER_PAUSED;
            if (kinder_carried_chao &&
                player_records[0]->held != kinder_carried_chao) {
                child = kinder_carried_chao;
                child->work->pos = player_work[0]->pos;
                /* @hack spelling: child->work cast at each use, where a
                 * struct chao_work local would do */
                ((struct chao_work *)child->work)->flags |= CHAO_NO_INTENTIONS;
                ((struct chao_work *)child->work)->flags &= ~CHAO_AGES;
                ((struct chao_work *)child->work)->flags &= ~CHAO_COLLIDES;
                ((struct chao_work *)child->work)->flags |= CHAO_MOVES;
                ((struct chao_work *)child->work)->flags |= CHAO_RUNS_BEHAVIOR;
                ((struct chao_work *)child->work)->flags |= CHAO_BODY_SPRINGS;
                ((struct chao_work *)child->work)->flags |= CHAO_BODY_IN_VIEW;
                ((struct chao_work *)child->work)->flags |= CHAO_VISIBLE;
                player_hand_task(0, child);
            }
        }
    } else if (!(room_flags & PLAYER_PAUSED)) {
        player_pause(0);
        room_flags |= PLAYER_PAUSED;
    }
    player = player_work[0];
    player->pos.x = entrance->pos.x;
    player->pos.y = entrance->pos.y;
    player->pos.z = entrance->pos.z;
    player->ang.y = entrance->yaw;
    pad_enable();
    if (room_flags & CAMERA_PUSHED) {
        camera_remove_mode(0, camera_top(0));
        room_flags &= ~CAMERA_PUSHED;
    }
    kinder_camera_start.pending = TRUE;
    kinder_camera_start.view = ENTRANCE_VIEW(entrance->view);
    kinder_camera_start.pos.x = entrance->camera_pos.x;
    kinder_camera_start.pos.y = entrance->camera_pos.y;
    kinder_camera_start.pos.z = entrance->camera_pos.z;
    kinder_camera_start.pitch = entrance->camera_pitch;
    kinder_camera_start.yaw = entrance->camera_yaw;
    switch (room->id) {
    case KINDER_BLACK_MARKET:
    case KINDER_CLASSROOM:
    case KINDER_HEALTH_CENTER:
    case KINDER_LIBRARY:
    case KINDER_PRINCIPAL:
    case KINDER_PLAYGROUND:
        camera_push_user_mode(0, kinder_room_camera);
        room_flags |= CAMERA_PUSHED;
        break;
    case KINDER_HOUSE:
        camera_push_user_mode(0, kinder_house_camera);
        room_flags |= CAMERA_PUSHED;
        break;
    case KINDER_CORRIDOR:
    default:
        if (!next_entrance) {
            garden_camera->work->mode = 0;
        }
        break;
    }
    camera_set_view_angle(0, ENTRANCE_VIEW(entrance->view), 0);
    task_destroy_children(task);
    current_room = next_room;
    current_entrance = next_entrance;
    for (object = room->objects; object < room->objects + room->object_count;
         object++) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((child = task_create_child(TASK_WORK, object->step, task)) !=
            NULL) {
            work = child->work;
            work->ang = object->ang;
#ifdef NONMATCHING
            /* this side gives a corridor door of ang.z 0 the model and slot
             * that corridor_door_models holds for it. the disc-free build's
             * doors have ang.z 0 */
            if (room->id == KINDER_CORRIDOR &&
                object->step == kinder_door_init && work->ang.z == 0) {
                work->ang.z =
                    corridor_door_models[object - (corridor_objects + 1)];
                work->ang.x = work->ang.z - 1;
            }
#endif
            work->pos = object->pos;
            work->scl = object->scl;
        }
    }
}

static void draw_fade(void)
{
    int alpha = fade_alpha;
    Bool translucent;

    if (alpha) {
        translucent = alpha != OPAQUE;
        fade_quad[0].col = fade_quad[1].col = fade_quad[2].col =
            fade_quad[3].col = (Uint32)alpha << ALPHA_SHIFT;
        njFogDisable();
        njDrawPolygon(fade_quad, ARRAY_COUNT(fade_quad), translucent);
        njFogEnable();
    }
}

static void kinder_room_update(struct task *task)
{
    struct task_work *work = task->work;

    switch (work->mode) {
    case ROOM_START:
        kinder_network_start();
        kinder_widgets_start();
        work->mode++;
        break;
    case ROOM_NETWORK_INFO:
        kinder_network_info = ntInfIsNetworkInfo();
        if (kinder_network_info != NTD_ERR_NOTINIT ||
            ++work->timer == NETWORK_INFO_STEPS) {
            work->timer = 0;
            work->mode++;
        }
        break;
    case ROOM_LOAD_HOUSE:
        next_room = KINDER_HOUSE;
        next_entrance = 0;
        garden_camera_start();
        garden_camera = garden_camera_create();
        load_room(task);
        texture_load_pvm("AL_STG_KINDER_OBJ_TEX", &kinder_object_texlist);
        kinder_banner_load();
        njCnkSetAmbient(0.3F, 0.1F, 0.1F);
        easy_light_set_direction(0.0F, 1.0F, 0.0F);
        njCnkSetEasyLightIntensity(1.0F, 0.2F);
        njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
        light_set(0, &main_light[0]);
        light_set(1, &fill_light);
        light_set(2, &main_light[0]);
        light_set(3, &main_light[0]);
        light_select(0, 0);
        light_select(0, 1);
        light_select_camera(0);
        light_select_after_draw(LIGHT_LAND);
        light_apply_player(0);
        music_fade_in(0);
        music_play_name("chao_k_m.adx");
        kinder_messages_load("msgChaoKinWhole", KINDER_WHOLE_BANK);
        kinder_message_reset();
        kinder_message_texture_load();
        njSetBackColor(ROOM_BACK_COLOR, ROOM_BACK_COLOR, ROOM_BACK_COLOR);
        garden_control_start();
        work->mode++;
        break;
    case ROOM_CARRIED:
        kinder_carried_chao = garden_restore_carried();
        work->mode++;
        break;
    }
    if (room_flags & LOBBY_REQUESTED) {
        njSetBackColor(BLACK, BLACK, BLACK);
        chao_change_area(CHAO_AREA_LOBBY);
    } else if (room_flags & ROOM_REQUESTED) {
        pad_disable();
        room_flags |= FADE_OUT;
        if (fade_alpha == OPAQUE) {
            room_flags &= ~(ROOM_REQUESTED | FADE_OUT);
            load_room(task);
        }
    }
    if (room_flags & FADE_OUT) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((fade_alpha += FADE_STEP) > OPAQUE) {
            fade_alpha = OPAQUE;
        }
    } else if (fade_alpha > 0) {
        /* @hack spelling: the assignment inside the test, where a statement
         * of its own would do */
        if ((fade_alpha -= FADE_STEP) < 0) {
            fade_alpha = 0;
        }
    }
}

static void kinder_room_draw(struct task *task)
{
    draw_fade();
}

static void kinder_room_stop(struct task *task)
{
    if (room_flags & PLAYER_PAUSED) {
        player_resume(0);
        room_flags &= ~PLAYER_PAUSED;
    }
    kinder_carried_chao = NULL;
    while (camera_top(0) > 1) {
        camera_remove_mode(0, camera_top(0));
    }
    kinder_banner_release();
    njReleaseTexture(&kinder_object_texlist);
    kinder_messages_release(KINDER_WHOLE_BANK);
    kinder_messages_release(KINDER_SERVICE_BANK);
    kinder_message_texture_release();
    world_land_stop();
    kinder_widgets_stop();
    kinder_network_stop();
}

void chaostgkinder_step(struct task *task)
{
    task->step = kinder_room_update;
    task->draw = kinder_room_draw;
    task->destructor = kinder_room_stop;
}

void kinder_request_room(Sint32 room, Sint32 entrance)
{
    if (!(room_flags & ROOM_REQUESTED)) {
        next_room = room;
        next_entrance = entrance;
        room_flags |= ROOM_REQUESTED;
    }
}

void kinder_request_lobby(void)
{
    room_flags |= LOBBY_REQUESTED;
}

Sint32 kinder_current_entrance(void)
{
    return current_entrance;
}
