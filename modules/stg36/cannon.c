/* MUJINGUN on Tails's route: shared/cannon_body.h's cannon. cannon.h says how
 * scl sets its range and its fire */
#include <ninja.h>

#include "shared/cannon_parts.h"
#include "stg36/cannon.h"

static struct task *shot_controller = NULL;
NJS_TEXLIST *core_cannon_texlists[] = {&cannon_shot_bright, &cannon_shot_dim,
                                       &cannon_texlist, NULL};

#define SHOT_CONTROLLER shot_controller
#define CANNON_INIT core_cannon_init
#define SHOT_HIT 0
#define SHOT_FLAGS SHAPE_UNK_800000
#include "shared/cannon_body.h" /* IWYU pragma: keep -- the code it compiles here */
