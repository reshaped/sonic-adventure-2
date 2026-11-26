#include <ninja.h>
#include <stdlib.h>
#include "chao/garden_save.h"
#include "chao/garden_scene.h"
#include "chao/messages.h"
#include "chao/race.h"
#include "chao/residents.h"
#include "chao/walk_screen.h"
#include "chaostgdark/garden.h"
#include "chaostgdark/props.h"
#include "chaostgdark/scenery.h"
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

static struct light dark_lights[2] = GARDEN_LIGHTS;
static struct set_head *dark_layout = NULL;
static struct garden_water dark_water[3] = {
#include "chaostgdark/garden/dark_water.inc"
};

static void dark_ambient(struct task *task)
{
    struct task_work *work = task->work;

    /* @hack redundant: a switch that raises work->mode from 0 to 1. nothing
     * reads the mode */
    switch (work->mode) {
    case 0:
        work->mode++;
        break;
    }
    GARDEN_AMBIENT_SOUND(work->timer, 200, 0.1F, 3);
    GARDEN_AMBIENT_SOUND(work->timer, 220, 0.1F, 4);
    GARDEN_AMBIENT_SOUND(work->timer, 250, 0.1F, 6);
    GARDEN_AMBIENT_SOUND(work->timer, 180, 0.1F, 8);
    GARDEN_AMBIENT_SOUND(work->timer, 160, 0.1F, 9);
    GARDEN_AMBIENT_SOUND(work->timer, 230, 0.1F, 7);
    work->timer++;
}

static void dark_garden_close(struct task *task)
{
    GARDEN_RELEASE_RESOURCES(dark_layout, &dark_garden_texlist);
    world_land_stop();
}

void chaostgdark_step(struct task *task)
{
#ifdef NONMATCHING
    /* this side installs the layout before the land by garden_start_layout. it
     * tries again the next step where it can't, as garden_layout.h says */
    dark_layout = garden_start_layout("set_chao_dark.bin", &chaostgdark_objects,
                                      &dark_land);
    if (dark_layout == NULL) {
        return;
    }
#else
    world_land_start(&dark_land);
    dark_layout = garden_file_load("set_chao_dark.bin");
    set_start(&chaostgdark_objects, dark_layout);
#endif
    GARDEN_LOAD_TEXTURES("AL_DARK_OBJ_TEX", &dark_garden_texlist);
    GARDEN_SET_LIGHTING(dark_lights);
    GARDEN_START_VIEW("chao_g_bgm_d.adx");
    garden_animals_start();
    garden_control_start();
    task->step = dark_ambient;
    task->draw = NULL;
    task->destructor = dark_garden_close;
    walk_load_textures();
    garden_load_residents();
    garden_load_trees();
    garden_restore_carried();
    {
        NJS_POINT3 position = {-85.0F, 0.0F, -85.0F};
        garden_transfer_machine_create(&position, FULL_TURN - EIGHTH_TURN);
    }
    if (garden_has_award(RACE_AWARD_DARK_BALL)) {
        NJS_POINT3 position = {40.0F, 5.0F, -63.0F};
        NJS_VECTOR velocity = {0.0F, 0.0F, 0.0F};
        garden_ball_create(&position, &velocity);
    }
    if (garden_has_award(RACE_AWARD_RADIO)) {
        NJS_POINT3 position = {-30.0F, 1.5F, 9.0F};
        garden_radio_create(&position, EIGHTH_TURN / 2);
    }
    /* @hack redundant: a test of RACE_AWARD_ORGAN whose branch only
     * initializes the local position. nothing reads that local */
    if (garden_has_award(RACE_AWARD_ORGAN)) {
        NJS_POINT3 position = {-30.0F, 1.5F, 9.0F};
        (void)position;
    }
    garden_messages_load("msgChaoOsanpo");
    garden_water_create(dark_water);
}
