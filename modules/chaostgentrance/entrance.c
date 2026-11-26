#include <ninja.h>
#include <sg_xpt.h>

#include "camera/camera.h"
#include "camera/stack.h"
#include "chao/area.h"
#include "chao/garden_scene.h"
#include "chao/messages.h"
#include "chao/model.h"
#include "chao/residents.h"
#include "chaostgentrance/entrance.h"
#include "chaostgentrance/land.h"
#include "collision/world.h"
#include "draw/light.h"
#include "player/player.h"
#include "shared/object_light.h"
#include "sound/adx.h"
#include "system/angle.h"
#include "system/game_state.h"
#include "system/task.h"
#include "system/text.h"
#include "system/texture.h"

extern Uint8 entrance_land_buffer[];

/* the walk's modes before player 0 walks in. in WALK_PLACE, the player
 * stands at WALK_START_Z once work->timer passes PLACE_DELAY. it then walks
 * along -z to WALK_STOP_Z */
enum { WALK_START, WALK_PLACE };
#define PLACE_DELAY 2
#define WALK_START_Z 110.0F
#define WALK_STOP_Z (-30.0F)
#define WALK_STEP 0.52F
#define WALK_ANIM_SPEED 0.364F
/* the backdrop moves BACKDROP_FOLLOW as far as the camera does, so it seems
 * far off. it stands BACKDROP_UP above and BACKDROP_BACK behind that point */
#define BACKDROP_FOLLOW 0.7F
#define BACKDROP_UP 80.0F
#define BACKDROP_BACK 220.0F
/* the lamps stand LAMP_NEAR_X and LAMP_FAR_X either side of the hall's
 * middle, at a height of LAMP_Y */
#define LAMP_NEAR_X 23
#define LAMP_FAR_X 41
#define LAMP_Y 24

static struct light entrance_lights[] = {
    {{0.1F, -0.9F, -0.3F}, 0.9F, 0.45F, 1, 1, 1},
    {{0.3F, 0.1F, 1}, 0.5F, 0, 0.4F, 0.6F, 0.5F}};

const Sint32 *entrance_label_ids;

static void entrance_walk(struct task *task)
{
    struct task_work *work = task->work;
    switch (work->mode) {
    case WALK_START:
        work->mode++;
        break;
    case WALK_PLACE:
        player_work[0]->ang.y = -QUARTER_TURN;
        player_work[0]->ang.x = 0;
        player_work[0]->ang.z = 0;
        if (work->timer++ > PLACE_DELAY) {
            NJS_POINT3 *pos = &player_work[0]->pos;
            pos->x = 0;
            pos->y = 0;
            pos->z = WALK_START_Z;
            player_records[0]->anim_speed = WALK_ANIM_SPEED;
            work->mode++;
            /* @hack spelling: a return, where break would do */
            return;
        }
        break;
    default:
        if (player_work[0]->pos.z > WALK_STOP_Z) {
            player_set_pos(0, player_work[0]->pos.x, player_work[0]->pos.y,
                           player_work[0]->pos.z - WALK_STEP);
        } else {
            player_request(0, PLAYER_REQUEST_ANIM, ANIM_STAND);
            player_work[0]->ang.y = -QUARTER_TURN;
        }
        break;
    }
}

static void entrance_draw_backdrop(struct task *task)
{
    struct camera_place *camera = camera_views[camera_current_index];
    njSetTexture(&entrance_land_texlist);
    njPushMatrixEx();
    njTranslate(NULL, camera->pos.x * BACKDROP_FOLLOW,
                (camera->pos.y * BACKDROP_FOLLOW) + BACKDROP_UP,
                (camera->pos.z * BACKDROP_FOLLOW) - BACKDROP_BACK);
    njCnkSimpleDrawObject(&entrance_backdrop);
    njPopMatrixEx();
}

static void entrance_stop(struct task *task)
{
    garden_messages_free();
    njReleaseTexture(&entrance_common_texlist);
    njReleaseTexture(&entrance_object_texlist);
    njReleaseTexture(&entrance_rival_texlist);
    njReleaseTexture(&entrance_map_texlist);
    njReleaseTexture(&garden_animal_texlist);
    njReleaseTexture(&entrance_vms_texlist);
    switch (game_state.text_language) {
    case TEXT_JAPANESE:
        njReleaseTexture(&entrance_japanese_chars);
        njReleaseTexture(&entrance_japanese_titles);
        break;
    case TEXT_ENGLISH:
    case TEXT_FRENCH:
    case TEXT_SPANISH:
    case TEXT_GERMAN:
        njReleaseTexture(&entrance_english_chars);
        njReleaseTexture(&entrance_english_titles);
        break;
    }
    entrance_camera_stop();
    world_land_stop();
}

void chaostgentrance_step(struct task *task)
{
    music_stop();
    world_land_start(&entrance_land);
    texture_load_pvm("AL_ENT_COMMON_TEX", &entrance_common_texlist);
    texture_load_pvm("AL_ENTRANCE_OBJ", &entrance_object_texlist);
    texture_load_pvm("AL_ENT_RIVAL_TEX", &entrance_rival_texlist);
    texture_load_pvm("AL_RACE_MAP_TEX", &entrance_map_texlist);
    texture_load_pvm("AL_MINIMAL_TEX", &garden_animal_texlist);
    texture_load_pvm("AL_ENT_VMS_TEX", &entrance_vms_texlist);
    switch (game_state.text_language) {
    case TEXT_JAPANESE:
        texture_load_pvm("AL_ENT_CHAR_J_TEX", &entrance_japanese_chars);
        texture_load_pvm("AL_ENT_TITLE_J_TEX", &entrance_japanese_titles);
        entrance_label_ids = entrance_japanese_ids;
        break;
    case TEXT_ENGLISH:
    case TEXT_FRENCH:
    case TEXT_SPANISH:
    case TEXT_GERMAN:
        texture_load_pvm("AL_ENT_CHAR_E_TEX", &entrance_english_chars);
        texture_load_pvm("AL_ENT_TITLE_E_TEX", &entrance_english_titles);
        entrance_label_ids = entrance_english_ids;
        break;
    }
    njCnkSetAmbient(0.3F, 0.1F, 0.1F);
    easy_light_set_direction(0.0F, 1.0F, 0.0F);
    njCnkSetEasyLightIntensity(1.0F, 0.2F);
    njCnkSetEasyLightColor(1.0F, 1.0F, 1.0F);
    light_set(0, &entrance_lights[0]);
    light_set(1, &entrance_lights[1]);
    light_set(2, &entrance_lights[0]);
    light_set(3, &entrance_lights[0]);
    light_select(0, 0);
    light_select(0, 1);
    light_select_camera(0);
    light_select_after_draw(LIGHT_LAND);
    light_apply_player(0);
    lights_load("stg50_light.bin");
    world_build_land(&entrance_land,
                     (Uint32)CHAO_LAND_BUFFER(entrance_land_buffer));
    music_fade_in(0);
    music_play_name("chao_r_e.ADX");
    njSetBackColor(0xff3333ff, 0x003333ff, 0x00bbbbff);
    camera_set_view_angle(0, FULL_TURN / 6, 0);
    garden_control_start();
    entrance_elevator_create();
    entrance_gate_create();
    entrance_lamps_reset();
    entrance_lamp_create(-LAMP_FAR_X, LAMP_Y, HALL_END_Z);
    entrance_lamp_create(-LAMP_NEAR_X, LAMP_Y, HALL_END_Z);
    entrance_lamp_create(LAMP_NEAR_X, LAMP_Y, HALL_END_Z);
    entrance_lamp_create(LAMP_FAR_X, LAMP_Y, HALL_END_Z);
    task->step = entrance_walk;
    task->draw = entrance_draw_backdrop;
    task->destructor = entrance_stop;
    garden_restore_carried();
    task->work->tick = 16;
    player_request(0, PLAYER_REQUEST_ANIM, ANIM_WALK);
    player_work[0]->ang.y = -QUARTER_TURN;
    player_motion[0]->aim_y = -QUARTER_TURN;
    entrance_camera_start();
    garden_messages_load("msgChaoEntrance");
}
