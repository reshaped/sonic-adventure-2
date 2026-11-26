#include <mathf.h>
#include <ninja.h>
#include <sg_xpt.h>

#include "chao/perception.h"
#include "chao/work.h"
#include "chao/world.h"
#include "player/player.h"
#include "system/angle.h"
#include "system/task.h"

#ifdef NONMATCHING
/* the plain form the hack tag names: all_toys's 32 objects */
#define TOY_SLOTS(senses, group) ((senses)->tail.all_toys.objects)
#else
/* @hack type: the toy group's objects, an array of 16, where all_toys's 32
 * would do */
#define TOY_SLOTS(senses, group) ((group)->objects)
#endif

#define SENSES(chao) (&((struct chao_work *)(chao)->work)->perception)
#define NO_DISTANCE 100000.0F
#define SIGHT_RANGE 50.0F
#define SIGHT_ANGLE 0x4e38
#define HEARING_RANGE 40.0F
#define SMELL_RANGE 40.0F

static void clear_perception(struct task *chao);

void chao_perception_init(struct task *chao)
{
    struct chao_perception *senses = SENSES(chao);

    clear_perception(chao);
    senses->sight_range = SIGHT_RANGE;
    senses->sight_angle = SIGHT_ANGLE;
    senses->sight_half_angle = senses->sight_angle >> 1;
    senses->hearing_range = HEARING_RANGE;
    senses->smell_range = SMELL_RANGE;
}

static void clear_perceived_object(struct chao_perceived_object *object)
{
    object->info[0] = -1;
    object->info[1] = -1;
    object->info[2] = -1;
    object->info[3] = -1;
    object->distance = NO_DISTANCE;
    object->in_sight = 0;
    object->heard = 0;
    object->smelled = 0;
    object->entry = NULL;
}

static void clear_perceived_group(struct chao_perceived_group *group)
{
    struct chao_perceived_object *object;
    struct chao_perceived_object *end;

    group->count = 0;
    group->in_sight = 0;
    group->heard = 0;
    group->smelled = 0;
    group->nearest_distance = NO_DISTANCE;
    group->nearest = -1;
    object = group->objects;
    end = &group->objects[CHAO_PERCEPTION_CAPACITY];
    for (; object < end; object++) {
        clear_perceived_object(object);
    }
}

static void clear_perception(struct task *chao)
{
    struct chao_perception *senses = SENSES(chao);
#ifdef NONMATCHING
    /* the plain form the hack tag names: a call for each group */
    clear_perceived_group(&senses->player);
    clear_perceived_group(&senses->chao);
    clear_perceived_group(&senses->fruit);
    clear_perceived_group(&senses->tree);
    clear_perceived_group(&senses->tail.groups.toy);
    clear_perceived_group(&senses->tail.groups.sound);
#else
    /* @hack type: the six groups walked as an array, where a call for each
     * would do */
    struct chao_perceived_group *group = &senses->player;
    struct chao_perceived_group *last = &senses->tail.groups.sound;

    for (; group <= last; group++) {
        clear_perceived_group(group);
    }
#endif
}

#define HEADING(from, to)                                                      \
    ((Angle)(atan2f((to)->x - (from)->x, (to)->z - (from)->z) *                \
             ANGLE_PER_RADIAN))

static int see_player(struct task *chao)
{
    struct task_work *work = chao->work;
    NJS_POINT3 *position = &work->pos;
    Angle yaw = work->ang.y;
    struct chao_perception *senses = SENSES(chao);
    struct chao_perceived_group *group = &senses->player;
    struct chao_perceived_object *object = group->objects;
    Float range = senses->sight_range;
    Angle half_angle = senses->sight_half_angle;
    int seen;
    struct task *player;
    struct task_work *player_work;
    NJS_POINT3 *player_position;
    Float distance;
    Angle heading;
    Uint16 difference;

    /* @hack redundant: a test of range that never runs */
    if (0) {
        if (range) {
        }
    }
    clear_perceived_group(group);
    seen = 0;
    player = player_tasks[0];
    if (player != NULL) {
        player_work = player->work;
        if (player_work != NULL) {
            player_position = &player_work->pos;
            distance = njDistanceP2P(position, player_position);
            /* @hack redundant: a test of distance, twice, that never runs */
            if (0) {
                /* NOLINTNEXTLINE(misc-redundant-expression) */
                if (distance && distance) {
                }
            }
            if (distance < range) {
                heading = HEADING(position, player_position);
                difference = angle_distance(heading, yaw);
                if (difference < half_angle) {
                    group->in_sight = 1;
                    seen = group->in_sight;
                    object->in_sight = 1;
                    object->distance = distance;
                    object->entry = NULL;
                }
            }
            if (seen) {
                if (group->nearest_distance > distance) {
                    group->nearest_distance = distance;
                    group->nearest = 0;
                }
                group->count++;
            }
        }
    }
#ifdef NONMATCHING
    /* this side returns whether the group sees anything, where SHC's code ends
     * without a return. chao_notice_objects' one caller ignores the result */
    return group->in_sight;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

/* each object in sight takes the group's next entry, which nearest indexes */
#define SCAN_GARDEN(category, accepts, within_range, measure_heading)          \
    remaining = garden_object_count(category);                                 \
    while (remaining > 0) {                                                    \
        seen = 0;                                                              \
        remaining--;                                                           \
        other = garden_active_object((category), remaining);                   \
        if (accepts) {                                                         \
            other_work = other->work;                                          \
            if (other_work != NULL) {                                          \
                other_position = &other_work->pos;                             \
                distance = njDistanceP2P(position, other_position);            \
                if (within_range) {                                            \
                    measure_heading;                                           \
                    if (difference < half_angle) {                             \
                        group->in_sight = 1;                                   \
                        object->in_sight = 1;                                  \
                        seen = 1;                                              \
                        object->distance = distance;                           \
                        object->entry = other->force.p;                        \
                    }                                                          \
                }                                                              \
                if (seen) {                                                    \
                    if (group->nearest_distance > distance) {                  \
                        group->nearest_distance = distance;                    \
                        group->nearest = index;                                \
                    }                                                          \
                    group->count++;                                            \
                    object++;                                                  \
                    index++;                                                   \
                }                                                              \
            }                                                                  \
        }                                                                      \
    }

#define MEASURE_HEADING                                                        \
    heading = HEADING(position, other_position);                               \
    difference = angle_distance(heading, yaw);
#define MEASURE_TREE_HEADING                                                   \
    difference = angle_distance(HEADING(position, other_position), yaw)

static int see_chao(struct task *chao)
{
    struct task_work *work = chao->work;
    NJS_POINT3 *position = &work->pos;
    Angle yaw = work->ang.y;
    struct chao_perception *senses = SENSES(chao);
    struct chao_perceived_group *group = &senses->chao;
    struct chao_perceived_object *object = group->objects;
    Float range = senses->sight_range;
    Angle half_angle = senses->sight_half_angle;
    Sint16 index = 0;
    int seen;
    int remaining;
    struct task *other;
    struct task_work *other_work;
    NJS_POINT3 *other_position;
    Float distance;
    Angle heading;
    Uint16 difference;

    clear_perceived_group(group);
    SCAN_GARDEN(GARDEN_OBJECT_CHAO, other != NULL && other != chao,
                distance < range, MEASURE_HEADING);
#ifdef NONMATCHING
    /* this side returns whether the group sees anything, as its see_player
     * does */
    return group->in_sight;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static int see_fruit(struct task *chao)
{
    struct task_work *work = chao->work;
    Angle yaw = work->ang.y;
    struct chao_perception *senses = SENSES(chao);
    NJS_POINT3 *position = &work->pos;
    Float range = senses->sight_range;
    struct chao_perceived_group *group = &senses->fruit;
    struct chao_perceived_object *object = group->objects;
    Angle half_angle = senses->sight_half_angle;
    Sint16 index = 0;
    int seen;
    int remaining;
    struct task *other;
    struct task_work *other_work;
    NJS_POINT3 *other_position;
    Float distance;
    Angle heading;
    Uint16 difference;

    clear_perceived_group(group);
    SCAN_GARDEN(GARDEN_OBJECT_FRUIT, other != NULL, distance < range,
                MEASURE_HEADING);
#ifdef NONMATCHING
    /* this side returns whether the group sees anything, as its see_player
     * does */
    return group->in_sight;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static int see_trees(struct task *chao)
{
    struct task_work *work = chao->work;
    NJS_POINT3 *position = &work->pos;
    Angle yaw = work->ang.y;
    struct chao_perception *senses = SENSES(chao);
    struct chao_perceived_group *group = &senses->tree;
    struct chao_perceived_object *object = group->objects;
    Float range = senses->sight_range;
    Angle half_angle = senses->sight_half_angle;
    Sint16 index = 0;
    int seen;
    int remaining;
    struct task *other;
    struct task_work *other_work;
    NJS_POINT3 *other_position;
    Float distance;
    Uint16 difference;

    clear_perceived_group(group);
    SCAN_GARDEN(GARDEN_OBJECT_TREE, other != NULL, range > distance,
                MEASURE_TREE_HEADING);
    SCAN_GARDEN(GARDEN_OBJECT_GARDEN_TREE, other != NULL, range > distance,
                MEASURE_TREE_HEADING);
#ifdef NONMATCHING
    /* this side returns whether the group sees anything, as its see_player
     * does */
    return group->in_sight;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

static int see_toys(struct task *chao)
{
    struct task_work *work = chao->work;
    Angle yaw = work->ang.y;
    struct chao_perception *senses = SENSES(chao);
    NJS_POINT3 *position = &work->pos;
    Float range = senses->sight_range;
    struct chao_perceived_group *group = &senses->tail.groups.toy;
    struct chao_perceived_object *object = TOY_SLOTS(senses, group);
    Angle half_angle = senses->sight_half_angle;
    Sint16 index = 0;
    int seen;
    int remaining;
    struct task *other;
    struct task_work *other_work;
    NJS_POINT3 *other_position;
    Float distance;
    Angle heading;
    Uint16 difference;

    clear_perceived_group(group);
    /* @bug memory: a toy in sight past the group's 16th writes over the
     * sound group. nothing bounds the entry written at 0x0c515f54 and
     * advanced at 0x0c515f6c */
    SCAN_GARDEN(GARDEN_OBJECT_TOY, other != NULL, distance < range,
                MEASURE_HEADING);
#ifdef NONMATCHING
    /* this side returns whether the group sees anything, as its see_player
     * does */
    return group->in_sight;
#endif
} /* NOLINT(clang-diagnostic-return-type) */

void chao_listen(struct task *chao)
{
}

/* in SHC's code the result is undefined, since each of the five scans ends
 * without a return. chao_choose_intention, the one caller, ignores it */
int chao_notice_objects(struct task *chao)
{
    int seen = 0;

    clear_perception(chao);
    seen |= see_player(chao);
    seen |= see_chao(chao);
    seen |= see_fruit(chao);
    seen |= see_trees(chao);
    seen |= see_toys(chao);
    return seen;
}

struct chao_perceived_object *chao_visible_player(struct task *chao)
{
    struct chao_perception *senses = SENSES(chao);
    struct chao_perceived_group *group = &senses->player;

    struct chao_perceived_object *object = NULL;
    Sint16 index;

    if (group->in_sight) {
        index = group->nearest;
        if (index >= 0) {
            object = &group->objects[index];
        }
    }
    return object;
}

struct task *chao_visible_player_task(struct task *chao)
{
    struct chao_perceived_object *object = chao_visible_player(chao);

    if (object != NULL) {
        return player_tasks[0];
    }
    return NULL;
}

struct chao_perceived_object *chao_nearest_visible_fruit(struct task *chao)
{
    struct chao_perception *senses = SENSES(chao);
    struct chao_perceived_group *group = &senses->fruit;

    struct chao_perceived_object *object = NULL;
    Sint16 index;

    if (group->in_sight) {
        index = group->nearest;
        if (index >= 0) {
            object = &group->objects[index];
        }
    }
    return object;
}

struct task *chao_nearest_fruit_task(struct task *chao)
{
    struct chao_perceived_object *object = chao_nearest_visible_fruit(chao);

    struct garden_entry *entry;

    if (object != NULL) {
        entry = object->entry;
        if (entry != NULL && entry->task != NULL) {
            return entry->task;
        }
    }
    return NULL;
}

struct chao_perceived_object *chao_nearest_visible_tree(struct task *chao)
{
    struct chao_perception *senses = SENSES(chao);
    struct chao_perceived_group *group = &senses->tree;

    struct chao_perceived_object *object = NULL;
    Sint16 index;

    if (group->in_sight) {
        index = group->nearest;
        if (index >= 0) {
            object = &group->objects[index];
        }
    }
    return object;
}

struct task *chao_nearest_tree_task(struct task *chao)
{
    struct chao_perceived_object *object = chao_nearest_visible_tree(chao);

    struct garden_entry *entry;

    if (object != NULL) {
        entry = object->entry;
        if (entry != NULL && entry->task != NULL) {
            return entry->task;
        }
    }
    return NULL;
}

struct chao_perceived_object *chao_nearest_visible_toy(struct task *chao)
{
    struct chao_perception *senses = SENSES(chao);
    struct chao_perceived_group *group = &senses->tail.groups.toy;

    struct chao_perceived_object *object = NULL;
    Sint16 index;

    if (group->in_sight) {
        index = group->nearest;
        if (index >= 0) {
            object = &TOY_SLOTS(senses, group)[index];
        }
    }
    return object;
}

struct task *chao_nearest_toy_task(struct task *chao)
{
    struct chao_perceived_object *object = chao_nearest_visible_toy(chao);

    struct garden_entry *entry;

    if (object != NULL) {
        entry = object->entry;
        if (entry != NULL && entry->task != NULL) {
            return entry->task;
        }
    }
    return NULL;
}
