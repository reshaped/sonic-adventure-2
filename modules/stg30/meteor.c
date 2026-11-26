/* Final Rush's meteors: four rocks that set files place, and the meteors the
 * background throws. a falling meteor's sparks shade to fall_spark_end_color
 * over the second half of its life */
#include <ninja.h>
#include <sg_xpt.h>

#include "collision/collision.h"
#include "stg30/meteor.h"
#include "shared/meteor_art.h"

/* the count of rock models, which scl.x picks among */
#define ROCKS 4
/* a meteor's shape takes ROCK_SHAPE_SCALE of its model's radius. rocks 0
 * and 3 take ROCK_SHAPE_MARGIN more */
#define ROCK_SHAPE_SCALE 0.7F
#define ROCK_SHAPE_MARGIN 10.0F

/* the chance that a falling meteor throws a spark at a step */
#define FALL_SPARK_CHANCE 0.8F

/* the rock scl.x picks: its whole part, taken unsigned, modulo ROCKS */
#define ROCK(work) ((Uint32)(work)->scl.x % ROCKS)

NJS_TEXLIST *meteor_texlists[6] = {
#include "stg30/meteor/meteor_texlists.inc"
};
static NJS_CNK_OBJECT *meteor_objects[ROCKS] = {
#include "stg30/meteor/meteor_objects.inc"
};
/* the rocks' sphere, moving or still. each init sets its radius by the rock */
static struct collision_shape meteor_shape = {
    0,
    SHAPE_SPHERE,
    PUSH_PLAYERS | PUSH_ENEMIES | PUSH_OBJECTS |
        PUSH_FIRM(PUSH_PLAYERS | PUSH_ENEMIES | PUSH_OBJECTS),
    0,
    0,
    {0.0F, 0.0F, 0.0F},
    35.0F,
    0.0F,
    0.0F,
    0,
    {0, 0, 0},
};
/* every meteor's spin about x and y, in degrees a step */
static Float spin_x = 0.36F;
static Float spin_y = 0.45000002F;
static Float fall_spark_size = 20.0F;
static Float fall_spark_size_range = 30.0F;
static Uint32 fall_spark_color = 0xff0070ff;
static Uint32 fall_spark_end_color = 0xff3020ff;

#define STILL_METEOR_SHAPE meteor_shape
#define ROCK_SPIN_X spin_x
#define ROCK_SPIN_Y spin_y
#define FINAL_RUSH_METEOR
#include "shared/meteor_rocks.h" /* IWYU pragma: keep -- the code it compiles here */
#include "shared/falling_meteor.h" /* IWYU pragma: keep -- the code it compiles here */
