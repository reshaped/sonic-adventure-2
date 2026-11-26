#include <ninja.h>
#include <sg_xpt.h>
#include <stdlib.h>
#include "camera/camera.h"
#include "chao/area.h"
#include "chao/garden_save.h"
#include "chao/garden_scene.h"
#include "chao/messages.h"
#include "chao/race.h"
#include "chao/residents.h"
#include "collision/world.h"
#include "draw/light.h"
#include "object/set.h"
#ifdef NONMATCHING
/* garden_start_layout, by which this side's step installs the garden's
 * layout */
#include "shared/garden_layout.h"
#endif
#include "shared/garden_setup.h"
#include "sound/sound.h"
#include "system/angle.h"
#include "system/task.h"
#include "system/util.h"
#include "chao/walk_screen.h"
#include "chaostgneut/garden.h"
#include "chaostgneut/scenery.h"

static struct light neutral_lights[2] = GARDEN_LIGHTS;
/* the pond and the two falls, then the entry with a null object, where
 * garden_water_create stops */
static struct garden_water neutral_water[4] = {
#include "chaostgneut/garden/neutral_water.inc"
};
static struct set_head *neutral_layout = NULL;

static void neutral_garden_close(struct task *task);

#define HELD_SOUND_X (-100.0F)
#define HELD_SOUND_Y 0.0F
#define HELD_SOUND_Z (-100.0F)
#define HELD_SOUND_PRIORITY 1
#define HELD_SOUND_VOLUME 10
#define RANDOM_POINT_INTERVAL 900
#define RANDOM_POINT_CHANCE 0.4F
#define RANDOM_POINT_PRIORITY 1
#define RANDOM_POINT_VOLUME 0

/* the horizontal square in which neutral_ambient picks its random point: its
 * center and the length of its side */
#define SQUARE_X 25.0F
#define SQUARE_Y 0.0F
#define SQUARE_Z (-75.0F)
#define SQUARE_SIDE 150.0F
/* the duration neutral_ambient gives sound_play_at_for: LEAST_DURATION
 * updates and a random part of DURATION_SPREAD more */
#define LEAST_DURATION 120.0F
#define DURATION_SPREAD 300.0F

static void neutral_ambient(struct task *task)
{
    struct task_work *work = task->work;
    NJS_POINT3 position;

    /* @hack redundant: a switch that raises work->mode from 0 to 1. nothing
     * reads the mode */
    switch (work->mode) {
    case 0:
        work->mode++;
        break;
    }
    position.x = HELD_SOUND_X;
    position.y = HELD_SOUND_Y;
    position.z = HELD_SOUND_Z;
    sound_play_held_at(1, task, HELD_SOUND_PRIORITY, HELD_SOUND_VOLUME,
                       &position);
    if (work->timer++ % RANDOM_POINT_INTERVAL == 0 &&
        rand() * RAND_SCALE < RANDOM_POINT_CHANCE) {
        position.x = (((rand() * RAND_SCALE) - 0.5F) * SQUARE_SIDE) + SQUARE_X;
        position.y = SQUARE_Y;
        position.z = (((rand() * RAND_SCALE) - 0.5F) * SQUARE_SIDE) + SQUARE_Z;
        sound_play_at_for(3, task, RANDOM_POINT_PRIORITY, RANDOM_POINT_VOLUME,
                          LEAST_DURATION +
                              (rand() * RAND_SCALE * DURATION_SPREAD),
                          &position);
    }
}

/* the sky's origin is at the camera's x and z. its y is SKY_Y_FRACTION times
 * the camera's y, plus SKY_HEIGHT. the sky's models draw at SKY_SCALE times
 * their size */
#define SKY_Y_FRACTION 0.15F
#define SKY_HEIGHT 10.0F
#define SKY_SCALE 2.0F

static void neutral_draw_sky(struct task *task)
{
    NJS_POINT3 position;

    position = camera_views[0]->pos;

    position.y *= SKY_Y_FRACTION;
    njSetTexture(&neutral_sky_texlist);
    njPushMatrixEx();
    njTranslateEx(&position);
    njTranslate(NULL, 0.0F, SKY_HEIGHT, 0.0F);
    njScale(NULL, SKY_SCALE, SKY_SCALE, SKY_SCALE);
    njCnkEasyDrawObject(&neutral_outer_sky);
    njCnkEasyDrawObject(&neutral_inner_sky);
    njPopMatrixEx();
}

void chaostgneut_step(struct task *task)
{
#ifdef NONMATCHING
    /* this side installs the layout before the land by garden_start_layout. it
     * tries again the next step where it can't, as garden_layout.h says */
    neutral_layout = garden_start_layout("set_chao_neut.bin",
                                         &chaostgneut_objects, &neutral_land);
    if (neutral_layout == NULL) {
        return;
    }
#else
    world_land_start(&neutral_land);
    neutral_layout = garden_file_load("set_chao_neut.bin");
    set_start(&chaostgneut_objects, neutral_layout);
#endif
    GARDEN_LOAD_TEXTURES("AL_NEUT_OBJ_TEX", &neutral_garden_texlist);
    walk_load_textures();
    GARDEN_SET_LIGHTING(neutral_lights);
    world_build_land(&neutral_land,
                     (Uint32)CHAO_LAND_BUFFER(neutral_land_buffer));
    GARDEN_START_VIEW("chao_g_bgm_n.adx");
    garden_animals_start();
    garden_control_start();
    task->step = neutral_ambient;
    task->draw = neutral_draw_sky;
    task->destructor = neutral_garden_close;
    garden_load_residents();
    garden_load_trees();
    garden_restore_carried();
    garden_flag_set(GARDEN_NEUTRAL_PLAY);
    garden_flag_set(GARDEN_KINDER_OPEN);
    {
        NJS_POINT3 position = {72.0F, 10.0F, 28.0F};
        garden_transfer_machine_create(&position, 3 * EIGHTH_TURN);
    }
    if (garden_has_award(RACE_AWARD_NEUTRAL_BALL)) {
        NJS_POINT3 position = {-8.0F, 5.0F, -46.0F};
        NJS_VECTOR velocity = {0.0F, 0.0F, 0.0F};
        garden_ball_create(&position, &velocity);
    }
    if (garden_has_award(RACE_AWARD_TELEVISION)) {
        NJS_POINT3 position = {30.0F, 1.5F, -83.0F};
        garden_television_create(&position, HALF_TURN);
    }
    if (garden_has_award(RACE_AWARD_JACK_IN_BOX)) {
        NJS_POINT3 position = {9.0F, 0.0F, -116.0F};
        garden_jack_in_box_create(&position, QUARTER_TURN);
    }
    /* @hack redundant: a test of RACE_AWARD_ROCKING_HORSE whose branch only
     * initializes the local position. nothing reads that local */
    if (garden_has_award(RACE_AWARD_ROCKING_HORSE)) {
        NJS_POINT3 position = {29.0F, 0.0F, -116.0F};
        (void)position;
    }
    garden_messages_load("msgChaoOsanpo");
    garden_water_create(neutral_water);
}

static void neutral_garden_close(struct task *task)
{
    GARDEN_RELEASE_RESOURCES(neutral_layout, &neutral_garden_texlist);
    world_land_stop();
}
