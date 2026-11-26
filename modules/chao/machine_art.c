/* the transfer machine's art in each garden, and its task and state */
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/garden_scene.h"
#include "chao/machine.h"
#include "chao/machine_art.h"
#include "chao/machine_models.h"

NJS_TEXLIST *machine_textures[3] = {
#include "chao/machine_art/machine_textures.inc"
};
NJS_CNK_OBJECT *machine_models[3] = {
#include "chao/machine_art/machine_models.inc"
};
NJS_MOTION *machine_door_motions[3] = {
#include "chao/machine_art/machine_door_motions.inc"
};
NJS_CNK_OBJECT *machine_hero_trim[3] = {
#include "chao/machine_art/machine_hero_trim.inc"
};
NJS_MOTION *machine_hero_trim_motions[3] = {
#include "chao/machine_art/machine_hero_trim_motions.inc"
};
NJS_CNK_OBJECT *machine_buttons[3] = {
#include "chao/machine_art/machine_buttons.inc"
};
NJS_CNK_OBJECT *machine_doors[3] = {
#include "chao/machine_art/machine_doors.inc"
};
NJS_CNK_OBJECT *machine_bases[3] = {
#include "chao/machine_art/machine_bases.inc"
};
/* clang-format off */
Uint16 machine_screen_textures[3][3] = {
    {10, 11, 12},
    {25, 26, 27},
    {29, 30, 31},
};
/* clang-format on */

struct task *garden_transfer_machine = NULL;
Sint32 garden_machine_in_use = 0;
